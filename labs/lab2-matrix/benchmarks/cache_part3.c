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
    static const char *const plain_names[] = {
        "ijk", "jik", "kij", "ikj", "jki", "kji"};
    static const gemm plain_functions[] = {ijk, jik, kij, ikj, jki, kji};
    static const char *const blocked_names[] = {
        "bijk", "bjik", "bkij", "bikj", "bjki", "bkji"};
    static const gemm_block blocked_functions[] = {
        bijk, bjik, bkij, bikj, bjki, bkji};
    const int plain_count =
        (int)(sizeof(plain_functions) / sizeof(plain_functions[0]));
    const int blocked_count =
        (int)(sizeof(blocked_functions) / sizeof(blocked_functions[0]));
    double results[12] = {0.0};
    unsigned char valid_results[12] = {0U};
    int n = 2000;
    int block_size = 10;
    int i;
    int exit_status = EXIT_SUCCESS;
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

    printf("\n*********** Cache Reuse (n=%d, block=%d) ***********\n",
           n, block_size);

    for (i = 0; i < plain_count; ++i) {
        matrix_copy(c, c_initial, n, n);
        time_measurement(plain_functions[i], a, b, c, n, &results[i]);
        if (verify_matrix(c, c_reference, n, n) != 0) {
            fprintf(stderr, "error detected in %s for n=%d\n",
                    plain_names[i], n);
            exit_status = EXIT_FAILURE;
        } else {
            valid_results[i] = 1U;
        }
    }
    for (i = 0; i < blocked_count; ++i) {
        const int result_index = plain_count + i;
        matrix_copy(c, c_initial, n, n);
        time_measurement_block(blocked_functions[i],
                               a,
                               b,
                               c,
                               n,
                               block_size,
                               &results[result_index]);
        if (verify_matrix(c, c_reference, n, n) != 0) {
            fprintf(stderr, "error detected in %s for n=%d, block=%d\n",
                    blocked_names[i], n, block_size);
            exit_status = EXIT_FAILURE;
        } else {
            valid_results[result_index] = 1U;
        }
    }

    for (i = 0; i < plain_count + blocked_count; ++i) {
        const char *name =
            i < plain_count ? plain_names[i] : blocked_names[i - plain_count];
        const double elapsed = results[i];
        const double gflops =
            elapsed > 0.0
                ? (2.0 * (double)n * (double)n * (double)n) /
                      (elapsed * 1e9)
                : 0.0;
        if (valid_results[i] != 0U) {
            printf("%-5s: %10.6f s, %9.3f GFLOP/s\n",
                   name, elapsed, gflops);
        } else {
            printf("%-5s: INVALID (measured %10.6f s)\n", name, elapsed);
        }
    }

    free(a);
    free(b);
    free(c_initial);
    free(c);
    free(c_reference);
    return exit_status;
}
