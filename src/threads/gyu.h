/*                       ___           ___           ___                      */
/*                      /\  \         |\__\         /\__\                     */
/*                     /::\  \        |:|  |       /:/  /                     */
/*                    /:/\:\  \       |:|  |      /:/  /                      */
/*                   /:/  \:\  \      |:|__|__   /:/  /  ___                  */
/*                  /:/__/_\:\__\     /::::\__\ /:/__/  /\__\                 */
/*                  \:\  /\ \/__/    /:/~~/~    \:\  \ /:/  /                 */
/*                   \:\ \:\__\     /:/  /       \:\  /:/  /                  */
/*                    \:\/:/  /     \/__/         \:\/:/  /                   */
/*                     \::/  /                     \::/  /                    */
/*                      \/__/                       \/__/                     */
/*                                                                            */
/*                                                                            */
/*                                                    -Created by Namgyu Ho   */

#pragma once

/* shameless imports */
#include "threads/thread.h"
#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include "threads/flags.h"
#include "threads/interrupt.h"
#include "threads/intr-stubs.h"
#include "threads/palloc.h"
#include "threads/switch.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include <limits.h>

#include "threads/float.h"

/*
 * list_less_func for ordered insertion into ready queue
 */
bool
higher_priority(const struct list_elem *a,
               const struct list_elem *b,
               void *aux);

/*
 * init load_avg, which is encapsulated in gyu
 */
void
init_load_avg(void);

void
update_load_avg(void);

int
get_load_avg(void);

/*
 * thread_action_func for updating priority according to BSD.
 * note that this function does not yield thread according to priority
 */
void
thread_update_priority(struct thread *t, void *aux);

/*
 * thread_action_func for updating recent_cpu according to BSD.
 * note that this function should be used within timer_interrupt, thus
 * is interrupt context.
 */
void
thread_update_recent_cpu(struct thread *t, void *aux);
