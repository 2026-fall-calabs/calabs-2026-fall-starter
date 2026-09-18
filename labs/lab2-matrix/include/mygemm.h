#ifndef CS211_LAB2_MYGEMM_H
#define CS211_LAB2_MYGEMM_H

void dgemm0(const double *a, const double *b, double *c, int n);
void dgemm1(const double *a, const double *b, double *c, int n);
void dgemm2(const double *a, const double *b, double *c, int n);
void dgemm3(const double *a, const double *b, double *c, int n);

void ijk(const double *a, const double *b, double *c, int n);
void jik(const double *a, const double *b, double *c, int n);
void kij(const double *a, const double *b, double *c, int n);
void ikj(const double *a, const double *b, double *c, int n);
void jki(const double *a, const double *b, double *c, int n);
void kji(const double *a, const double *b, double *c, int n);

void bijk(const double *a, const double *b, double *c, int n, int block_size);
void bjik(const double *a, const double *b, double *c, int n, int block_size);
void bkij(const double *a, const double *b, double *c, int n, int block_size);
void bikj(const double *a, const double *b, double *c, int n, int block_size);
void bjki(const double *a, const double *b, double *c, int n, int block_size);
void bkji(const double *a, const double *b, double *c, int n, int block_size);

void optimal(const double *a,
             const double *b,
             double *c,
             int n,
             int block_size);

#endif
