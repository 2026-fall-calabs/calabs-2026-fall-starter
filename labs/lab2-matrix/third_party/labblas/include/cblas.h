#ifndef LABBLAS_CBLAS_H
#define LABBLAS_CBLAS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Minimal declarations required by this lab. The enum values match CBLAS. */
enum CBLAS_ORDER {
    CblasRowMajor = 101,
    CblasColMajor = 102
};

enum CBLAS_TRANSPOSE {
    CblasNoTrans = 111,
    CblasTrans = 112,
    CblasConjTrans = 113
};

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
                 int ldc);

#ifdef __cplusplus
}
#endif

#endif
