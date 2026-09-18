#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <cblas.h>

#include "mygemm.h"
#include "util.h"

static int parse_positive_int(const char *text, int *value)
{
    char *end = NULL;
    long parsed;

    errno = 0;
    parsed = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || parsed <= 0 ||
        parsed > INT_MAX) {
        return -1;
    }
    *value = (int)parsed;
    return 0;
}

int main(int argc, char **argv)
{
    int n = 2040;
    int block_size = 60;
    int exit_status = EXIT_SUCCESS;
    double elapsed_time;
    double gflops;
    double *a;
    double *b;
    double *c_initial;
    double *c;
    double *c_reference;

    if (argc > 1 && parse_positive_int(argv[1], &n) != 0) {
        fprintf(stderr, "invalid matrix dimension: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (argc > 2 && parse_positive_int(argv[2], &block_size) != 0) {
        fprintf(stderr, "invalid block size: %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    if (argc > 3) {
        fprintf(stderr, "usage: %s [matrix_dimension] [block_size]\n", argv[0]);
        return EXIT_FAILURE;
    }

    a = allocate_matrix(n, n);
    b = allocate_matrix(n, n);
    c_initial = allocate_matrix(n, n);
    c = allocate_matrix(n, n);
    c_reference = allocate_matrix(n, n);
    if (a == NULL || b == NULL || c_initial == NULL || c == NULL ||
        c_reference == NULL) {
        fprintf(stderr, "matrix allocation failed for n=%d\n", n);
        free(a);
        free(b);
        free(c_initial);
        free(c);
        free(c_reference);
        return EXIT_FAILURE;
    }

    seed_random(211U);
    randomize_matrix(a, n, n);
    randomize_matrix(b, n, n);
    randomize_matrix(c_initial, n, n);
    matrix_copy(c, c_initial, n, n);
    matrix_copy(c_reference, c_initial, n, n);

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
                c_reference,
                n);

    time_measurement_block(optimal,
                           a,
                           b,
                           c,
                           n,
                           block_size,
                           &elapsed_time);
    if (verify_matrix(c, c_reference, n, n) != 0) {
        fprintf(stderr, "error detected in optimal for n=%d, block=%d\n",
                n, block_size);
        exit_status = EXIT_FAILURE;
    }

    gflops = elapsed_time > 0.0
                 ? (2.0 * (double)n * (double)n * (double)n) /
                       (elapsed_time * 1e9)
                 : 0.0;
    if (exit_status == EXIT_SUCCESS) {
        printf("optimal n=%d, block=%d: %10.6f s, %9.3f GFLOP/s\n",
               n, block_size, elapsed_time, gflops);
    } else {
        printf("optimal n=%d, block=%d: INVALID (measured %10.6f s)\n",
               n, block_size, elapsed_time);
    }

    free(a);
    free(b);
    free(c_initial);
    free(c);
    free(c_reference);
    return exit_status;
}
