/**************************************
**
** PORT.H : Miscellaneous definitions for portability.  Please add
** to this file for any new machines/compilers you may have.
**
** XFRACT file "SHARED.H" merged into PORT.H on 3/14/92 by --CWM--
** TW also merged in Wes Loewer's BIGPORT.H.
*/

#ifndef PORT_H          /* If this is defined, this file has been       */
#define PORT_H          /* included already in this module.             */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#if (defined(__STDC__) || defined(__cplusplus)) && !defined(STDC)
#  define STDC
#endif

#if (defined(XFRACT)) && !defined(STDC)
#  define STDC
#endif

#ifndef STDC
#  ifndef const /* cannot use !defined(STDC) && !defined(const) on Mac */
#    define const
#  endif
#endif

/* If endian.h is not present, it can be handled in the code below, */
/* but if you have this file, it can make it more fool proof. */
// TODO (jonathan#1#): Found SDL_endian.h, use SDL_BIG_ENDIAN & SDL_LIL_ENDIAN
// #include <endian.h>
#if (defined(XFRACT) && !defined(__sun))
#if defined(sgi)
#include <sys/endian.h>
#else
#include <endian.h>
#endif
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN    4321  /* to show byte order (taken from gcc) */
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

        typedef unsigned char  U8;
        typedef signed char    S8;
        typedef unsigned short U16;
        typedef short          S16;
        typedef unsigned int   U32;
        typedef int            S32;
        typedef unsigned long long U64;
        typedef long long      S64;
        typedef unsigned char  BYTE;
        typedef char           CHAR;

# ifdef BADVOID
   typedef char          *VOIDPTR;
   typedef char          *VOIDFARPTR;
   typedef char          *VOIDCONSTPTR;
# else
   typedef void          *VOIDPTR;
   typedef void          *VOIDFARPTR;
   typedef const void    *VOIDCONSTPTR;
# endif

typedef int sigfunc(int);

#ifndef BYTE_ORDER
/* change for little endians that don't have this defined elsewhere (endian.h) */
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* ifndef BYTE_ORDER */

#ifndef USE_BIGNUM_C_CODE
#define USE_BIGNUM_C_CODE 1
#endif
#ifndef BIG_ANSI_C
#define BIG_ANSI_C 1
#endif

#ifndef XFRACT
 #define CONST          const
 #define SLASHC         '\\'
 #define SLASH          "\\"
 #define SLASHSLASH     "\\\\"
 #define SLASHDOT       "\\."
 #define DOTSLASH       ".\\"
 #define DOTDOTSLASH    "..\\"
 #define READMODE       "rb"    /* Correct DOS text-mode        */
 #define WRITEMODE      "wb"    /* file open "feature".         */
#else
 #define CONST          const
 #define SLASHC         '/'
 #define SLASH          "/"
 #define SLASHSLASH     "//"
 #define SLASHDOT       "/."
 #define DOTSLASH       "./"
 #define DOTDOTSLASH    "../"
 #define READMODE       "r"
 #define WRITEMODE      "w"
#endif

#define write1(ptr,len,n,stream) (fputc(*(ptr),stream),1)
#define write2(ptr,len,n,stream) (fputc((*(ptr))&255,stream),fputc((*(ptr))>>8,stream),1)
#define rand15() (rand()&0x7FFF)

#include "unix.h"


/* The following FILE_* #defines were moved here from fractint.h to
 * avoid inconsistent declarations in dos_help/hc.c and unix/unix.c. */

/* these are used to declare arrays for file names */
#if 1 /* XFRACT */
#define FILE_MAX_PATH  256       /* max length of path+filename  */
#define FILE_MAX_DIR   256       /* max length of directory name */
#else
#define FILE_MAX_PATH  80       /* max length of path+filename  */
#define FILE_MAX_DIR   80       /* max length of directory name */
#endif
#define FILE_MAX_DRIVE  3       /* max length of drive letter   */

#if 1
#define FILE_MAX_FNAME  9       /* max length of filename       */
#define FILE_MAX_EXT    5       /* max length of extension      */
#else
/*
The filename limits were increased in Xfract 3.02. But alas,
in this poor program that was originally developed on the
nearly-brain-dead DOS operating system, quite a few things
in the UI would break if file names were bigger than DOS 8-3
names. So for now humor us and let's keep the names short.
*/
#define FILE_MAX_FNAME  64       /* max length of filename       */
#define FILE_MAX_EXT    64       /* max length of extension      */
#endif

#define MAX_NAME FILE_MAX_FNAME+FILE_MAX_EXT-1

struct DIR_SEARCH               /* Allocate DTA and define structure */
{
     char path[21];             /* DOS path and filespec */
     char attribute;            /* File attributes wanted */
     int  ftime;                /* File creation time */
     int  fdate;                /* File creation date */
     long size;                 /* File size in bytes */
     char filename[MAX_NAME+1];         /* Filename and extension */
};
extern struct DIR_SEARCH DTA;   /* Disk Transfer Area */

/* Uses big_access32(), big_set32(),... functions instead of macros. */
/* Some little endian machines may require this as well. */
#if BYTE_ORDER == BIG_ENDIAN
#define ACCESS_BY_BYTE
#endif

#ifdef LOBYTEFIRST
#define GET16(c,i)    (i) = *((U16*)(&(c)))
#else
#define GET16(c,i)    (i) = (*(unsigned char *)&(c))+\
                      ((*((unsigned char*)&(c)+1))<<8)
#endif

/* Some compiler libraries don't correctly handle long double.*/
/* If you want to force the use of doubles, or                */
/* if the compiler supports long doubles, but does not allow  */
/*   scanf("%Lf", &longdoublevar);                            */
/* to read in a long double, then uncomment this next line    */
/* #define DO_NOT_USE_LONG_DOUBLE */
#ifndef XFRACT
#define DO_NOT_USE_LONG_DOUBLE
#endif

#ifndef DO_NOT_USE_LONG_DOUBLE
#ifdef LDBL_DIG
/* this is what we're hoping for */
#define USE_LONG_DOUBLE
        typedef long double LDBL;
#else
#define DO_NOT_USE_LONG_DOUBLE
#endif /* #ifdef LDBL_DIG */
#endif /* #ifndef DO_NOT_USE_LONG_DOUBLE */


#ifdef DO_NOT_USE_LONG_DOUBLE

#ifdef USE_LONG_DOUBLE
#undef USE_LONG_DOUBLE
#endif

/* long double isn't supported */
/* impliment LDBL as double */
        typedef double          LDBL;

#if !defined(LDBL_DIG)
#define LDBL_DIG        DBL_DIG        /* # of decimal digits of precision */
#endif
#if !defined(LDBL_EPSILON)
#define LDBL_EPSILON    DBL_EPSILON    /* smallest such that 1.0+LDBL_EPSILON != 1.0 */
#endif
#if !defined(LDBL_MANT_DIG)
#define LDBL_MANT_DIG   DBL_MANT_DIG   /* # of bits in mantissa */
#endif
#if !defined(LDBL_MAX)
#define LDBL_MAX        DBL_MAX        /* max value */
#endif
#if !defined(LDBL_MAX_10_EXP)
#define LDBL_MAX_10_EXP DBL_MAX_10_EXP /* max decimal exponent */
#endif
#if !defined(LDBL_MAX_EXP)
#define LDBL_MAX_EXP    DBL_MAX_EXP    /* max binary exponent */
#endif
#if !defined(LDBL_MIN)
#define LDBL_MIN        DBL_MIN        /* min positive value */
#endif
#if !defined(LDBL_MIN_10_EXP)
#define LDBL_MIN_10_EXP DBL_MIN_10_EXP /* min decimal exponent */
#endif
#if !defined(LDBL_MIN_EXP)
#define LDBL_MIN_EXP    DBL_MIN_EXP    /* min binary exponent */
#endif
#if !defined(LDBL_RADIX)
#define LDBL_RADIX      DBL_RADIX      /* exponent radix */
#endif
#if !defined(LDBL_ROUNDS)
#define LDBL_ROUNDS     DBL_ROUNDS     /* addition rounding: near */
#endif

#define asinl           asin
#define atanl           atan
#define atan2l          atan2
#define ceill           ceil
#define cosl            cos
#define expl            exp
#define fabsl           fabs
#define floorl          floor
#define logl            log
#define log10l          log10
#define sinl            sin
#define sqrtl           sqrt
#endif /* #ifdef DO_NOT_USE_LONG_DOUBLE */

#endif  /* PORT_H */