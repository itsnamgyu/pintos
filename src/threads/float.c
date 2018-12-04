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

#include "threads/float.h"
#include <stdint.h>

const int Q = 14;

int int_plus_float(int i, int f) {
  return (i << Q) + f;
}

int int_minus_float(int i, int f) {
  return (i << Q) - f;
}

int int_times_float(int i, int f) {
  return i * f;
}

int int_over_float(int i, int f) {
  return ((int64_t) i << (2 * Q)) / f;
}


int float_minus_int(int f, int i) {
  return f - (i << Q);
}

int float_over_int(int f, int i) {
  return f / i;
}

int float_plus_float(int f1, int f2) {
  return f1 + f2;
}

int float_minus_float(int f1, int f2) {
  return f1 - f2;
}

int float_times_float(int f1, int f2) {
  int64_t precise = ((int64_t) f1 * f2) >> Q;
  return (int) precise;
}

int float_over_float(int f1, int f2) {
  // int less_precise = f1 / (f2 >> Q);
  int64_t precise = (((int64_t) f1 << Q) /  f2);
  return (int) precise;
}
