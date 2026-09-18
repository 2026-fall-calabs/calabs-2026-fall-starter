# labblas

`labblas` is a small, portable CBLAS-compatible reference backend bundled with
this teaching framework. It implements only the `cblas_dgemm` operation needed
to generate correctness-reference results for the matrix multiplication lab.

It is not intended to replace a production BLAS implementation. Its purpose is
to make the framework build offline on ordinary Linux, macOS, and Windows C
toolchains without Intel MKL or another external package.

The implementation is compiled as part of the framework and is not included in
the measured student-function time. Configure with
`-DLAB_USE_SYSTEM_BLAS=ON` only when an installed OpenBLAS/CBLAS should be used
instead.
