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

/*
 * list_less_func for ordered insertion into ready queue
 */
bool
higher_priority(const struct list_elem *a,
               const struct list_elem *b,
               void *aux);
