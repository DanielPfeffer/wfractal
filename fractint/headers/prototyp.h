#ifndef PROTOTYP_H
#define PROTOTYP_H

/* includes needed to define the prototypes */

#include "mpmath.h"
#include "big.h"
#include "fractint.h"
#include "helpcom.h"
#include "externs.h"

/* maintain the common prototypes in this file
 * split the dos/win/unix prototypes into separate files.
 */

#ifdef XFRACT
#include "unixprot.h"
#endif
long calcmandfpasm_c(void);

#ifdef WINFRACT
#include "winprot.h"
#endif

#if (!defined(XFRACT) && !defined(WINFRACT))
#include "dosprot.h"
#endif

int _fstrnicmp(char const far* string1, char const far* string2, size_t maxlen);

/*  calcmand -- assembler file prototypes */

extern long cdecl calcmandasm(void);

/*  calmanfp -- assembler file prototypes */

extern void cdecl calcmandfpasmstart(void);
/* extern long  cdecl calcmandfpasm(void); */
extern long  cdecl calcmandfpasm_287(void);
extern long  cdecl calcmandfpasm_87(void);
extern long (*calcmandfpasm)(void);

/*  fpu087 -- assembler file prototypes */

extern void cdecl FPUcplxmul(_CMPLX *, _CMPLX *, _CMPLX *);
extern void cdecl FPUcplxdiv(_CMPLX *, _CMPLX *, _CMPLX *);
extern void cdecl FPUsincos(double *, double *, double *);
extern void cdecl FPUsinhcosh(double *, double *, double *);
extern void cdecl FPUcplxlog(_CMPLX *, _CMPLX *);
extern void cdecl SinCos086(long , long *, long *);
extern void cdecl SinhCosh086(long , long *, long *);
extern long far cdecl r16Mul(long , long );
extern long far cdecl RegFloat2Fg(long , int );
extern long cdecl Exp086(long);
extern unsigned long far cdecl ExpFudged(long , int );
extern long far cdecl RegDivFloat(long , long );
extern long far cdecl LogFudged(unsigned long , int );
extern long far cdecl LogFloat14(unsigned long );
extern long far cdecl RegFg2Float(long , int );
extern long far cdecl RegSftFloat(long , int );

/*  fpu387 -- assembler file prototypes */

void cdecl FPUaptan387(double *, double *, double *);
extern void cdecl FPUcplxexp387(_CMPLX *, _CMPLX *);

/*  fracsuba -- assembler file prototypes */

extern int near asmlMODbailout(void);
extern int near asmlREALbailout(void);
extern int near asmlIMAGbailout(void);
extern int near asmlORbailout(void);
extern int near asmlANDbailout(void);
extern int near asmlMANHbailout(void);
extern int near asmlMANRbailout(void);
extern int near asm386lMODbailout(void);
extern int near asm386lREALbailout(void);
extern int near asm386lIMAGbailout(void);
extern int near asm386lORbailout(void);
extern int near asm386lANDbailout(void);
extern int near asm386lMANHbailout(void);
extern int near asm386lMANRbailout(void);
extern int FManOWarfpFractal( void );
extern int FJuliafpFractal( void );
extern int FBarnsley1FPFractal( void );
extern int FBarnsley2FPFractal( void );
extern int FLambdaFPFractal( void );
extern int near asmfpMODbailout(void);
extern int near asmfpREALbailout(void);
extern int near asmfpIMAGbailout(void);
extern int near asmfpORbailout(void);
extern int near asmfpANDbailout(void);
extern int near asmfpMANHbailout(void);
extern int near asmfpMANRbailout(void);

/*  newton -- assembler file prototypes */

extern int cdecl    NewtonFractal2( void );
extern void cdecl   invertz2(_CMPLX *);

/*  tplus_a -- assembler file prototypes */

extern void WriteTPlusBankedPixel(int, int, unsigned long);
extern unsigned long ReadTPlusBankedPixel(int, int);

/*  3d -- C file prototypes */

extern void identity(MATRIX);
extern void mat_mul(MATRIX,MATRIX,MATRIX);
extern void scale(double ,double ,double ,MATRIX);
extern void xrot(double ,MATRIX);
extern void yrot(double ,MATRIX);
extern void zrot(double ,MATRIX);
extern void trans(double ,double ,double ,MATRIX);
extern int cross_product(VECTOR,VECTOR,VECTOR);
extern int normalize_vector(VECTOR);
extern int vmult(VECTOR,MATRIX,VECTOR);
extern void mult_vec(VECTOR);
extern int perspective(VECTOR);
extern int longvmultpersp(LVECTOR,LMATRIX,LVECTOR,LVECTOR,LVECTOR,int);
extern int longpersp(LVECTOR,LVECTOR,int );
extern int longvmult(LVECTOR,LMATRIX,LVECTOR,int );

/*  biginit -- C file prototypes */

/* CAE removed static functions from header 28 Jan 95  */

void free_bf_vars(void);
bn_t alloc_stack(size_t size);
int save_stack(void);
void restore_stack(int old_offset);
void init_bf_dec(int dec);
void init_bf_length(int bnl);
void init_big_pi(void);


/*  calcfrac -- C file prototypes */

extern int calcfract(void);
extern int calcmand(void);
extern int calcmandfp(void);
extern int StandardFractal(void);
extern int test(void);
extern int plasma(void);
extern int diffusion(void);
extern int Bifurcation(void );
extern int BifurcLambda(void);
extern int BifurcSetTrigPi(void);
extern int BifurcAddTrigPi(void);
extern int BifurcMay(void);
extern int BifurcMaySetup(void);
extern int BifurcLambdaTrig(void);
extern int BifurcVerhulstTrig(void);
extern int BifurcStewartTrig(void);
extern int popcorn(void);
extern int lyapunov(void);
extern int lya_setup(void);
extern int cellular(void);
extern int CellularSetup(void);
extern int calcfroth(void);
extern int froth_per_pixel(void);
extern int froth_per_orbit(void);
extern int froth_setup(void);
extern int logtable_in_extra_ok(void);
extern int find_alternate_math(int, int);

/*  cmdfiles -- C file prototypes */

extern int cmdfiles(int ,char **);
extern int load_commands(FILE *);
extern void set_3d_defaults(void);
extern int get_curarg_len(char *curarg);
extern int get_max_curarg_len(char *floatvalstr[], int totparm);
extern int init_msg(int,char *,char far *,int);
extern int cmdarg(char *curarg,int mode);
extern int getpower10(double x);
extern void dopause(int);

/*  decoder -- C file prototypes */

extern short decoder(short );
extern void set_byte_buff(BYTE *ptr);

/*  diskvid -- C file prototypes */

extern int pot_startdisk(void);
extern int targa_startdisk(FILE *,int );
extern void enddisk(void);
#ifndef XFRACT
extern int readdisk(unsigned int, unsigned int );
extern void writedisk(unsigned int, unsigned int, unsigned int );
#else
extern int readdisk(int, int );
extern void writedisk(int, int, int );
#endif
extern void targa_readdisk(unsigned int ,unsigned int ,BYTE *,BYTE *,BYTE *);
extern void targa_writedisk(unsigned int ,unsigned int ,BYTE ,BYTE ,BYTE );
extern void dvid_status(int ,char far *);
extern int  _fastcall common_startdisk(long, long, int);
extern int FromMemDisk(long,int,void far *);
extern int ToMemDisk(long,int,void far *);

/*  editpal -- C file prototypes */

extern void EditPalette(void );
extern VOIDPTR mem_alloc(unsigned size);
void putrow(int x, int y, int width, char *buff);
void getrow(int x, int y, int width, char *buff);
void mem_init(VOIDPTR block, unsigned size);
/* void hline(int x, int y, int width, int color); */
int Cursor_WaitKey(void);
void Cursor_CheckBlink(void);
#ifdef XFRACT
void Cursor_StartMouseTracking(void);
void Cursor_EndMouseTracking(void);
#endif
void clip_putcolor(int x, int y, int color);
int clip_getcolor(int x, int y);
BOOLEAN Cursor_Construct (void);
void Cursor_Destroy (void);
void Cursor_SetPos (int x, int y);
void Cursor_Move (int xoff, int yoff);
int Cursor_GetX (void);
int Cursor_GetY (void);
void Cursor_Hide (void);
void Cursor_Show (void);
extern void displayc(int, int, int, int, int);

/*  encoder -- C file prototypes */

extern int savetodisk(char *);
extern int encoder(void);
extern int _fastcall new_to_old(int new_fractype);

/*  evolve -- C file prototypes */

extern  void initgene(void);
extern  void param_history(int);
extern  int get_variations(void);
extern  int get_evolve_Parms(void);
extern  void set_current_params(void);
extern  void fiddleparms(GENEBASE gene[], int ecount);
extern  void set_evolve_ranges(void);
extern  void set_mutation_level(int);
extern  void drawparmbox(int);
extern  void spiralmap(int);
extern  int unspiralmap(void);
extern  int explore_check(void);
extern  void SetupParamBox(void);
extern  void ReleaseParamBox(void);

/*  f16 -- C file prototypes */

extern FILE *t16_open(char *,int *,int *,int *,U8 *);
extern int t16_getline(FILE *,int ,U16 *);

/*  fracsubr -- C file prototypes */

extern void calcfracinit(void);
extern void adjust_corner(void);
#ifndef USE_VARARGS
extern int put_resume(int ,... );
extern int get_resume(int ,... );
#else
extern int put_resume();
extern int get_resume();
#endif
extern int alloc_resume(int ,int );
extern int start_resume(void);
extern void end_resume(void);
extern void sleepms(long );
extern void reset_clock(void);
extern void iplot_orbit(long ,long ,int );
extern void plot_orbit(double ,double ,int );
extern void scrub_orbit(void);
extern int add_worklist(int ,int, int ,int ,int ,int ,int ,int );
extern void tidy_worklist(void);
extern void get_julia_attractor(double ,double );
extern int ssg_blocksize(void);
extern void _fastcall symPIplot(int ,int ,int );
extern void _fastcall symPIplot2J(int ,int ,int );
extern void _fastcall symPIplot4J(int ,int ,int );
extern void _fastcall symplot2(int ,int ,int );
extern void _fastcall symplot2Y(int ,int ,int );
extern void _fastcall symplot2J(int ,int ,int );
extern void _fastcall symplot4(int ,int ,int );
extern void _fastcall symplot2basin(int ,int ,int );
extern void _fastcall symplot4basin(int ,int ,int );
extern void _fastcall noplot(int ,int ,int );
extern void fractal_floattobf(void);
extern void adjust_cornerbf(void);
extern void set_grid_pointers(void);
extern void fill_dx_array(void);
extern void fill_lx_array(void);
extern int snd_open(void);
extern void w_snd(int);
extern void snd_time_write(void);
extern void close_snd(void);

/*  fractalp -- C file prototypes */

extern int typehasparm(int,int,char *);
extern int paramnotused(int);

/*  fractals -- C file prototypes */

extern void FloatPreCalcMagnet2(void);
extern void cpower(_CMPLX *,int ,_CMPLX *);
extern int Barnsley1FPFractal(void);
extern int Barnsley2FPFractal(void);
extern int JuliafpFractal(void);
extern int LambdaFPFractal(void);
extern int SierpinskiFPFractal(void);
extern int LambdaexponentFractal(void);
extern int FloatTrigPlusExponentFractal(void);
extern int MarksLambdafpFractal(void);
extern int UnityfpFractal(void);
extern int Mandel4fpFractal(void);
extern int floatZtozPluszpwrFractal(void);
extern int floatZpowerFractal(void);
extern int floatCmplxZpowerFractal(void);
extern int Barnsley3FPFractal(void);
extern int TrigPlusZsquaredfpFractal(void);
extern int Richard8fpFractal(void);
extern int PopcornFractal(void);
extern int PopcornFractal_Old(void);
extern int PopcornFractalFn(void);
extern int MarksCplxMand(void );
extern int SpiderfpFractal(void );
extern int TetratefpFractal(void);
extern int ZXTrigPlusZfpFractal(void);
extern int ScottZXTrigPlusZfpFractal(void);
extern int SkinnerZXTrigSubZfpFractal(void);
extern int Sqr1overTrigfpFractal(void);
extern int TrigPlusTrigfpFractal(void);
extern int ScottTrigPlusTrigfpFractal(void);
extern int SkinnerTrigSubTrigfpFractal(void);
extern int TrigXTrigfpFractal(void);
extern int TrigPlusSqrfpFractal(void);
extern int ScottTrigPlusSqrfpFractal(void);
extern int SkinnerTrigSubSqrfpFractal(void);
extern int TrigZsqrdfpFractal(void);
extern int SqrTrigfpFractal(void);
extern int Magnet1Fractal(void);
extern int Magnet2Fractal(void);
extern int LambdaTrigfpFractal(void);
extern int LambdaTrigfpFractal1(void);
extern int LambdaTrigfpFractal2(void);
extern int ManOWarfpFractal(void);
extern int MarksMandelPwrfpFractal(void);
extern int TimsErrorfpFractal(void);
extern int CirclefpFractal(void);
extern int VLfpFractal(void);
extern int EscherfpFractal(void);
extern int marksmandelfp_per_pixel(void);
extern int marks_mandelpwrfp_per_pixel(void);
extern int mandelfp_per_pixel(void);
extern int juliafp_per_pixel(void);
extern int otherrichard8fp_per_pixel(void);
extern int othermandelfp_per_pixel(void);
extern int otherjuliafp_per_pixel(void);
extern int MarksCplxMandperp(void );
extern int LambdaTrigOrTrigfpFractal(void);
extern int JuliaTrigOrTrigfpFractal(void);
extern int HalleyFractal(void);
extern int Halley_per_pixel(void);
extern int dynamfloat(double *,double *,double*);
extern int mandelcloudfloat(double *,double *,double*);
extern int dynam2dfloat(void);
extern int QuaternionFPFractal(void);
extern int quaternionfp_per_pixel(void);
extern int quaternionjulfp_per_pixel(void);
extern int PhoenixFractal(void);
extern int phoenix_per_pixel(void);
extern int mandphoenix_per_pixel(void);
extern int HyperComplexFPFractal(void);
extern int PhoenixFractalcplx(void);
extern int (near *floatbailout)(void);
extern int (near *bignumbailout)(void);
extern int (near *bigfltbailout)(void);
extern int near fpMODbailout(void);
extern int near fpREALbailout(void);
extern int near fpIMAGbailout(void);
extern int near fpORbailout(void);
extern int near fpANDbailout(void);
extern int near fpMANHbailout(void);
extern int near fpMANRbailout(void);
extern int near bnMODbailout(void);
extern int near bnREALbailout(void);
extern int near bnIMAGbailout(void);
extern int near bnORbailout(void);
extern int near bnANDbailout(void);
extern int near bnMANHbailout(void);
extern int near bnMANRbailout(void);
extern int near bfMODbailout(void);
extern int near bfREALbailout(void);
extern int near bfIMAGbailout(void);
extern int near bfORbailout(void);
extern int near bfANDbailout(void);
extern int near bfMANHbailout(void);
extern int near bfMANRbailout(void);
extern int ant(void);
extern int PhoenixFractal(void);
extern int PhoenixFractalcplx(void);
extern int PhoenixPlusFractal(void);
extern int PhoenixMinusFractal(void);
extern int PhoenixCplxPlusFractal(void);
extern int PhoenixCplxMinusFractal(void);
extern int phoenix_per_pixel(void);
extern int mandphoenix_per_pixel(void);
extern void set_pixel_calc_functions(void);
extern int MandelbrotMix4fp_per_pixel(void);
extern int MandelbrotMix4fpFractal(void);
extern int MandelbrotMix4Setup(void);
extern int DivideBrot5fp_per_pixel(void);
extern int DivideBrot5fpFractal(void);
extern int DivideBrot5Setup(void);

/*  fractint -- C file prototypes */

#ifdef XFRACT
extern int main(int argc,char **argv );
#else
extern void main(int argc,char **argv );
#endif
extern int elapsed_time(int);

/*  framain2 -- C file prototypes */

extern int big_while_loop(int *,char *,int);
extern int check_key(void);
extern int cmp_line(BYTE *,int );
extern int key_count(int);
extern int main_menu_switch(int *,int *,int *,char *,int);
extern int pot_line(BYTE *,int );
extern int sound_line(BYTE *,int );
#ifdef XFRACT
extern int XZoomWaiting;
#endif
#ifndef USE_VARARGS
extern int timer(int,int (*subrtn)(),...);
#else
extern int timer();
#endif


extern void clear_zoombox(void);
extern void flip_image(int kbdchar);
#ifndef WINFRACT
extern void reset_zoom_corners(void);
#endif
extern void setup287code(void);
extern void checkfreemem(int);

/*  frasetup -- C file prototypes */

extern int VLSetup(void);
extern int MandelfpSetup(void);
extern int NewtonSetup(void);
extern int StandaloneSetup(void);
extern int JuliafpSetup(void);
extern int TrigPlusTrigfpSetup(void);
extern int ZXTrigPlusZSetup(void);
extern int LambdaTrigSetup(void);
extern int JuliafnPlusZsqrdSetup(void);
extern int SqrTrigSetup(void);
extern int FnXFnSetup(void);
extern int MandelTrigSetup(void);
extern int MarksJuliafpSetup(void);
extern int SierpinskiFPSetup(void);
extern int StandardSetup(void);
extern int LambdaTrigOrTrigSetup(void);
extern int JuliaTrigOrTrigSetup(void);
extern int ManlamTrigOrTrigSetup(void);
extern int MandelTrigOrTrigSetup(void);
extern int HalleySetup(void);
extern int dynam2dfloatsetup(void);
extern int PhoenixSetup(void);
extern int MandPhoenixSetup(void);
extern int PhoenixCplxSetup(void);
extern int MandPhoenixCplxSetup(void);

/*  gifview -- C file prototypes */

extern int get_byte(void);
extern int get_bytes(BYTE *,int );
extern int gifview(void);

/*  hcmplx -- C file prototypes */

extern void HComplexTrig0(_HCMPLX *,_HCMPLX *);

/*  help -- C file prototypes */

extern int _find_token_length(char far *,unsigned int ,int *,int *);
extern int find_token_length(int ,char far *,unsigned int ,int *,int *);
extern int find_line_width(int ,char far *,unsigned int );
extern int process_document(PD_FUNC ,PD_FUNC ,VOIDPTR );
extern int help(int );
extern int read_help_topic(int ,int ,int ,VOIDFARPTR );
extern int makedoc_msg_func(int ,int );
extern void print_document(char *,int (*)(int ,int ),int );
extern int init_help(void );
extern void end_help(void );

/*  history -- C file prototypes */

void _fastcall restore_history_info(int);
void _fastcall save_history_info(void);

/*  intro -- C file prototypes */

extern void intro(void );

/*  jb -- C file prototypes */

int JulibrotSetup(void);
int JulibrotfpSetup(void);
int jbfp_per_pixel(void);
int Std4dfpFractal(void);

/*  jiim -- C file prototypes */

extern void Jiim(int);
extern _CMPLX PopFloat        (void);
extern _CMPLX DeQueueFloat    (void);
extern int    Init_Queue      (unsigned long);
extern void   Free_Queue      (void);
extern void   ClearQueue      (void);
extern int    QueueEmpty      (void);
extern int    QueueFull       (void);
extern int    QueueFullAlmost (void);
extern int    PushLong        (long ,  long);
extern int    PushFloat       (float,  float);
extern int    EnQueueLong     (long ,  long);
extern int    EnQueueFloat    (float,  float);

/*  line3d -- C file prototypes */

extern int line3d(BYTE *,unsigned int );
extern int _fastcall targa_color(int ,int ,int );
extern int targa_validate(char *);
extern int startdisk1(char *, FILE *, int);

/*  loadfdos -- C file prototypes */
#ifndef WINFRACT
extern int get_video_mode(struct fractal_info *,struct ext_blk_3 *);
#endif
/*  loadfile -- C file prototypes */

extern int read_overlay(void);
extern void set_if_old_bif(void);
extern void set_function_parm_defaults(void);
extern int fgetwindow(void);
extern void backwards_v18(void);
extern void backwards_v19(void);
extern void backwards_v20(void);
extern int check_back(void);

/*  loadmap -- C file prototypes */

extern void SetTgaColors(void);
extern int ValidateLuts(char *);
extern int SetColorPaletteName(char *);

/*  lorenz -- C file prototypes */

extern int orbit3dfloatsetup(void);
extern int lorenz3d1floatorbit(double *,double *,double *);
extern int lorenz3dfloatorbit(double *,double *,double *);
extern int lorenz3d3floatorbit(double *,double *,double *);
extern int lorenz3d4floatorbit(double *,double *,double *);
extern int henonfloatorbit(double *,double *,double *);
extern int inverse_julia_orbit(double *,double *,double *);
extern int Minverse_julia_orbit(void);
extern int inverse_julia_per_image(void);
extern int rosslerfloatorbit(double *,double *,double *);
extern int pickoverfloatorbit(double *,double *,double *);
extern int gingerbreadfloatorbit(double *,double *,double *);
extern int kamtorusfloatorbit(double *,double *,double *);
extern int hopalong2dfloatorbit(double *,double *,double *);
extern int chip2dfloatorbit(double *,double *,double *);
extern int quadruptwo2dfloatorbit(double *,double *,double *);
extern int threeply2dfloatorbit(double *,double *,double *);
extern int martin2dfloatorbit(double *,double *,double *);
extern int orbit2dfloat(void);
extern int funny_glasses_call(int (*)(void));
extern int ifs(void);
extern int orbit3dfloat(void);
extern int iconfloatorbit(double *, double *, double *);  /* dmf */
extern int latoofloatorbit(double *, double *, double *);  /* hb */
extern int  setup_convert_to_screen(struct affine *);
extern int plotorbits2dsetup(void);
extern int plotorbits2dfloat(void);

/*  lsys -- C file prototypes */

extern double  _fastcall getnumber(char far **);
extern int _fastcall ispow2(int);
extern int Lsystem(void);
extern int LLoad(void);

/*  miscfrac -- C file prototypes */

extern void froth_cleanup(void);

/*  miscovl -- C file prototypes */

extern void make_batch_file(void);
extern void shell_to_dos(void);
extern long fr_farfree(void);
extern void showfreemem(void);
extern int edit_text_colors(void);
extern int select_video_mode(int );
extern void format_vid_table(int choice,char *buf);
extern void make_mig(unsigned int, unsigned int);
extern int getprecdbl(int);
extern int getprecbf(int);
extern int getprecbf_mag(void);
extern void parse_comments(char *value);
extern void init_comments(void);
extern void write_batch_parms(char *, int, int, int, int);
extern void expand_comments(char far *, char far *);

/*  miscres -- C file prototypes */

extern void restore_active_ovly(void);
extern void findpath(char far *,char *);
extern void notdiskmsg(void);
extern void cvtcentermag(double *,double *,double *, double *,double *,double *);
extern void cvtcorners(double,double,double,double,double,double);
extern void cvtcentermagbf(bf_t, bf_t, double *, double *, double *, double *);
extern void cvtcornersbf(bf_t, bf_t, double,double,double,double);
extern void updatesavename(char *);
extern int check_writefile(char *,char *);
extern int check_key(void);
extern void showtrig(char *);
extern int set_trig_array(int ,char *);
extern void set_trig_pointers(int );
extern int tab_display(void);
extern int endswithslash(char far *);
extern int ifsload(void);
extern int find_file_item(char *,char *,FILE **, int);
extern int file_gets(char *,int ,FILE *);
extern void roundfloatd(double *);
extern void fix_inversion(double *);
extern int ungetakey(int);
extern void get_calculation_time(char *, long);
#ifndef XFRACT
extern int _cdecl _matherr(struct exception *);
#endif

/*  mpmath_c -- C file prototypes */

extern _CMPLX ComplexPower(_CMPLX ,_CMPLX );
extern void SetupLogTable(void );
extern long logtablecalc(long);
extern long far ExpFloat14(long );
extern int ComplexNewtonSetup(void );
extern int ComplexNewton(void );
extern int ComplexBasin(void );
extern int GausianNumber(int ,int );
extern void Arcsinz(_CMPLX z, _CMPLX *rz);
extern void Arccosz(_CMPLX z, _CMPLX *rz);
extern void Arcsinhz(_CMPLX z, _CMPLX *rz);
extern void Arccoshz(_CMPLX z, _CMPLX *rz);
extern void Arctanhz(_CMPLX z, _CMPLX *rz);
extern void Arctanz(_CMPLX z, _CMPLX *rz);
#ifndef XFRACT
extern _CMPLX ComplexSqrtFloat(double ,  double);
#else
extern _CMPLX ComplexSqrtFloat(double ,  double);
#endif

/*  msccos -- C file prototypes */

extern double _cos(double );

/*  parser -- C file prototypes */

struct fls { /* function, load, store pointers  CAE fp */
   void (near *function)(void);
   union Arg near *operand;
};

extern unsigned int SkipWhiteSpace(char *);
extern unsigned long NewRandNum(void );
extern void dRandom(void );
extern void SetRandFnct(void );
extern void RandomSeed(void );
extern void dStkSRand(void );
extern void dStkAbs(void );
extern void dStkSqr(void );
extern void dStkAdd(void );
extern void dStkSub(void );
extern void dStkConj(void );
extern void dStkZero(void );
extern void dStkOne(void );
extern void dStkReal(void );
extern void dStkImag(void );
extern void dStkNeg(void );
extern void dStkMul(void );
extern void dStkDiv(void );
extern void StkSto(void );
extern void StkLod(void );
extern void dStkMod(void );
extern void StkClr(void );
extern void dStkFlip(void );
extern void dStkSin(void );
extern void dStkTan(void );
extern void dStkTanh(void );
extern void dStkCoTan(void );
extern void dStkCoTanh(void );
extern void dStkRecip(void );
extern void StkIdent(void );
extern void dStkSinh(void );
extern void dStkCos(void );
extern void dStkCosXX(void );
extern void dStkCosh(void );
extern void dStkLT(void );
extern void dStkGT(void );
extern void dStkLTE(void );
extern void dStkGTE(void );
extern void dStkEQ(void );
extern void dStkNE(void );
extern void dStkOR(void );
extern void dStkAND(void );
extern void dStkLog(void );
extern void FPUcplxexp(_CMPLX *,_CMPLX *);
extern void dStkExp(void );
extern void dStkPwr(void );
extern void dStkASin(void );
extern void dStkASinh(void );
extern void dStkACos(void );
extern void dStkACosh(void );
extern void dStkATan(void );
extern void dStkATanh(void );
extern void dStkCAbs(void );
extern void dStkSqrt(void );
extern void dStkFloor(void );
extern void dStkCeil(void );
extern void dStkTrunc(void );
extern void dStkRound(void );
extern void EndInit(void );
extern struct ConstArg far *isconst(char *,int );
extern void NotAFnct(void );
extern void FnctNotFound(void );
extern int whichfn(char *,int );
extern int CvtStk(void);
extern int fFormula(void );
#ifndef XFRACT
extern void (far *isfunct(char *,int ))(void );
#else
extern void (far *isfunct(char *,int ))();
#endif
extern void RecSortPrec(void );
extern int Formula(void );
extern int BadFormula(void );
extern int form_per_pixel(void );
extern int frm_get_param_stuff (char * );
extern int RunForm(char *, int);
extern int fpFormulaSetup(void );
extern int intFormulaSetup(void );
extern void init_misc(void);
extern void free_workarea(void);
extern int fill_if_group(int endif_index, JUMP_PTRS_ST *jump_data);

/*  plot3d -- C file prototypes */

extern void cdecl draw_line(int ,int ,int ,int ,int );
extern void _fastcall plot3dsuperimpose16(int ,int ,int );
extern void _fastcall plot3dsuperimpose256(int ,int ,int );
extern void _fastcall plotIFS3dsuperimpose256(int ,int ,int );
extern void _fastcall plot3dalternate(int ,int ,int );
extern void plot_setup(void);

/*  printer -- C file prototypes */

extern void Print_Screen(void);

/*  prompts1 -- C file prototypes */

extern int fullscreen_prompt(char far*,int ,char far **,struct fullscreenvalues *,int ,char far *);
extern long get_file_entry(int,char *,char *,char *,char *);
extern int get_fracttype(void);
extern int get_fract_params(int );
extern int get_fract3d_params(void);
extern int get_3d_params(void);
extern int prompt_valuestring(char *buf,struct fullscreenvalues *val);
extern void setbailoutformula(enum bailouts);
extern int find_extra_param(int);
extern void load_params(int fractype);
extern int check_orbit_name(char *);
extern int scan_entries(FILE *infile, void far *ch, char *itemname);

/*  prompts2 -- C file prototypes */

extern int get_toggles(void);
extern int get_toggles2(void);
extern int passes_options(void);
extern int get_view_params(void);
extern int get_starfield_params(void );
extern int get_commands(void);
extern void goodbye(void);
extern int isadirectory(char *s);
extern int getafilename(char *,char *,char *);
extern int splitpath(char far *template,char *drive,char *dir,char *fname,char *ext);
extern int makepath(char *template,char *drive,char *dir,char *fname,char *ext);
extern int fr_findfirst(char *path);
extern int fr_findnext(void );
extern void shell_sort(void far *,int n,unsigned,int (__cdecl *fct)(VOIDFARPTR,VOIDFARPTR));
extern void fix_dirname(char *dirname);
extern int merge_pathnames(char *, char *, int);
extern int get_browse_params(void);
extern int get_cmd_string(void);
extern int get_rds_params(void);
extern int starfield(void);
extern int get_a_number(double *, double *);
extern int lccompare(VOIDFARPTR, VOIDFARPTR);
extern int dir_open(char *, char *, int, int);
extern int dir_remove(char *,char *);
extern FILE *dir_fopen(char *, char *, char *);
extern void extract_filename(char *, char *);
extern char *has_ext(char *source);

/*  realdos -- C file prototypes */

#ifndef WINFRACT
extern int showvidlength(void);
extern int stopmsg(int ,char far *);
extern int texttempmsg(char far *);
extern int showtempmsg(char far *);
extern void cleartempmsg(void);
extern void blankrows(int ,int ,int );
extern void helptitle(void);
extern int putstringcenter(int ,int ,int ,int ,char far *);
extern void stackscreen(void);
extern void unstackscreen(void);
extern void discardscreen(void);
extern int fullscreen_choice(int options, char far *hdg, char far *hdg2, char far *instr, int numchoices, char far *far *choices, int far *attributes, int boxwidth, int boxdepth, int colwidth, int current
, void (*formatitem)(int, char *), char *speedstring, int (*speedprompt)(int, int, int, char *, int), int (*checkkey)(int, int));
#ifndef XFRACT /* Unix should have this in string.h */
extern int strncasecmp(char far *,char far *,int );
#endif
extern int check_exit(void);
extern int main_menu(int );
extern int input_field(int ,int ,char *,int ,int ,int ,int (*)(int));
extern int field_prompt(int ,char far *,char far *,char *,int ,int (*)(int));
extern int thinking(int ,char far *);
extern void clear_screen(int );
extern int savegraphics(void);
extern int restoregraphics(void);
extern void discardgraphics(void);
extern int load_fractint_cfg(int );
extern void bad_fractint_cfg_msg(void);
extern void load_videotable(int );
extern int check_vidmode_key(int ,int );
extern int check_vidmode_keyname(char *);
extern void vidmode_keyname(int ,char *);
extern void freetempmsg(void);
extern char *despace(char *);
extern int menu_checkkey(int ,int );
#endif

/*  rotate -- C file prototypes */

extern void rotate(int );
extern void save_palette(void);
extern int load_palette(void );

/*  slideshw -- C file prototypes */

extern int slideshw(void);
extern int startslideshow(void);
extern void stopslideshow(void);
extern void recordshw(int );

/*  stereo -- C file prototypes */

extern int do_AutoStereo(void);
extern int outline_stereo(BYTE *, int);

/*  targa -- C file prototypes */

extern void WriteTGA(int ,int ,int );
extern int ReadTGA(int ,int );
extern void EndTGA(void );
extern void StartTGA(void);
extern void ReopenTGA(void);

/*  testpt -- C file prototypes */

extern int teststart(void);
extern void testend(void);
extern int testpt(double ,double ,double ,double ,long ,int );

/*  tgaview -- C file prototypes */

extern int tgaview(void);
extern int outlin16(BYTE*,int );

/*  yourvid -- C file prototypes */

extern int startvideo(void);
extern int endvideo(void);
extern void writevideo(int ,int ,int );
extern int readvideo(int ,int );
extern int readvideopalette(void);
extern int writevideopalette(void);
#ifdef XFRACT
extern void readvideoline(int ,int, int, BYTE * );
extern void writevideoline(int ,int, int, BYTE * );
#endif

/*  zoom -- C file prototypes */

extern void drawbox(int );
extern void moveboxf(double ,double );
extern void resizebox(int );
extern void chgboxi(int ,int );
extern void zoomout(void);
extern void aspectratio_crop(float ,float );
extern int init_pan_or_recalc(int );
extern void _fastcall drawlines(struct coords, struct coords, int, int);
extern void _fastcall addbox(struct coords);
extern void clearbox(void);
extern void dispbox(void);

/*  fractalb.c -- C file prototypes */

extern _CMPLX cmplxbntofloat(_BNCMPLX *);
extern _CMPLX cmplxbftofloat(_BFCMPLX *);
extern void comparevalues(char *, double, bn_t);
extern void comparevaluesbf(char *, double, bf_t);
extern void show_var_bf(char *s, bf_t n);
extern void show_two_bf(char *,bf_t,char *, bf_t, int);
extern void bfcornerstofloat(void);
extern void showcornersdbl(char *);
extern int MandelbnSetup(void);
extern int mandelbn_per_pixel(void);
extern int juliabn_per_pixel(void);
extern int dividebrot5bn_per_pixel(void);
extern int JuliabnFractal(void);
extern int JuliaZpowerbnFractal(void);
extern int DivideBrot5bnFractal(void);
extern _BNCMPLX *cmplxlog_bn(_BNCMPLX *t, _BNCMPLX *s);
extern _BNCMPLX *cplxmul_bn( _BNCMPLX *t, _BNCMPLX *x, _BNCMPLX *y);
extern _BNCMPLX *cplxdiv_bn( _BNCMPLX *t, _BNCMPLX *x, _BNCMPLX *y);
extern _BNCMPLX *ComplexPower_bn(_BNCMPLX *t, _BNCMPLX *xx, _BNCMPLX *yy);
extern int MandelbfSetup(void);
extern int mandelbf_per_pixel(void);
extern int juliabf_per_pixel(void);
extern int dividebrot5bf_per_pixel(void);
extern int JuliabfFractal(void);
extern int JuliaZpowerbfFractal(void);
extern int DivideBrot5bfFractal(void);
extern _BFCMPLX *cmplxlog_bf(_BFCMPLX *t, _BFCMPLX *s);
extern _BFCMPLX *cplxmul_bf( _BFCMPLX *t, _BFCMPLX *x, _BFCMPLX *y);
extern _BFCMPLX *cplxdiv_bf( _BFCMPLX *t, _BFCMPLX *x, _BFCMPLX *y);
extern _BFCMPLX *ComplexPower_bf(_BFCMPLX *t, _BFCMPLX *xx, _BFCMPLX *yy);

/*  memory -- C file prototypes */

extern void DisplayMemory (void);
extern void DisplayHandle (U16 handle);
extern int MemoryType (U16 handle);
extern void InitMemory (void);
extern void ExitCheck (void);
extern U16 MemoryAlloc(U16 size, long count, int stored_at);
extern void MemoryRelease(U16 handle);
extern int MoveToMemory(BYTE far *buffer,U16 size,long count,long offset,U16 handle);
extern int MoveFromMemory(BYTE far *buffer,U16 size,long count,long offset,U16 handle);
extern int SetMemory(int value,U16 size,long count,long offset,U16 handle);

/*  soi -- C file prototypes */

extern void soi (void);
extern void soi_ldbl (void);

/*
 *  uclock -- C file prototypes
 *  The  uclock_t typedef placed here because uclock.h
 *  prototype is for DOS version only.
 */
typedef unsigned long uclock_t;

extern uclock_t usec_clock(void);
extern void restart_uclock(void);
extern void wait_until(int index, uclock_t wait_time);


#ifndef DEBUG
/*#define DEBUG */
#endif

#endif
