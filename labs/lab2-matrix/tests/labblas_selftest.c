#include <math.h>
#include <stdio.h>

#include <cblas.h>

static int close_enough(double actual, double expected)
{
    return fabs(actual - expected) <= 1e-12;
}

int main(void)
{
    const double a[] = {
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0
    };
    const double b[] = {
        7.0, 8.0,
        9.0, 10.0,
        11.0, 12.0
    };
    double c[] = {
        1.0, 1.0,
        1.0, 1.0
    };
    const double expected[] = {
        59.0, 65.0,
        140.0, 155.0
    };
    int i;

    cblas_dgemm(CblasRowMajor,
                CblasNoTrans,
                CblasNoTrans,
                2,
                2,
                3,
                1.0,
                a,
                3,
                b,
                2,
                1.0,
                c,
                2);

    for (i = 0; i < 4; ++i) {
        if (!close_enough(c[i], expected[i])) {
            fprintf(stderr,
                    "labblas self-test failed at %d: actual=%.17g, "
                    "expected=%.17g\n",
                    i,
                    c[i],
                    expected[i]);
            return 1;
        }
    }
    puts("labblas self-test passed");
    return 0;
}
