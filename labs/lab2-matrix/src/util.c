#include "util.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#endif

static uint64_t random_state = UINT64_C(0x9e3779b97f4a7c15);

static int matrix_element_count(int rows, int columns, size_t *count)
{
    if (rows <= 0 || columns <= 0 || count == NULL) {
        return -1;
    }
    if ((size_t)rows > SIZE_MAX / (size_t)columns) {
        return -1;
    }
    *count = (size_t)rows * (size_t)columns;
    return 0;
}

static uint64_t next_random(void)
{
    uint64_t x = random_state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    random_state = x;
    return x * UINT64_C(2685821657736338717);
}

void seed_random(unsigned int seed)
{
    random_state = (uint64_t)seed + UINT64_C(0x9e3779b97f4a7c15);
    if (random_state == 0) {
        random_state = UINT64_C(0x9e3779b97f4a7c15);
    }
}

double *allocate_matrix(int rows, int columns)
{
    size_t count;
    if (matrix_element_count(rows, columns, &count) != 0 ||
        count > SIZE_MAX / sizeof(double)) {
        fprintf(stderr, "invalid matrix size: %d x %d\n", rows, columns);
        return NULL;
    }
    return (double *)malloc(count * sizeof(double));
}

int print_matrix(const double *a, int m, int n)
{
    int i;
    int j;

    if (a == NULL || m <= 0 || n <= 0) {
        return -1;
    }
    printf("[");
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            printf(j + 1 == n ? "%8.3f" : "%8.3f, ",
                   a[(size_t)i * (size_t)n + (size_t)j]);
        }
        printf(i + 1 == m ? "]\n" : ";\n");
    }
    return 0;
}

int randomize_matrix(double *a, int m, int n)
{
    size_t count;
    size_t i;

    if (a == NULL || matrix_element_count(m, n, &count) != 0) {
        return -1;
    }
    for (i = 0; i < count; ++i) {
        const uint64_t value = next_random();
        const double magnitude = (double)(value % UINT64_C(10000)) / 100.0;
        a[i] = (value & UINT64_C(0x10000)) ? -magnitude : magnitude;
    }
    return 0;
}

int matrix_copy(double *destination, const double *source, int m, int n)
{
    size_t count;
    if (destination == NULL || source == NULL ||
        matrix_element_count(m, n, &count) != 0) {
        return -1;
    }
    memcpy(destination, source, count * sizeof(double));
    return 0;
}

int verify_matrix(const double *actual, const double *expected, int m, int n)
{
    const double absolute_tolerance = 1e-8;
    const double relative_tolerance = 1e-10;
    size_t count;
    size_t i;

    if (actual == NULL || expected == NULL ||
        matrix_element_count(m, n, &count) != 0) {
        return -1;
    }

    for (i = 0; i < count; ++i) {
        const double difference = fabs(actual[i] - expected[i]);
        const double scale = fmax(fabs(actual[i]), fabs(expected[i]));
        const double tolerance =
            absolute_tolerance + relative_tolerance * scale;

        if (!isfinite(actual[i]) || !isfinite(expected[i]) ||
            difference > tolerance) {
            fprintf(stderr,
                    "verification failed at (%zu, %zu): actual=%.17g, "
                    "expected=%.17g, abs_error=%.6e, tolerance=%.6e\n",
                    i / (size_t)n,
                    i % (size_t)n,
                    actual[i],
                    expected[i],
                    difference,
                    tolerance);
            return -1;
        }
    }
    return 0;
}

double get_sec(void)
{
#if defined(_WIN32)
    LARGE_INTEGER counter;
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)frequency.QuadPart;
#else
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return (double)now.tv_sec + 1e-9 * (double)now.tv_nsec;
#endif
}

void time_measurement(gemm function,
                      const double *a,
                      const double *b,
                      double *c,
                      int n,
                      double *elapsed_time)
{
    const double start = get_sec();
    function(a, b, c, n);
    *elapsed_time = get_sec() - start;
}

void time_measurement_block(gemm_block function,
                            const double *a,
                            const double *b,
                            double *c,
                            int n,
                            int block_size,
                            double *elapsed_time)
{
    const double start = get_sec();
    function(a, b, c, n, block_size);
    *elapsed_time = get_sec() - start;
}
