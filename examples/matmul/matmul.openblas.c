#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>

// #include "mkl.h"

#ifdef PERFCTR
#include <papi.h>
#include "papi_defs.h"
#endif

#include "cblas.h"
#include "decls.h"

#ifdef TIME
#define IF_TIME(foo) foo;
#else
#define IF_TIME(foo)
#endif

void init_array()
{
    int i, j;

    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
            A[i][j] = (i + j);
            B[i][j] = (double)(i*j);
            C[i][j] = 0.0;
        }
    }
}
double rtclock()
{
    struct timezone Tzp;
    struct timeval Tp;
    int stat;
    stat = gettimeofday (&Tp, &Tzp);
    if (stat != 0) printf("Error return from gettimeofday: %d",stat);
    return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}


double t_start, t_end;

int main()
{
    int i, j, k;
    register double s;
    int LDA, LDB, LDC;
    double *a, *b, *c;
    // char transa='n', transb='n';
    LDA=M;
    LDB=N;
    LDC=M;

    init_array();

    a = (double *) malloc(sizeof(double)*M*M);
    b = (double *) malloc(sizeof(double)*M*M);
    c = (double *) malloc(sizeof(double)*M*M);

    for( i=0; i<M; i++){
        for( j=0; j<N; j++){
            a[i*M+j]= A[i][j]; 
            b[i*M+j]= B[i][j]; 
            c[i*M+j]= C[i][j]; 
        }
    }

#ifdef PERFCTR
    PERF_INIT; 
#endif

    IF_TIME(t_start = rtclock());

    cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,M,N,K,alpha,a,LDA,b,LDB,beta,c,LDC);

    IF_TIME(t_end = rtclock());
    IF_TIME(fprintf(stderr, "%0.6lfs\n", t_end - t_start));
    IF_TIME(fprintf(stdout, "%0.2lf GFLOPS\n",
                  2.0 * M * N * K / (t_end - t_start) / 1E9));

#ifdef PERFCTR
    PERF_EXIT; 
#endif

#ifdef TEST
    for( i=0; i<M; i++){
        for( j=0; j<N; j++){
            C[i][j] = c[i*M+j];
        }
    }

    print_array();
#endif
    return 0;
}
