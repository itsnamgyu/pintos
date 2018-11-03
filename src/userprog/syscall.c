#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "lib/user/syscall.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "devices/input.h"

#include "userprog/pagedir.h"
#include "userprog/exception.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "filesys/fsutil.h"
#include "threads/malloc.h"

#define WORD sizeof(uint32_t)

static struct lock syscall_lock;

bool is_user_valid (const void *addr);
static void syscall_handler (struct intr_frame *);
static struct file *fd2file (int);

void
syscall_init (void) 
{
    lock_init(&syscall_lock);
    intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* The arguments are stacked at f->esp. */
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
    uint32_t syscall_number = *(uint32_t *) f->esp;
    void *f_esp = f->esp;

    is_user_valid (f_esp);
    switch (syscall_number)
    {
        case SYS_HALT:
            halt();
            break;

        case SYS_EXIT:
            if (!is_user_valid (f_esp + WORD))
                exit (-1);

            exit ((int) *(uint32_t *) (f_esp + WORD));
            break;

        case SYS_EXEC:
            if (!is_user_valid (f_esp + WORD))
                exit (-1);

            f->eax = exec ((const char*) *(uint32_t *) (f_esp + WORD));
            break;

        case SYS_WAIT:
            f->eax = wait ((pid_t) *(uint32_t *) (f_esp + WORD));
            break;

        case SYS_CREATE:
            if (!is_user_valid (f_esp + WORD)
                || !is_user_valid (f_esp + 2*WORD))
                exit (-1);

            f->eax = create ((const char*) *(uint32_t *) (f_esp + WORD),
                             (unsigned) *((uint32_t *) (f_esp + 2*WORD)));
            break;

        case SYS_REMOVE:
            if (!is_user_valid (f_esp + WORD))
                exit (-1);

            f->eax = remove ((const char*) *(uint32_t *) (f_esp + WORD));
            break;

        case SYS_OPEN:
            if (!is_user_valid (f_esp + WORD))
                exit (-1);

            f->eax = open ((const char*) *(uint32_t *) (f_esp + WORD));
            break;

        case SYS_FILESIZE:
            if (!is_user_valid (f_esp + WORD))
                exit (-1);

            f->eax = filesize ((int) *(uint32_t *) (f_esp + WORD));
            break;

        case SYS_READ:
            if (!is_user_valid (f_esp + WORD))
                exit (-1);

            if (!is_user_valid (f_esp + 2*WORD))
                exit (-1);

            if (!is_user_valid (f_esp + 3*WORD))
                exit (-1);

            f->eax = read ((int) *(uint32_t *) (f_esp + WORD),
                    (void *) *(uint32_t *) (f_esp + 2*WORD),
                    (unsigned) *((uint32_t *) (f_esp + 3*WORD)));
            break;

        case SYS_WRITE:
            f->eax = write ((int) *(uint32_t *) (f_esp + WORD),
                    (void*) *(uint32_t *) (f_esp + 2*WORD),
                    (unsigned) *((uint32_t *) (f_esp + 3*WORD)));
            break;

        case SYS_SEEK:
            if (!is_user_valid (f_esp + 2*WORD))
                exit (-1);

            seek ((int) *(uint32_t *) (f_esp + WORD),
                  (unsigned) *((uint32_t *) (f_esp + 2*WORD)));
            break;

        case SYS_TELL:
            if (!is_user_valid (f_esp + 2*WORD))
                exit (-1);

            f->eax = tell ((int) *(uint32_t *) (f_esp + WORD));
            break;

        case SYS_CLOSE:
            if (!is_user_valid (f_esp + WORD))
                exit (-1);

            close ((int) *(uint32_t *) (f_esp + WORD));
            break;

            /* Additional System calls. */
        case SYS_FIBONACCI:
            fibonacci ((int) *(uint32_t *) (f_esp + WORD));
            break;

        case SYS_SUM_OF_FOUR_INTEGERS:
            sum_of_four_integers ((int) *(uint32_t *) (f_esp + WORD),
                    (int) *(uint32_t *) (f_esp + 2*WORD),
                    (int) *(uint32_t *) (f_esp + 3*WORD),
                    (int) *(uint32_t *) (f_esp + 4*WORD));
            break;
    }
}

void
halt (void)
{
    /* Simply call shutdown_power_off (). */
    shutdown_power_off ();
}

void
exit (int status)
{ 
    struct thread *cur;
    cur = thread_current();

    /* Assign the exit status of thread. */
    cur->exit_status = status;

    /* Print necessary format. */
    printf("%s: exit(%d)\n", cur->name, status);

    /* thread_exit() calls process_exit(). */
    thread_exit ();
}

pid_t
exec (const char *file)
{
    struct thread *t = thread_current ();
    tid_t tid ;

    /* run process_execute() */
    tid = process_execute(file);
    
    /* Starts loading after returning from process_execute()
     * wait for child to load. */
    sema_down (&t->sema_load);

    /* In this part of the code, parent should find out whether
     * the child successfuly loaded its execute file. 
     * Consider the following cases;
     *   1) No file
     *   2) File exists, but loading failed by some reason
     *   3) File exists, and successfuly loaded.
     *
     * For each cases, we do the following
     *   1) & 2) THREAD CREATED, needs no wait but needs deallocation.
     *      return -1, deallocation done by itself in process_exit().
     *   3) Child runs.
     * */

    /* Case 1) & 2): Thread exits; it needs to be freed. */
    if (t->is_child_zombie)
    {
        t->is_child_zombie = false;
        tid = -1;
    }

    /* Case 3): Return the tid of child. */
    return tid;
}

int
wait (pid_t pid)
{
    /* No need to wait for invalid pid. */
    if (pid == -1)
        return -1;
    return process_wait(pid);
}

bool
create (const char *file, unsigned initial_size)
{
    bool result;

    if(!is_user_valid(file))
        exit (-1);

    /* Synchronization needed. */
    lock_acquire (&syscall_lock);
    result = filesys_create (file, initial_size);
    lock_release (&syscall_lock);

    return result;
}

bool
remove (const char *file)
{
    bool result;

    if(!is_user_valid(file))
        exit (-1);

    /* Synchronization needed. */
    lock_acquire (&syscall_lock);
    result = filesys_remove (file);
    lock_release (&syscall_lock);

    return result;
}

int
open (const char *file)
{
    struct file *f;
    struct thread *t = thread_current ();
    struct file_wrapper *fw;

    if (!is_user_valid(file))
        exit (-1);

    /* Synchronization needed. */
    lock_acquire (&syscall_lock);
    f = filesys_open(file);
    lock_release (&syscall_lock);

    if (!f)
        return -1;

    fw = (struct file_wrapper *) malloc (sizeof (struct file_wrapper));

    fw->fd = t->fd_next++;
    fw->f = f;

    list_push_front (&t->file_list, &fw->file_elem);
    return fw->fd;
}

int
filesize (int fd)
{
    struct file *f = fd2file (fd);

    if (!f)
        return -1;

    return file_length(f);
}

int
read (int fd, void *buffer, unsigned size)
{
    unsigned i = 0;
    int nread = 0;
    struct file *f;

    if (!is_user_valid (buffer) || !is_user_valid (buffer + size))
        exit (-1);

    /* Read from stdin */
    if (fd == 0)
    {
        for (i = 0; i < size; ++i)
            *((char*) buffer + i) = input_getc();
        return size;
    }

    /* Read from file */
    else if (fd > 1)
    {
        f = fd2file (fd);
        if (!f)
            nread = -1;

        else
        {
            /* Synchronization needed. */
            lock_acquire (&syscall_lock);
            nread = file_read (f, buffer, size);
            lock_release (&syscall_lock);
        }
    }

    /* Read from stdout?? */
    else
        nread = -1;

    return nread;
}

int
write (int fd, const void *buffer, unsigned size)
{
    int i;
    struct file *f;

    if (!is_user_valid(buffer) || !is_user_valid(buffer + size))
        exit (-1);

    if (fd < 0)
        exit (-1);
    
    /* Write to stdout. */
    else if (fd == 1)
    {
        putbuf (buffer, size);
        i = size;
    }

    /* Write to file. */
    else
    {
        f = fd2file (fd);
        if (!f)
            i = -1;
            
        /* Synchronization needed. */
        lock_acquire (&syscall_lock);
        i = file_write (f, buffer, size);
        lock_release (&syscall_lock);
    }
        return i;
}

void
seek (int fd, unsigned position)
{
    struct file *f;

    f = fd2file (fd);

    if (!f)
        return;
    else
        file_seek (f, position);
}

unsigned
tell (int fd)
{
    struct file *f;

    f = fd2file (fd);

    if (!f)
        return -1;
    else
        return file_tell (f);
}

void
close (int fd)
{
    struct thread *t;
    struct list_elem *e;
    struct file_wrapper *fw;

    t = thread_current ();
    e = list_head (&t->file_list);

    while ((e = list_next (e)) != list_end (&t->file_list))
    {
        fw = list_entry (e, struct file_wrapper, file_elem);

        if (fw->fd == fd)
        {
            file_close (fw->f);
            list_remove (e);
            free(fw);
            break;
        }
        fw = NULL;
    }
}
int
fibonacci (int n)
{
    int res = 0;
    int f1 = 1, f2 = 1;
    int i;

    if (n == 1 || n == 2)
        res = 1;
    else
        for (i = 0; i < n - 2; ++i)
        {
            res = f1 + f2;
            f1 = f2;
            f2 = res;
        }

    printf("fibonacci (%d) = %d\n", n, res);
    return res;
}

int
sum_of_four_integers (int a, int b, int c, int d)
{
    int sum = a + b + c + d;

    printf("sum_of_four_integers (%d, %d, %d, %d) = %d\n",
            a, b, c, d, sum);
    return sum;
}

/* Check if it is not pointing kernel memory space. */
bool
is_user_valid (const void *addr)
{
    if (addr 
            && is_user_vaddr (addr)
            && pagedir_get_page (thread_current ()->pagedir, addr))
        return true;
    else
        return false;
}

static struct file*
fd2file (int fd)
{
    struct thread *t = thread_current ();
    struct list_elem *e;
    struct file_wrapper *fw;

    /* find the corresponding file in file list. */
    e = list_head (&t->file_list);
    while ((e = list_next (e)) != list_end (&t->file_list))
    {
        fw = list_entry (e, struct file_wrapper, file_elem);

        if (fw->fd == fd)
            break;
        fw = NULL;
    }

    if(fw)
        return fw->f;
    else
        return NULL;
}
