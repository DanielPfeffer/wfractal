#ifndef FRACTYPE_H
#define FRACTYPE_H

#define SIN             0
#define COS             1   /* Beware this is really COSXX */
#define SINH            2
#define COSH            3
#define EXP             4
#define LOG             5
#define SQR             6
#define TAN            10

/* These MUST match the corresponding fractalspecific record in fractals.c */
enum FRACTAL_TYPE
{
    NOFRACTAL               = -1,
    MANDELFP                = 0,
    JULIAFP,
    NEWTBASIN,
    LAMBDAFP,
    NEWTON,
    PLASMA,
    MANDELTRIGFP,
    MANOWARFP,
    TEST,
    SIERPINSKIFP,
    BARNSLEYM1FP,
    BARNSLEYJ1FP,
    BARNSLEYM2FP,
    BARNSLEYJ2FP,
    SQRTRIGFP,
    TRIGPLUSTRIGFP,
    MANDELLAMBDAFP,
    MARKSMANDELFP,
    MARKSJULIAFP,
    UNITYFP,
    MANDEL4FP,
    JULIA4FP,
    IFS,
    IFS3D,
    BARNSLEYM3FP,
    BARNSLEYJ3FP,
    TRIGSQRFP,
    BIFURCATION,
    TRIGXTRIGFP,
    SQR1OVERTRIGFP,
    ZXTRIGPLUSZFP,
    KAMFP,
    KAM3DFP,
    LAMBDATRIGFP,
    FPMANTRIGPLUSZSQRD,
    FPJULTRIGPLUSZSQRD,
    FPMANDELZPOWER,
    FPJULIAZPOWER,
    FPMANZTOZPLUSZPWR,
    FPJULZTOZPLUSZPWR,
    FPMANTRIGPLUSEXP,
    FPJULTRIGPLUSEXP,
    FPPOPCORN,
    FPLORENZ,
    FPLORENZ3D,
    COMPLEXNEWTON,
    COMPLEXBASIN,
    COMPLEXMARKSMAND,
    COMPLEXMARKSJUL,
    FFORMULA,
    JULIBROTFP,
    FPROSSLER,
    FPHENON,
    FPPICKOVER,
    FPGINGERBREAD,
    DIFFUSION,
    SPIDERFP,
    TETRATEFP,
    MAGNET1M,
    MAGNET1J,
    MAGNET2M,
    MAGNET2J,
    BIFLAMBDA,
    BIFADSINPI,
    BIFEQSINPI,
    FPPOPCORNJUL,
    LSYSTEM,
    MANOWARJFP,
    FNPLUSFNPIXFP,
    MARKSMANDELPWRFP,
    TIMSERRORFP,
    BIFSTEWART,
    FPHOPALONG,
    FPCIRCLE,
    FPMARTIN,
    LYAPUNOV,
    FPLORENZ3D1,
    FPLORENZ3D3,
    FPLORENZ3D4,
    FPLAMBDAFNFN,
    FPJULFNFN,
    FPMANLAMFNFN,
    FPMANFNFN,
    BIFMAY,
    HALLEY,
    DYNAMICFP,
    QUATFP,
    QUATJULFP,
    CELLULAR,
    INVERSEJULIAFP,
    MANDELCLOUD,
    PHOENIXFP,
    MANDPHOENIXFP,
    HYPERCMPLXFP,
    HYPERCMPLXJFP,
    FROTHFP,
    ICON,
    ICON3D,
    PHOENIXCPLX,
    PHOENIXFPCPLX,
    MANDPHOENIXCPLX,
    MANDPHOENIXFPCPLX,
    ANT,
    CHIP,
    QUADRUPTWO,
    THREEPLY,
    VL,
    ESCHER,
    LATOO,
/* #define MANDELBROTMIX4         172 */
    DIVIDEBROT5
};

#define LAMBDASINE               8 /* obsolete */
#define LAMBDACOS                9 /* obsolete */
#define LAMBDAEXP               10 /* obsolete */
#define MANDELSINE              17 /* obsolete */
#define MANDELCOS               18 /* obsolete */
#define MANDELEXP               19 /* obsolete */
#define DEMM                    30 /* obsolete */
#define DEMJ                    31 /* obsolete */
#define MANDELSINH              33 /* obsolete */
#define LAMBDASINH              34 /* obsolete */
#define MANDELCOSH              35 /* obsolete */
#define LAMBDACOSH              36 /* obsolete */
#define LMANDELSINE             37 /* obsolete */
#define LLAMBDASINE             38 /* obsolete */
#define LMANDELCOS              39 /* obsolete */
#define LLAMBDACOS              40 /* obsolete */
#define LMANDELSINH             41 /* obsolete */
#define LLAMBDASINH             42 /* obsolete */
#define LMANDELCOSH             43 /* obsolete */
#define LLAMBDACOSH             44 /* obsolete */
#define LMANDELEXP              49 /* obsolete */
#define LLAMBDAEXP              50 /* obsolete */
#endif
