/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
#ifndef _DOITGEN_H
# define _DOITGEN_H

/* Default to LARGE_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
# endif

# if !defined(NQ) && !defined(NR) && !defined(NP)
/* Define sample dataset sizes. */
#  ifdef MINI_DATASET
#   define NQ 8
#   define NR 10
#   define NP 12
#  endif

#  ifdef SMALL_DATASET
#   define NQ 20
#   define NR 25
#   define NP 30
#  endif

#  ifdef MEDIUM_DATASET
#   define NQ 40
#   define NR 50
#   define NP 60
#  endif

#  ifdef LARGE_DATASET
#   define NQ 140
#   define NR 150
#   define NP 160
#  endif

#  ifdef EXTRALARGE_DATASET
#   define NQ 220
#   define NR 250
#   define NP 270
#  endif


#endif /* !(NQ NR NP) */

# define _PB_NQ POLYBENCH_LOOP_BOUND(NQ,nq)
# define _PB_NR POLYBENCH_LOOP_BOUND(NR,nr)
# define _PB_NP POLYBENCH_LOOP_BOUND(NP,np)


/* Default data type */
# if !defined(DATA_TYPE_IS_INT) && !defined(DATA_TYPE_IS_FLOAT) && !defined(DATA_TYPE_IS_DOUBLE)
#  define DATA_TYPE_IS_DOUBLE
# endif

#ifdef DATA_TYPE_IS_INT
#  define DATA_TYPE int
#  define DATA_PRINTF_MODIFIER "%d "
#endif

#if defined(CONF_BAD)
#define ANN_1 __attribute__((annotate("no_float")))
#define ANN_2 __attribute__((annotate("no_float 7 25")))
#elif defined(CONF_WORSE)
#define ANN_1
#define ANN_2 __attribute__((annotate("no_float")))
#else  // CONF_GOOD
#define ANN_1 __attribute__((annotate("no_float")))
#define ANN_2 __attribute__((annotate("no_float")))
#endif

#ifdef DATA_TYPE_IS_FLOAT
#  define DATA_TYPE1 ANN_1 float
#  define DATA_TYPE2 ANN_2 float
#  define DATA_PRINTF_MODIFIER "%0.16f "
#  define SCALAR_VAL(x) x##f
#  define SQRT_FUN(x) sqrtf(x)
#  define EXP_FUN(x) expf(x)
#  define POW_FUN(x,y) powf(x,y)
# endif

#ifdef DATA_TYPE_IS_DOUBLE
#  define DATA_TYPE1 ANN_1 double
#  define DATA_TYPE2 ANN_2 double
#  define DATA_PRINTF_MODIFIER "%0.16lf "
#  define SCALAR_VAL(x) x
#  define SQRT_FUN(x) sqrt(x)
#  define EXP_FUN(x) exp(x)
#  define POW_FUN(x,y) pow(x,y)
# endif

#endif /* !_DOITGEN_H */
