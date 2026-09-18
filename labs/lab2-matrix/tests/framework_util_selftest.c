#include <stdio.h>
#include <stdlib.h>

#include <cblas.h>

#include "util.h"

static void naive_gemm(const double *a,
                       const double *b,
                       double *c,
                       int n)
{
    int i;
    int j;
    int k;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            double accumulator = c[(size_t)i * (size_t)n + (size_t)j];
            for (k = 0; k < n; ++k) {
                accumulator +=
                    a[(size_t)i * (size_t)n + (size_t)k] *
                    b[(size_t)k * (size_t)n + (size_t)j];
            }
            c[(size_t)i * (size_t)n + (size_t)j] = accumulator;
        }
    }
}

int main(void)
{
    const int n = 5;
    double elapsed = 0.0;
    double *a = allocate_matrix(n, n);
    double *b = allocate_matrix(n, n);
    double *actual = allocate_matrix(n, n);
    double *expected = allocate_matrix(n, n);

    if (a == NULL || b == NULL || actual == NULL || expected == NULL) {
        free(a);
        free(b);
        free(actual);
        free(expected);
        return 1;
    }

    seed_random(211U);
    randomize_matrix(a, n, n);
    randomize_matrix(b, n, n);
    randomize_matrix(actual, n, n);
    matrix_copy(expected, actual, n, n);

    cblas_dgemm(CblasRowMajor,
                CblasNoTrans,
                CblasNoTrans,
                n,
                n,
                n,
                1.0,
                a,
                n,
                b,
                n,
                1.0,
                expected,
                n);
    time_measurement(naive_gemm, a, b, actual, n, &elapsed);

    if (elapsed < 0.0 || verify_matrix(actual, expected, n, n) != 0) {
        fprintf(stderr, "framework correctness pipeline self-test failed\n");
        free(a);
        free(b);
        free(actual);
        free(expected);
        return 1;
    }

    free(a);
    free(b);
    free(actual);
    free(expected);
    puts("framework correctness pipeline self-test passed");
    return 0;
}
