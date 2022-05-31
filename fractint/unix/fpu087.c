/* Fpu087.c
 * This file contains routines to replace fpu087.asm.
 *
 * This file Copyright 1991 Ken Shirriff.  It may be used according to the
 * fractint license conditions, blah blah blah.
 */

#include <math.h>

#include "port.h"
#include "prototyp.h"

#if defined(__BORLANDC__)
#include <float.h>

static int isinf(double x)
{
    return !_isnan(x) && !_finite(x);
}

static int isnan(double x)
{
    return _isnan(x);
}

static int islessequal(double x, double y)
{
    return _isnan(x) || _isnan(y)
        ? 0
        : x < y;
}

#endif


/*
 *----------------------------------------------------------------------
 *
 * xxx086 --
 *
 *	Simulate integer math routines using floating point.
 *	This will of course slow things down, so this should all be
 *	changed at some point.
 *
 *----------------------------------------------------------------------
 */

void FPUcplxmul(_CMPLX *x, _CMPLX *y, _CMPLX *z)
{
    double tx, ty;
    if (y->y == 0.0) { /* y is real */
      tx = (double)x->x * (double)y->x;
      ty = (double)x->y * (double)y->x;
    }
    else if (x->y == 0.0) { /* x is real */
      tx = (double)x->x * (double)y->x;
      ty = (double)x->x * (double)y->y;
    }
    else {
      tx = (double)x->x * (double)y->x - (double)x->y * (double)y->y;
      ty = (double)x->x * (double)y->y + (double)x->y * (double)y->x;
    }
    if (isnan(ty) || isinf(ty))
      z->y = HUGE_VAL;
    else
      z->y = (double)ty;
    if (isnan(tx) || isinf(tx))
      z->x = HUGE_VAL;
    else
      z->x = (double)tx;
}

void FPUcplxdiv(_CMPLX *x, _CMPLX *y, _CMPLX *z)
{
   double tx,
          yx = y->x,
          yy = y->y,
          mod = hypot(yx, yy);

   if (mod == 0.0)
   {
      z->x = HUGE_VAL;
      z->y = HUGE_VAL;
      overflow = 1;
      return;
   }

   yx /= mod;
   yy /= mod;
   tx = (x->x * yx - x->y * yy) / mod;
   z->y = (x->x * yy + x->y * yx) / mod;
   z->x = tx;
}

void FPUsincos(double *Angle, double *Sin, double *Cos)
{
   if (isnan(*Angle))
   {
      *Sin = *Angle;
      *Cos = *Angle;
   }
   else if (isinf(*Angle))
   {
      *Sin = 0.0;
      *Cos = 1.0;
   }
   else
   {
      *Sin = sin(*Angle);
      *Cos = cos(*Angle);
   }
}

void FPUsinhcosh(double *Angle, double *Sinh, double *Cosh)
{
   if (isnan(*Angle))
   {
      *Sinh = *Angle;
      *Cosh = *Angle;
   }
   else if (isinf(*Angle))
   {
      *Sinh = 0.0;		/* TODO# should be NaN */
      *Cosh = 1.0;
   }
   else
   {
      *Sinh = sinh(*Angle);
      *Cosh = cosh(*Angle);
   }
}

void FPUcplxlog(_CMPLX *x, _CMPLX *z)
{
    double mod, xx, xy;
    xx = x->x;
    xy = x->y;
    if (xx == 0.0 && xy == 0.0) {
        z->x = z->y = 0.0;
        return;
    }
    else if(xy == 0.0) { /* x is real */
        z->x = log(xx);
        z->y = 0.0;
        return;
    }
    mod = xx*xx + xy*xy;
    if (isnan(mod) || islessequal(mod,0) || isinf(mod))
      z->x = 0.5;
    else
      z->x = 0.5 * log(mod);
    if (isnan(xx) || isnan(xy) || isinf(xx) || isinf(xy))
      z->y = 1.0;
    else
      z->y = atan2(xy,xx);
}

/* Integer Routines */

#define em2float(l) (*(float *)&(l))
#define float2em(f) (*(long *)&(f))

/*
 * Input is a 16 bit offset number.  Output is shifted by Fudge.
 */
unsigned long ExpFudged(long x, int Fudge)
{
    return (long) (exp((double)x/(double)(1<<16))*(double)(1<<Fudge));
}

/* This multiplies two e/m numbers and returns an e/m number. */
long r16Mul(long x, long y)
{
    float f;
    f = em2float(x)*em2float(y);
    return float2em(f);
}

/* This takes an exp/mant number and returns a shift-16 number */
long LogFloat14(unsigned long x)
{
    return log((double)em2float(x))*(1<<16);
}

/* This divides two e/m numbers and returns an e/m number. */
long RegDivFloat(long x, long y)
{
    float f;
    f = em2float(x)/em2float(y);
    return float2em(f);
}

/*
 * This routine on the IBM converts shifted integer x,FudgeFact to
 * the 4 byte number: exp,mant,mant,mant
 * Instead of using exp/mant format, we'll just use floats.
 * Note: If sizeof(float) != sizeof(long), we're hosed.
 */
long RegFg2Float(long x, int FudgeFact)
{
    float f;
    long l;
    f = x/(float)(1<<FudgeFact);
    l = float2em(f);
    return l;
}

/*
 * This converts em to shifted integer format.
 */
long RegFloat2Fg(long x, int Fudge)
{
    return em2float(x)*(float)(1<<Fudge);
}

long RegSftFloat(long x, int Shift)
{
    float f;
    f = em2float(x);
    if (Shift>0) {
	f *= (1 << Shift);
    } else {
	f /= (1 << -Shift);
    }
    return float2em(f);
}

