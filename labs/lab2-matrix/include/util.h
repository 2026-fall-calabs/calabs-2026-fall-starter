#ifndef CS211_LAB2_UTIL_H
#define CS211_LAB2_UTIL_H

#include <stddef.h>

typedef void (*gemm)(const double *a, const double *b, double *c, int n);
typedef void (*gemm_block)(const double *a,
                           const double *b,
                           double *c,
                           int n,
                           int block_size);

void seed_random(unsigned int seed);
int randomize_matrix(double *a, int m, int n);
int matrix_copy(double *destination, const double *source, int m, int n);
int verify_matrix(const double *actual, const double *expected, int m, int n);
int print_matrix(const double *a, int m, int n);

double get_sec(void);
void time_measurement(gemm function,
                      const double *a,
                      const double *b,
                      double *c,
                      int n,
                      double *elapsed_time);
void time_measurement_block(gemm_block function,
                            const double *a,
                            const double *b,
                            double *c,
                            int n,
                            int block_size,
                            double *elapsed_time);

double *allocate_matrix(int rows, int columns);

#endif
