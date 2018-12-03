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

#include "threads/gyu.h"

/*
 * list_less_func for ordered insertion into ready queue
 */
bool
higher_priority(const struct list_elem *left,
               const struct list_elem *right,
               void *aux) {
  struct thread *left_thread = list_entry(left, struct thread, elem);
  struct thread *right_thread = list_entry(right, struct thread, elem);
  return left_thread->priority > right_thread->priority;
}
