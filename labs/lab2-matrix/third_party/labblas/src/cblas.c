#include "cblas.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static void invalid_argument(const char *message)
{
    fprintf(stderr, "labblas cblas_dgemm: %s\n", message);
    abort();
}

static double matrix_get(const double *x,
                         int row,
                         int column,
                         int leading_dimension,
                         enum CBLAS_ORDER order)
{
    if (order == CblasRowMajor) {
        return x[(size_t)row * (size_t)leading_dimension + (size_t)column];
    }
    return x[(size_t)column * (size_t)leading_dimension + (size_t)row];
}

static double op_get(const double *x,
                     int row,
                     int column,
                     int leading_dimension,
                     enum CBLAS_ORDER order,
                     enum CBLAS_TRANSPOSE transpose)
{
    if (transpose == CblasNoTrans) {
        return matrix_get(x, row, column, leading_dimension, order);
    }
    return matrix_get(x, column, row, leading_dimension, order);
}

void cblas_dgemm(enum CBLAS_ORDER order,
                 enum CBLAS_TRANSPOSE trans_a,
                 enum CBLAS_TRANSPOSE trans_b,
                 int m,
                 int n,
                 int k,
                 double alpha,
                 const double *a,
                 int lda,
                 const double *b,
                 int ldb,
                 double beta,
                 double *c,
                 int ldc)
{
    int i;
    int j;
    int p;

    if (order != CblasRowMajor && order != CblasColMajor) {
        invalid_argument("unsupported storage order");
    }
    if ((trans_a != CblasNoTrans && trans_a != CblasTrans &&
         trans_a != CblasConjTrans) ||
        (trans_b != CblasNoTrans && trans_b != CblasTrans &&
         trans_b != CblasConjTrans)) {
        invalid_argument("unsupported transpose value");
    }
    if (m < 0 || n < 0 || k < 0 || lda <= 0 || ldb <= 0 || ldc <= 0) {
        invalid_argument("invalid matrix dimension or leading dimension");
    }
    if ((m > 0 && n > 0 && c == NULL) ||
        (m > 0 && k > 0 && a == NULL) ||
        (k > 0 && n > 0 && b == NULL)) {
        invalid_argument("null matrix pointer");
    }

    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            size_t c_index;
            if (order == CblasRowMajor) {
                c_index = (size_t)i * (size_t)ldc + (size_t)j;
            } else {
                c_index = (size_t)j * (size_t)ldc + (size_t)i;
            }
            c[c_index] = beta == 0.0 ? 0.0 : beta * c[c_index];
        }
    }

    if (alpha == 0.0 || k == 0) {
        return;
    }

    /* Fast path for the exact operation used by this lab. Keeping the
     * reference portable does not require making correctness checks slow. */
    if (order == CblasRowMajor && trans_a == CblasNoTrans &&
        trans_b == CblasNoTrans) {
        for (i = 0; i < m; ++i) {
            double *const c_row = c + (size_t)i * (size_t)ldc;
            const double *const a_row = a + (size_t)i * (size_t)lda;
            for (p = 0; p < k; ++p) {
                const double scaled_a = alpha * a_row[p];
                const double *const b_row = b + (size_t)p * (size_t)ldb;
                for (j = 0; j < n; ++j) {
                    c_row[j] += scaled_a * b_row[j];
                }
            }
        }
        return;
    }

    /* This generic implementation supports the standard modes needed for a
     * reference result. Its time is never included in student measurements. */
    for (i = 0; i < m; ++i) {
        for (p = 0; p < k; ++p) {
            const double scaled_a =
                alpha * op_get(a, i, p, lda, order, trans_a);
            for (j = 0; j < n; ++j) {
                size_t c_index;
                if (order == CblasRowMajor) {
                    c_index = (size_t)i * (size_t)ldc + (size_t)j;
                } else {
                    c_index = (size_t)j * (size_t)ldc + (size_t)i;
                }
                c[c_index] +=
                    scaled_a * op_get(b, p, j, ldb, order, trans_b);
            }
        }
    }
}
