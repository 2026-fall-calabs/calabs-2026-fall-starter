#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <cblas.h>

#include "mygemm.h"
#include "util.h"

static int parse_dimension(const char *text, int *value)
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
    static const int default_dimensions[] = {66, 126, 258, 510, 1026, 2046};
    static const char *const function_names[] = {
        "dgemm0", "dgemm1", "dgemm2", "dgemm3"};
    static const gemm functions[] = {dgemm0, dgemm1, dgemm2, dgemm3};
    const int function_count = (int)(sizeof(functions) / sizeof(functions[0]));
    const int *dimensions = default_dimensions;
    int dimension_count =
        (int)(sizeof(default_dimensions) / sizeof(default_dimensions[0]));
    int *command_line_dimensions = NULL;
    double *results = NULL;
    unsigned char *valid_results = NULL;
    int dimension_index;
    int function_index;
    int exit_status = EXIT_SUCCESS;

    if (argc > 1) {
        dimension_count = argc - 1;
        command_line_dimensions =
            (int *)malloc((size_t)dimension_count * sizeof(int));
        if (command_line_dimensions == NULL) {
            fprintf(stderr, "failed to allocate dimension list\n");
            return EXIT_FAILURE;
        }
        for (dimension_index = 0; dimension_index < dimension_count;
             ++dimension_index) {
            if (parse_dimension(argv[dimension_index + 1],
                                &command_line_dimensions[dimension_index]) != 0) {
                fprintf(stderr, "invalid matrix dimension: %s\n",
                        argv[dimension_index + 1]);
                free(command_line_dimensions);
                return EXIT_FAILURE;
            }
        }
        dimensions = command_line_dimensions;
    }

    results = (double *)calloc((size_t)dimension_count *
                                   (size_t)function_count,
                               sizeof(double));
    if (results == NULL) {
        fprintf(stderr, "failed to allocate result table\n");
        free(command_line_dimensions);
        return EXIT_FAILURE;
    }
    valid_results = (unsigned char *)calloc(
        (size_t)dimension_count * (size_t)function_count,
        sizeof(unsigned char));
    if (valid_results == NULL) {
        fprintf(stderr, "failed to allocate validity table\n");
        free(results);
        free(command_line_dimensions);
        return EXIT_FAILURE;
    }

    seed_random(211U);
    printf("\n*********** Register Reuse ***********\n");

    for (dimension_index = 0; dimension_index < dimension_count;
         ++dimension_index) {
        const int n = dimensions[dimension_index];
        double *a = allocate_matrix(n, n);
        double *b = allocate_matrix(n, n);
        double *c_initial = allocate_matrix(n, n);
        double *c = allocate_matrix(n, n);
        double *c_reference = allocate_matrix(n, n);

        if (a == NULL || b == NULL || c_initial == NULL || c == NULL ||
            c_reference == NULL) {
            fprintf(stderr, "matrix allocation failed for n=%d\n", n);
            free(a);
            free(b);
            free(c_initial);
            free(c);
            free(c_reference);
            exit_status = EXIT_FAILURE;
            break;
        }

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

        for (function_index = 0; function_index < function_count;
             ++function_index) {
            double *elapsed =
                &results[(size_t)dimension_index * (size_t)function_count +
                         (size_t)function_index];
            matrix_copy(c, c_initial, n, n);
            time_measurement(functions[function_index], a, b, c, n, elapsed);
            if (verify_matrix(c, c_reference, n, n) != 0) {
                fprintf(stderr, "error detected in %s for n=%d\n",
                        function_names[function_index], n);
                exit_status = EXIT_FAILURE;
            } else {
                valid_results[(size_t)dimension_index *
                                  (size_t)function_count +
                              (size_t)function_index] = 1U;
            }
        }

        free(a);
        free(b);
        free(c_initial);
        free(c);
        free(c_reference);
    }

    for (function_index = 0; function_index < function_count; ++function_index) {
        for (dimension_index = 0; dimension_index < dimension_count;
             ++dimension_index) {
            const int n = dimensions[dimension_index];
            const double elapsed =
                results[(size_t)dimension_index * (size_t)function_count +
                        (size_t)function_index];
            const int is_valid =
                valid_results[(size_t)dimension_index *
                                  (size_t)function_count +
                              (size_t)function_index] != 0U;
            const double gflops =
                elapsed > 0.0
                    ? (2.0 * (double)n * (double)n * (double)n) /
                          (elapsed * 1e9)
                    : 0.0;
            if (is_valid) {
                printf("%-7s n=%4d: %10.6f s, %9.3f GFLOP/s\n",
                       function_names[function_index], n, elapsed, gflops);
            } else {
                printf("%-7s n=%4d: INVALID (measured %10.6f s)\n",
                       function_names[function_index], n, elapsed);
            }
        }
    }

    free(results);
    free(valid_results);
    free(command_line_dimensions);
    return exit_status;
}
