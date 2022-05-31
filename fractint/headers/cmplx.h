/* various complex number defs */
#ifndef _CMPLX_DEFINED
#define _CMPLX_DEFINED


struct DHyperComplex {
    double x,y;
    double z,t;
};

struct LHyperComplex {
    long x,y;
    long z,t;
};

struct DComplex {
  double x,y;
};

struct LDComplex {
    double x,y;
};

typedef struct  DComplex         _CMPLX;
typedef struct  LDComplex        _LDCMPLX;
typedef struct  DHyperComplex    _HCMPLX;
typedef struct  LHyperComplex    _LHCMPLX;
#endif
