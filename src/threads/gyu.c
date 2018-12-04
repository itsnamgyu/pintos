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

static int load_avg;

bool
higher_priority(const struct list_elem *left,
               const struct list_elem *right,
               void *aux UNUSED) {
  struct thread *left_thread = list_entry(left, struct thread, elem);
  struct thread *right_thread = list_entry(right, struct thread, elem);
  return left_thread->priority > right_thread->priority;
}

void
init_load_avg(void) {
  load_avg = 0;
}

void
update_load_avg(void) {
  int f_59 = (59 << Q) / 60;
  int f_01 = (1 << Q) / 60;

  int f0 = float_times_float(f_59, load_avg);
  int f1 = f_01 * ready_threads();

  load_avg = f0 + f1;
}

int
get_load_avg() {
  return load_avg;
}

void
thread_update_priority(struct thread *t, void *aux UNUSED) {
  int f_max = PRI_MAX << Q;
  int f_r4 = t->recent_cpu / 4;
  int f_n2 = t->nice * (2 << Q);

  t->priority = (f_max - f_r4 - f_n2) >> Q;

  if (t->priority < PRI_MIN) t->priority = PRI_MIN;
  else if (PRI_MAX < t->priority) t->priority = PRI_MAX;
}

void
thread_update_recent_cpu(struct thread *t,
                         void *aux UNUSED) {
  int f_coef = float_over_float(2 * load_avg, 2 * load_avg + (1 << Q));
  int f_term = float_times_float(f_coef, t->recent_cpu);
  int f_nice = t->nice * (1 << Q);
  t->recent_cpu = f_term + f_nice;
}

