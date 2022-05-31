/* PARSERFP.C  -- Part of FRACTINT fractal drawer.  */

/*   By Chuck Ebbert  CompuServe [76306,1226]  */
/*                     internet: 76306.1226@compuserve.com  */

/* Fast floating-point parser code.  The functions beginning with  */
/*    "fStk" are in PARSERA.ASM.  PARSER.C calls this code after  */
/*    it has parsed the formula.  */

/*   Converts the function pointers/load pointers/store pointers  */
/*       built by parsestr() into an optimized array of function  */
/*       pointer/operand pointer pairs.  */

/*   As of 31 Dec 93, also generates executable code in memory.  */
/*       Define the varible COMPILER to generate executable code.  */
/*       COMPILER must also be defined in PARSERA.ASM. */


/* Revision history:  */

/* 15 Mar 1997 TIW  */
/*    Fixed if/else bug, replaced stopmsg with pstopmsg */

/* 09 Mar 1997 TIW/GGM  */
/*    Added support for if/else */

/* 30 Jun 1996 TIW  */
/*    Removed function names if TESTFP not defined to save memory      */
/*    Function fStkFloor added to support new 'floor' function         */
/*    Function fStkCeil  added to support new 'ceil'  function         */
/*    Function fStkTrunc added to support new 'trunc' function         */
/*    Function fStkRound added to support new 'round' function         */

/* 15 Feb 1995 CAE  */
/*    added safety tests to pointer conversion code  */
/*    added the capability for functions to require 4 free regs  */

/* 8 Feb 1995 CAE  */
/*    Comments changed.  */

/* 8 Jan 1995 JCO  */
/*    Function fStkASin added to support new 'asin' function in v19    */
/*    Function fStkASinh added to support new 'asinh' function in v19  */
/*    Function fStkACos added to support new 'acos' function in v19    */
/*    Function fStkACosh added to support new 'acosh' function in v19  */
/*    Function fStkATan added to support new 'atan' function in v19    */
/*    Function fStkATanh added to support new 'atanh' function in v19  */
/*    Function fStkSqrt added to support new 'sqrt' function in v19    */
/*    Function fStkCAbs added to support new 'cabs' function in v19    */
/*    Added support for a third parameter p3    */

/* 31 Dec 1993 CAE  */
/*    Fixed optimizer bug discovered while testing compiler.  */

/* 29 Dec 1993 CAE  */
/*    Added compiler.  */

/* 04 Dec 1993 CAE  */
/*    Added optimizations for LodImagAdd/Sub/Mul.  */

/* 06 Nov 1993 CAE  */
/*    Added optimizer support for LodRealPwr and ORClr2 functions.  */
/*    If stack top is a real, a simpler sqr() or mod() fn will be  */
/*          called (fStkSqr3() was added.)  */
/*    The identities x^0=1, x^1=x, and x^-1=recip(x) are now used by the  */
/*          optimizer.  (fStkOne() was added for this.)  */

/* 31 Oct 1993 CAE  */
/*    Optimizer converts '2*x' and 'x*2' to 'x+x'. */
/*        "     recognizes LastSqr as a real if not stored to.  */

/* 9 Oct 1993 CAE  */
/*    Functions are now converted via table search.                    */
/*    Added "real" stack count variable and debug msgs for stack size. */
/*    Added optimizer extension for commutative multiply.              */
/*    P1, P2 will be treated as consts if they are never stored to.    */
/*    Function fStkStoClr2 now emitted for sto,clr with 2 on stack.    */
/*       "     fStkZero added to support new 'zero' function in v18    */
/*    Added optimization for x^2 -> sqr(x).                            */
/*    Changed "stopmsg" to "DBUGMSG" and made all macros upper case.   */
/*       (debugflag=324 now needed for debug msgs to print.)           */

/* 12 July 1993 (for v18.1) by CAE to fix optimizer bug  */

/* 22 MAR 1993 (for Fractint v18.0)  */

/* ******************************************************************* */
/*                                                                     */
/*  (c) Copyright 1992-1995 Chuck Ebbert.  All rights reserved.        */
/*                                                                     */
/*    This code may be freely distributed and used in non-commercial   */
/*    programs provided the author is credited either during program   */
/*    execution or in the documentation, and this copyright notice     */
/*    is left intact.  Sale of this code, or its use in any commercial */
/*    product requires permission from the author.  Nominal            */
/*    distribution and handling fees may be charged by shareware and   */
/*    freeware distributors.                                           */
/*                                                                     */
/* ******************************************************************* */

/* Uncomment the next line to enable debug messages.  */
/* #define TESTFP 1 */

/* Use startup parameter "debugflag=324" to show debug messages after  */
/*    compiling with above #define uncommented.  */

#include <string.h>
#include <ctype.h>
#include <time.h>

  /* see Fractint.c for a description of the "include"  hierarchy */
#include "port.h"
#include "prototyp.h"

/* global data  */
struct fls far *pfls = (struct fls far *)0;

#ifndef XFRACT /* --  */

/* not moved to PROTOTYPE.H because these only communicate within
   PARSER.C and other parser modules */

extern union Arg *Arg1, *Arg2;
extern double _1_, _2_;
extern union Arg s[20], far * far *Store, far * far *Load;
extern int StoPtr, LodPtr, OpPtr;
extern unsigned int vsp, LastOp;
extern struct ConstArg far *v;
extern int InitLodPtr, InitStoPtr, InitOpPtr, LastInitOp;
extern void (far * far *f)(void);
extern JUMP_CONTROL_ST far *jump_control;
extern int uses_jump, jump_index;

typedef void OLD_FN(void);  /* old C functions  */

OLD_FN  StkLod, StkClr, StkSto, EndInit, StkJumpLabel;
OLD_FN  dStkAdd, dStkSub, dStkMul, dStkDiv;
OLD_FN  dStkSqr, dStkMod;
OLD_FN  dStkSin, dStkCos, dStkSinh, dStkCosh, dStkCosXX;
OLD_FN  dStkTan, dStkTanh, dStkCoTan, dStkCoTanh;
OLD_FN  dStkLog, dStkExp, dStkPwr;
OLD_FN  dStkLT, dStkLTE;
OLD_FN  dStkFlip, dStkReal, dStkImag;
OLD_FN  dStkConj, dStkNeg, dStkAbs;
OLD_FN  dStkRecip, StkIdent;
OLD_FN  dStkGT, dStkGTE, dStkNE, dStkEQ;
OLD_FN  dStkAND, dStkOR;
OLD_FN  dStkZero;
OLD_FN  dStkSqrt;
OLD_FN  dStkASin, dStkACos, dStkASinh, dStkACosh;
OLD_FN  dStkATanh, dStkATan;
OLD_FN  dStkCAbs;
OLD_FN  dStkFloor;
OLD_FN  dStkCeil;
OLD_FN  dStkTrunc;
OLD_FN  dStkRound;
OLD_FN  StkJump, dStkJumpOnTrue, dStkJumpOnFalse;
OLD_FN  dStkOne;

typedef void (near NEW_FN)(void);  /* new 387-only ASM functions  */

NEW_FN  fStkPull2;  /* pull up fpu stack from 2 to 4  */
NEW_FN  fStkPush2;  /* push down fpu stack from 8 to 6  */
NEW_FN  fStkPush2a;  /* push down fpu stack from 6 to 4  */
NEW_FN  fStkPush4;  /* push down fpu stack from 8 to 4  */
NEW_FN  fStkLodDup;  /* lod, dup  */
NEW_FN  fStkLodSqr;  /* lod, sqr, dont save magnitude(i.e. lastsqr)  */
NEW_FN  fStkLodSqr2;  /* lod, sqr, save lastsqr  */
NEW_FN  fStkStoDup;  /* store, duplicate  */
NEW_FN  fStkStoSqr;  /* store, sqr, save lastsqr  */
NEW_FN  fStkStoSqr0;  /* store, sqr, dont save lastsqr  */
NEW_FN  fStkLodDbl;  /* load, double  */
NEW_FN  fStkStoDbl;  /* store, double  */
NEW_FN  fStkReal2;  /* fast ver. of real  */
NEW_FN  fStkSqr;  /* sqr, save magnitude in lastsqr  */
NEW_FN  fStkSqr0;  /* sqr, no save magnitude  */
NEW_FN  fStkClr1;  /* clear fpu  */
NEW_FN  fStkClr2;  /* test stack top, clear fpu  */
NEW_FN  fStkStoClr1;  /* store, clr1  */
NEW_FN  fStkAdd, fStkSub;
NEW_FN  fStkSto, fStkSto2;  /* fast ver. of sto  */
NEW_FN  fStkLod, fStkEndInit;
NEW_FN  fStkMod, fStkMod2;  /* faster mod  */
NEW_FN  fStkLodMod2, fStkStoMod2;
NEW_FN  fStkLTE, fStkLodLTEMul, fStkLTE2, fStkLodLTE;
NEW_FN  fStkLodLTE2, fStkLodLTEAnd2;
NEW_FN  fStkLT, fStkLodLTMul, fStkLT2, fStkLodLT;
NEW_FN  fStkLodLT2;
NEW_FN  fStkGTE, fStkLodGTE, fStkLodGTE2;
NEW_FN  fStkGT, fStkGT2, fStkLodGT, fStkLodGT2;
NEW_FN  fStkEQ, fStkLodEQ, fStkNE, fStkLodNE;
NEW_FN  fStkAND, fStkANDClr2, fStkOR, fStkORClr2;
NEW_FN  fStkSin, fStkSinh, fStkCos, fStkCosh, fStkCosXX;
NEW_FN  fStkTan, fStkTanh, fStkCoTan, fStkCoTanh;
NEW_FN  fStkLog, fStkExp, fStkPwr;
NEW_FN  fStkMul, fStkDiv;
NEW_FN  fStkFlip, fStkReal, fStkImag, fStkRealFlip, fStkImagFlip;
NEW_FN  fStkConj, fStkNeg, fStkAbs, fStkRecip;
NEW_FN  fStkLodReal, fStkLodRealC, fStkLodImag;
NEW_FN  fStkLodRealFlip, fStkLodRealAbs;
NEW_FN  fStkLodRealMul, fStkLodRealAdd, fStkLodRealSub, fStkLodRealPwr;
NEW_FN  fStkLodImagMul, fStkLodImagAdd, fStkLodImagSub;  /* CAE 4Dec93  */
NEW_FN  fStkLodImagFlip, fStkLodImagAbs;
NEW_FN  fStkLodConj;
NEW_FN  fStkLodAdd, fStkLodSub, fStkLodSubMod, fStkLodMul;
NEW_FN  fStkPLodAdd, fStkPLodSub;  /* push-lod-add/sub  */
NEW_FN  fStkIdent;
NEW_FN  fStkStoClr2;  /* store, clear stack by popping  */
NEW_FN  fStkZero;  /* to support new parser fn.  */
NEW_FN  fStkDbl;  /* double the stack top  CAE 31OCT93  */
NEW_FN  fStkOne, fStkSqr3;  /* sqr3 is sqr/mag of a real  CAE 09NOV93  */
NEW_FN  fStkSqrt;
NEW_FN  fStkASin, fStkACos, fStkASinh, fStkACosh;
NEW_FN  fStkATanh, fStkATan;
NEW_FN  fStkCAbs;
NEW_FN  fStkFloor, fStkCeil, fStkTrunc, fStkRound; /* rounding functions */
NEW_FN  fStkJump, fStkJumpOnTrue, fStkJumpOnFalse, fStkJumpLabel; /* flow */
NEW_FN  fStkOne;   /* to support new parser fn.  */

/* check to see if a const is being loaded  */
/* the really awful hack below gets the first char of the name  */
/*    of the variable being accessed  */
/* if first char not alpha, or const p1, p2, or p3 are being accessed  */
/*    then this is a const.  */
#define IS_CONST(x) \
      (!isalpha(**(((char * far *)x ) - 2 ) ) \
      || (x==&PARM1 && p1const ) \
      || (x==&PARM2 && p2const ) \
      || (x==&PARM3 && p3const ) \
      || (x==&PARM4 && p4const ) \
      || (x==&PARM5 && p5const ) )

/* is stack top a real?  */
#define STACK_TOP_IS_REAL \
      ( prevfptr == fStkReal || prevfptr == fStkReal2 \
      || prevfptr == fStkLodReal || prevfptr == fStkLodRealC \
      || prevfptr == fStkLodRealAbs \
      || prevfptr == fStkImag || prevfptr == fStkLodImag )

/* remove push operator from stack top  */
#define REMOVE_PUSH --cvtptrx, stkcnt+=2

#define CLEAR_STK 127
#define FNPTR(x) pfls[(x)].function  /* function pointer */
#define OPPTR(x) pfls[(x)].operand   /* operand pointer */
#define NO_OPERAND (union Arg near *)0
#define NO_FUNCTION (void (near *)(void))0
#define LASTSQR v[4].a
#define PARM1 v[1].a
#define PARM2 v[2].a
#define PARM3 v[8].a
#define PARM4 v[17].a
#define PARM5 v[18].a
#define MAX_STACK 8   /* max # of stack register avail  */

#ifdef TESTFP
int pstopmsg(int x,char far *msg)
{
   static FILE *fp = NULL;
   if(fp == NULL)
      fp = fopen("fpdebug.txt","w");
   if(fp)
   {
#ifndef XFRACT
      fprintf(fp,"%Fs\n",msg);
#else
      fprintf(fp,"%s\n",msg);
#endif
      fflush(fp);
   }
   return(x); /* just to quiet warnings */
}

#define stopmsg pstopmsg

#define DBUGMSG(x,y) if (debugflag==324 || debugflag==322 ) stopmsg((x), (y))
#define DBUGMSG1(x,y,p) \
      if (debugflag==324 || debugflag==322 ){ \
         sprintf(cDbgMsg, (y), (p) ); \
         stopmsg((x), cDbgMsg ); \
      }
#define DBUGMSG2(x,y,p,q) \
      if (debugflag==324 || debugflag==322 ){ \
         sprintf(cDbgMsg, (y), (p), (q) ); \
         stopmsg((x), cDbgMsg ); \
      }
#define DBUGMSG3(x,y,p,q,r) \
      if (debugflag==324 || debugflag==322 ){ \
         sprintf(cDbgMsg, (y), (p), (q), (r) ); \
         stopmsg((x), cDbgMsg ); \
      }
#define DBUGMSG4(x,y,p,q,r,s) \
      if (debugflag==324 || debugflag==322 ){ \
         sprintf(cDbgMsg, (y), (p), (q), (r), (s) ); \
         stopmsg((x), cDbgMsg ); \
      }
#define FNAME(a,b,c,d,e,f) a,b,c,d,e,f    /* use the function name string */
#else

#define DBUGMSG(x,y)
#define DBUGMSG1(x,y,p)
#define DBUGMSG2(x,y,p,q)
#define DBUGMSG3(x,y,p,q,r)
#define DBUGMSG4(x,y,p,q,r,s)
#define FNAME(a,b,c,d,e,f) b,c,d,e,f    /* don't use the function name string */
#endif /* TESTFP */

#define FN_LOD            0
#define FN_CLR            1
#define FN_ADD            2
#define FN_SUB            3
#define FN_MUL            4
#define FN_DIV            5
#define FN_STO            6
#define FN_SQR            7
#define FN_ENDINIT        8
#define FN_MOD            9
#define FN_LTE           10
#define FN_SIN           11
#define FN_COS           12
#define FN_SINH          13
#define FN_COSH          14
#define FN_COSXX         15
#define FN_TAN           16
#define FN_TANH          17
#define FN_COTAN         18
#define FN_COTANH        19
#define FN_LOG           20
#define FN_EXP           21
#define FN_PWR           22
#define FN_LT            23
#define FN_FLIP          24
#define FN_REAL          25
#define FN_IMAG          26
#define FN_CONJ          27
#define FN_NEG           28
#define FN_ABS           29
#define FN_RECIP         30
#define FN_IDENT         31
#define FN_GT            32
#define FN_GTE           33
#define FN_NE            34
#define FN_EQ            35
#define FN_AND           36
#define FN_OR            37
#define FN_ZERO          38
#define FN_SQRT          39
#define FN_ASIN          40
#define FN_ACOS          41
#define FN_ASINH         42
#define FN_ACOSH         43
#define FN_ATANH         44
#define FN_ATAN          45
#define FN_CABS          46
#define FN_FLOOR         47
#define FN_CEIL          48
#define FN_TRUNC         49
#define FN_ROUND         50
#define FN_JUMP          51
#define FN_JUMP_ON_TRUE  52
#define FN_JUMP_ON_FALSE 53
#define FN_JUMP_LABEL    54
#define FN_ONE           55


/* number of "old" functions in the table.  */
/* these are the ones that the parser outputs  */

#define LAST_OLD_FN   FN_ONE
#define NUM_OLD_FNS   LAST_OLD_FN + 1

/* total number of functions in the table.  */

#define LAST_FN          FN_ONE
#define NUM_FNS          LAST_FN + 1

#ifdef TESTFP
static char cDbgMsg[255];
#endif  /* TESTFP  */


#endif /* XFRACT  */
