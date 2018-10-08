#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "lib/user/syscall.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "userprog/process.h"

#define WORD sizeof(uint32_t)

bool is_user_valid (void *addr);
static void syscall_handler (struct intr_frame *);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* The arguments are stacked from f->esp. */
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
			  exit(-1);

		  exit((int) *(uint32_t *) (f_esp + WORD));
		  break;

	  case SYS_EXEC:
		  if (!is_user_valid (f_esp + WORD))
			  exit(-1);

		  f->eax = exec ((const char*) *(uint32_t *) (f_esp + WORD));
		  break;

	  case SYS_WAIT:
		  f->eax = wait((pid_t) *(uint32_t *) (f_esp + WORD));
		  break;

	  case SYS_CREATE:
		  break;

	  case SYS_REMOVE:
		  break;

	  case SYS_OPEN:
		  break;

	  case SYS_FILESIZE:
		  break;

	  case SYS_READ:
		  if (!is_user_valid (f_esp + WORD))
			  exit(-1);

		  if (!is_user_valid (f_esp + 2*WORD))
			  exit(-1);

		  if (!is_user_valid (f_esp + 3*WORD))
			  exit(-1);

		  read ((int) *(uint32_t *) (f_esp + WORD),
				(void *) *(uint32_t *) (f_esp + 2*WORD),
				(unsigned) *((uint32_t *) (f_esp + 3*WORD)));
		  break;

	  case SYS_WRITE:
		  f->eax = write ((int) *(uint32_t *) (f_esp + WORD),
				          (void*) *(uint32_t *) (f_esp + 2*WORD),
						  (unsigned) *((uint32_t *) (f_esp + 3*WORD)));
		  break;

	  case SYS_SEEK:
		  break;

	  case SYS_TELL:
		  break;

	  case SYS_CLOSE:
		  break;

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
	shutdown_power_off ();
}

void
exit (int status)
{
	thread_current ()->exit_status = status;
	printf("%s: exit(%d)\n", thread_name (), status);

	/* thread_exit() calls process_exit(). */
	thread_exit ();
}

pid_t exec (const char *file)
{
	return process_execute(file);
}

int wait (pid_t pid)
{
	return process_wait(pid);
}

int read (int fd, void *buffer, unsigned length)
{
	unsigned i = 0;

	if (!fd)
		while (((char *) buffer)[i] != '\0' && ++i < length);

	return i;
}

int write (int fd, const void *buffer, unsigned length)
{
	if (fd)
	{
		putbuf (buffer, length);
		return length;
	}

	else
		return -1;
}

bool is_user_valid (void *addr)
{
	if (!is_user_vaddr (addr))
		return false;
	else
		return true;
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
	printf("sum_of_four_integers(%d, %d, %d, %d) = %d",
			a, b, c, d, a + b + c + d);
	return a + b + c + d;
}
