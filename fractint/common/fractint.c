/*
		FRACTINT - The Ultimate Fractal Generator
                        Main Routine
*/

#include <string.h>
#include <time.h>
#include <signal.h>

/* for getcwd() */
#if defined(LINUX)
#include <unistd.h>
#endif
#if defined(_WIN32)
#include <direct.h>
#endif

#ifndef XFRACT
#include <io.h>
#endif

#ifndef USE_VARARGS
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <ctype.h>

/* #include hierarchy for fractint is a follows:
		Each module should include port.h as the first fractint specific
				include. port.h includes <stdlib.h>, <stdio.h>, <math.h>,
				<float.h>; and, ifndef XFRACT, <dos.h>.
		Most modules should include prototyp.h, which incorporates by
				direct or indirect reference the following header files:
				mpmath.h
				cmplx.h
				fractint.h
				big.h
				biginit.h
				helpcom.h
				externs.h
		Other modules may need the following, which must be included
				separately:
				fractype.h
				helpdefs.h
				lsys.y
				targa.h
				targa_lc.h
				tplus.h
		If included separately from prototyp.h, big.h includes cmplx.h
		and biginit.h; and mpmath.h includes cmplx.h
	*/

#include "port.h"
#include "prototyp.h"
#include "fractype.h"
#include "helpdefs.h"
#include "drivers.h"

struct videoinfo g_video_entry;
int helpmode;

long timer_start, timer_interval;        /* timer(...) start & total */
int     g_adapter;                /* Video Adapter chosen from list in ...h */
char *fract_dir1="", *fract_dir2="";

/*
	the following variables are out here only so
	that the calculate_fractal() and assembler routines can get at them easily
*/
		int     dotmode;                /* video access method      */
		int     textsafe2;              /* textsafe override from g_video_table */
		int     g_ok_to_print;              /* 0 if printf() won't work */
		int     sxdots, sydots;          /* # of dots on the physical screen    */
		int     sxoffs, syoffs;          /* physical top left of logical screen */
		int     xdots, ydots;           /* # of dots on the logical screen     */
		double  dxsize, dysize;         /* xdots-1, ydots-1         */
		int     colors = 256;           /* maximum colors available */
		long    maxit;                  /* try this many iterations */
		int     boxcount;               /* 0 if no zoom-box yet     */
		int     zrotate;                /* zoombox rotation         */
		double  zbx, zby;                /* topleft of zoombox       */
		double  zwidth, zdepth, zskew;    /* zoombox size & shape     */

		int     fractype;               /* if == 0, use Mandelbrot  */
		char    stdcalcmode;            /* '1', '2', 'g', 'b'       */
		long    creal, cimag;           /* real, imag'ry parts of C */
		long    delx, dely;             /* screen pixel increments  */
		long    delx2, dely2;           /* screen pixel increments  */
		LDBL    delxx, delyy;           /* screen pixel increments  */
		LDBL    delxx2, delyy2;         /* screen pixel increments  */
		long    delmin;                 /* for calcfrac/calculate_mandelbrot    */
		double  ddelmin;                /* same as a double         */
		double  param[MAXPARAMS];       /* parameters               */
		double  potparam[3];            /* three potential parameters*/
		long    fudge;                  /* 2**fudgefactor           */
		long    l_at_rad;               /* finite attractor radius  */
		double  f_at_rad;               /* finite attractor radius  */
		int     bitshift;               /* fudgefactor              */

		int     g_bad_config = 0;          /* 'fractint.cfg' ok?       */
		int hasinverse = 0;
		/* note that integer grid is set when integerfractal && !invert;    */
		/* otherwise the floating point grid is set; never both at once     */
		long    *lx0, *ly0;     /* x, y grid                */
		long    *lx1, *ly1;     /* adjustment for rotate    */
		/* note that lx1 & ly1 values can overflow into sign bit; since     */
		/* they're used only to add to lx0/ly0, 2s comp straightens it out  */
		double *dx0, *dy0;      /* floating pt equivs */
		double *dx1, *dy1;
		int     integerfractal;         /* TRUE if fractal uses integer math */

		/* usr_xxx is what the user wants, vs what we may be forced to do */
		char    usr_stdcalcmode;
		int     usr_periodicitycheck;
		long    usr_distest;
		char    usr_floatflag;

		int     viewwindow;             /* 0 for full screen, 1 for window */
		float   viewreduction;          /* window auto-sizing */
		int     viewcrop;               /* nonzero to crop default coords */
		float   finalaspectratio;       /* for view shape and rotation */
		int     viewxdots, viewydots;    /* explicit view sizing */

		int maxhistory = 10;

/* variables defined by the command line/files processor */
int     comparegif = 0;                   /* compare two gif files flag */
int     timedsave = 0;                    /* when doing a timed save */
int     resave_flag = RESAVE_NO;                  /* tells encoder not to incr filename */
int     started_resaves = FALSE;              /* but incr on first resave */
int     save_system;                    /* from and for save files */
int     tabmode = 1;                    /* tab display enabled */

/* for historical reasons (before rotation):         */
/*    top    left  corner of screen is (xxmin, yymax) */
/*    bottom left  corner of screen is (xx3rd, yy3rd) */
/*    bottom right corner of screen is (xxmax, yymin) */
double  xxmin, xxmax, yymin, yymax, xx3rd, yy3rd; /* selected screen corners  */
long    xmin, xmax, ymin, ymax, x3rd, y3rd;  /* integer equivs           */
double  sxmin, sxmax, symin, symax, sx3rd, sy3rd; /* displayed screen corners */
double  plotmx1, plotmx2, plotmy1, plotmy2;     /* real->screen multipliers */

int calc_status = CALCSTAT_NO_FRACTAL;
					  /* -1 no fractal                   */
                      /*  0 parms changed, recalc reqd   */
                      /*  1 actively calculating         */
                      /*  2 interrupted, resumable       */
                      /*  3 interrupted, not resumable   */
                      /*  4 completed                    */
long calctime;

int max_colors;                         /* maximum palette size */
int        zoomoff;                     /* = 0 when zoom is disabled    */
int        savedac;                     /* save-the-Video DAC flag      */
int browsing;                 /* browse mode flag */
char file_name_stack[16][FILE_MAX_FNAME]; /* array of file names used while browsing */
int name_stack_ptr ;
double toosmall;
int  minbox;
int no_sub_images;
int autobrowse, doublecaution;
char brwscheckparms, brwschecktype;
char browsemask[FILE_MAX_FNAME];
int scale_map[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}; /*RB, array for mapping notes to a (user defined) scale */


#define RESTART           1
#define IMAGESTART        2
#define RESTORESTART      3
#define CONTINUE          4

void check_samename(void)
	{
		char drive[FILE_MAX_DRIVE];
		char dir[FILE_MAX_DIR];
		char fname[FILE_MAX_FNAME];
		char ext[FILE_MAX_EXT];
		char path[FILE_MAX_PATH];
		splitpath(g_save_name, drive, dir, fname, ext);
		if (strcmp(fname, "fract001"))
		{
			makepath(path, drive, dir, fname, "gif");
			if (access(path, 0) == 0)
			{
				exit(0);
			}
		}
	}

/* Do nothing if math error */
static void my_floating_point_err(int sig)
{
	if (sig != 0)
	{
		overflow = 1;
	}
}

char g_exe_path[FILE_MAX_PATH] = { 0 };

static void set_exe_path(char *path)
{
	splitpath(path, NULL, g_exe_path, NULL, NULL);
	if (g_exe_path[0] != SLASHC)
	{
		/* relative path */
		char cwd[FILE_MAX_PATH];
		char *result = getcwd(cwd, NUM_OF(cwd));
		if (result)
		{
			char *end = &cwd[strlen(cwd)];
			if (end[-1] != SLASHC)
			{
				strcat(end, SLASH);
			}
			strcat(end, g_exe_path);
			strcpy(g_exe_path, end);
		}
		else
		{
			strcpy(g_exe_path, DOTSLASH);
			strcat(g_exe_path, path);
		}
	}
}


int main(int argc, char **argv)
{
	int resumeflag;
	int kbdchar;						/* keyboard key-hit value       */
	int kbdmore;						/* continuation variable        */
	char stacked = 0;						/* flag to indicate screen stacked */

	set_exe_path(argv[0]);

	fract_dir1 = getenv("FRACTDIR");
	if (fract_dir1 == NULL)
	{
		fract_dir1 = ".";
	}
#ifdef SRCDIR
	fract_dir2 = SRCDIR;
#else
	fract_dir2 = ".";
#endif

	/* this traps non-math library floating point errors */
	signal(SIGFPE, my_floating_point_err);

	initasmvars();                       /* initialize ASM stuff */
	InitMemory();

	/* let drivers add their video modes */
	if (! init_drivers(&argc, argv))
	{
		init_failure("Sorry, I couldn't find any working video drivers for your system\n");
		exit(-1);
	}
	/* load fractint.cfg, match against driver supplied modes */
	load_fractint_config();
	init_help();


restart:   /* insert key re-starts here */
#if defined(_WIN32)
	_ASSERTE(_CrtCheckMemory());
#endif
	autobrowse     = FALSE;
	brwschecktype  = TRUE;
	brwscheckparms = TRUE;
	doublecaution  = TRUE;
	no_sub_images = FALSE;
	toosmall = 6;
	minbox   = 3;
	strcpy(browsemask, "*.gif");
	strcpy(browsename, "            ");
	name_stack_ptr = -1; /* init loaded files stack */

	evolving = FALSE;
	paramrangex = 4;
	opx = newopx = -2.0;
	paramrangey = 3;
	opy = newopy = -1.5;
	odpx = odpy = 0;
	gridsz = 9;
	fiddlefactor = 1;
	fiddle_reduction = 1.0;
	this_gen_rseed = (unsigned int)clock_ticks();
	srand(this_gen_rseed);
	g_start_show_orbit = 0;
	g_show_dot = -1; /* turn off g_show_dot if entered with <g> command */
	calc_status = CALCSTAT_NO_FRACTAL;                    /* no active fractal image */

	cmdfiles(argc, argv);         /* process the command-line */
	dopause(PAUSE_ERROR_NO_BATCH); /* pause for error msg if not batch */
	init_msg("", NULL, 0);  /* this causes driver_get_key if init_msg called on runup */

	history_allocate();

	if (DEBUGFLAG_ABORT_SAVENAME == g_debug_flag && g_initialize_batch == INITBATCH_NORMAL)   /* abort if savename already exists */
	{
		check_samename();
	}
	driver_window();
	memcpy(olddacbox, g_dac_box, 256*3);      /* save in case colors= present */

	if (DEBUGFLAG_CPU_8088 == g_debug_flag)
	{
		cpu =  86; /* for testing purposes */
	}
	if (DEBUGFLAG_X_FPU_287 == g_debug_flag && fpu >= 287)
	{
		fpu = 287; /* for testing purposes */
		cpu = 286;
	}
	if (DEBUGFLAG_FPU_87 == g_debug_flag && fpu >=  87)
	{
		fpu =  87; /* for testing purposes */
		cpu =  86;
	}
	if (DEBUGFLAG_NO_FPU == g_debug_flag)
	{
		fpu =   0; /* for testing purposes */
	}
	if (getenv("NO87"))
	{
		fpu = 0;
	}

	if (fpu >= 287 && g_debug_flag != DEBUGFLAG_FAST_287_MATH)   /* Fast 287 math */
	{
		setup287code();
	}
	adapter_detect();                    /* check what video is really present */

	driver_set_for_text();                      /* switch to text mode */
	savedac = SAVEDAC_NO;                         /* don't save the VGA DAC */

#ifndef XFRACT
	if (g_bad_config < 0)                   /* fractint.cfg bad, no msg yet */
	{
		bad_fractint_cfg_msg();
	}
#endif

	max_colors = 256;                    /* the Windows version is lower */
	g_max_input_counter = (cpu >= 386) ? 80 : 30;   /* check the keyboard this often */

	if (g_show_file && g_init_mode < 0)
	{
		intro();                          /* display the credits screen */
		if (driver_key_pressed() == FIK_ESC)
		{
			driver_get_key();
			goodbye();
		}
	}

	browsing = FALSE;

	if (!g_function_preloaded)
	{
		set_if_old_bif();
	}
	stacked = 0;

restorestart:
#if defined(_WIN32)
	_ASSERTE(_CrtCheckMemory());
#endif

	if (g_color_preloaded)
	{
		memcpy(g_dac_box, olddacbox, 256*3);   /* restore in case colors= present */
	}

	lookatmouse = LOOK_MOUSE_NONE;                     /* ignore mouse */

	while (g_show_file <= 0)              /* image is to be loaded */
	{
		char *hdg;
		tabmode = 0;
		if (!browsing)     /*RB*/
		{
			if (g_overlay_3d)
			{
				hdg = "Select File for 3D Overlay";
				helpmode = HELP3DOVLY;
			}
			else if (g_display_3d)
			{
				hdg = "Select File for 3D Transform";
				helpmode = HELP3D;
			}
			else
			{
				hdg = "Select File to Restore";
				helpmode = HELPSAVEREST;
			}
			if (g_show_file < 0 && getafilename(hdg, g_gif_mask, g_read_name) < 0)
			{
				g_show_file = 1;               /* cancelled */
				g_init_mode = -1;
				break;
			}

			name_stack_ptr = 0; /* 'r' reads first filename for browsing */
			strcpy(file_name_stack[name_stack_ptr], browsename);
		}

		evolving = viewwindow = 0;
		g_show_file = 1;
		helpmode = -1;
		tabmode = 1;
		if (stacked)
		{
			driver_discard_screen();
			driver_set_for_text();
			stacked = 0;
		}
		if (read_overlay() == 0)       /* read hdr, get video mode */
		{
			break;                      /* got it, exit */
		}
		g_show_file = browsing ? 1 : -1;
	}

	helpmode = HELPMENU;                 /* now use this help mode */
	tabmode = 1;
	lookatmouse = LOOK_MOUSE_NONE;                     /* ignore mouse */

	if (((g_overlay_3d && !g_initialize_batch) || stacked) && g_init_mode < 0)        /* overlay command failed */
	{
		driver_unstack_screen();                  /* restore the graphics screen */
		stacked = 0;
		g_overlay_3d = 0;                    /* forget overlays */
		g_display_3d = 0;                    /* forget 3D */
		if (calc_status == CALCSTAT_NON_RESUMABLE)
		{
			calc_status = CALCSTAT_PARAMS_CHANGED;
		}
		resumeflag = 1;
		goto resumeloop;                  /* ooh, this is ugly */
	}

	savedac = SAVEDAC_NO;                         /* don't save the VGA DAC */

imagestart:                             /* calc/display a new image */
#if defined(_WIN32)
	_ASSERTE(_CrtCheckMemory());
#endif

	if (stacked)
	{
		driver_discard_screen();
		stacked = 0;
	}
#ifdef XFRACT
	usr_floatflag = 1;
#endif
	g_got_status = GOT_STATUS_NONE;                     /* for tab_display */

	if (g_show_file)
	{
		if (calc_status > CALCSTAT_PARAMS_CHANGED)              /* goto imagestart implies re-calc */
		{
			calc_status = CALCSTAT_PARAMS_CHANGED;
		}
	}

	if (g_initialize_batch == INITBATCH_NONE)
	{
		lookatmouse = -FIK_PAGE_UP;           /* just mouse left button, == pgup */
	}

	g_cycle_limit = g_initial_cycle_limit;         /* default cycle limit   */
	g_adapter = g_init_mode;                  /* set the video adapter up */
	g_init_mode = -1;                       /* (once)                   */

	while (g_adapter < 0)                /* cycle through instructions */
	{
		if (g_initialize_batch)                          /* batch, nothing to do */
		{
			g_initialize_batch = INITBATCH_BAILOUT_INTERRUPTED; /* exit with error condition set */
			goodbye();
		}
		kbdchar = main_menu(0);
		if (kbdchar == FIK_INSERT) /* restart pgm on Insert Key  */
		{
			goto restart;
		}
		if (kbdchar == FIK_DELETE)                    /* select video mode list */
		{
			kbdchar = select_video_mode(-1);
		}
		g_adapter = check_vidmode_key(0, kbdchar);
		if (g_adapter >= 0)
		{
			break;                                 /* got a video mode now */
		}
#ifndef XFRACT
		if ('A' <= kbdchar && kbdchar <= 'Z')
		{
			kbdchar = tolower(kbdchar);
		}
#endif
		if (kbdchar == 'd')  /* shell to DOS */
		{
			driver_set_clear();
#if !defined(_WIN32)
			/* don't use stdio without a console on Windows */
#ifndef XFRACT
			printf("\n\nShelling to DOS - type 'exit' to return\n\n");
#else
			printf("\n\nShelling to Linux/Unix - type 'exit' to return\n\n");
#endif
#endif
			driver_shell();
			goto imagestart;
		}

#ifndef XFRACT
		if (kbdchar == '@' || kbdchar == '2')  /* execute commands */
		{
#else
			if (kbdchar == FIK_F2 || kbdchar == '@')  /* We mapped @ to F2 */
			{
#endif
				if ((get_commands() & COMMAND_3D_YES) == 0)
				{
					goto imagestart;
				}
				kbdchar = '3';                         /* 3d=y so fall thru '3' code */
			}
#ifndef XFRACT
			if (kbdchar == 'r' || kbdchar == '3' || kbdchar == '#')
			{
#else
				if (kbdchar == 'r' || kbdchar == '3' || kbdchar == FIK_F3)
				{
#endif
					g_display_3d = 0;
					if (kbdchar == '3' || kbdchar == '#' || kbdchar == FIK_F3)
					{
						g_display_3d = 1;
					}
					if (g_color_preloaded)
					{
						memcpy(olddacbox, g_dac_box, 256*3);     /* save in case colors= present */
					}
					driver_set_for_text(); /* switch to text mode */
					g_show_file = -1;
					goto restorestart;
				}
				if (kbdchar == 't')  /* set fractal type */
				{
					julibrot = 0;
					get_fracttype();
					goto imagestart;
				}
				if (kbdchar == 'x')  /* generic toggle switch */
				{
					get_toggles();
					goto imagestart;
				}
				if (kbdchar == 'y')  /* generic toggle switch */
				{
					get_toggles2();
					goto imagestart;
				}
				if (kbdchar == 'z')  /* type specific parms */
				{
					get_fract_params(1);
					goto imagestart;
				}
				if (kbdchar == 'v')  /* view parameters */
				{
					get_view_params();
					goto imagestart;
				}
				if (kbdchar == 2)  /* ctrl B = browse parms*/
				{
					get_browse_params();
					goto imagestart;
				}
				if (kbdchar == 6)  /* ctrl f = sound parms*/
				{
					get_sound_params();
					goto imagestart;
				}
				if (kbdchar == 'f')  /* floating pt toggle */
				{
					usr_floatflag = (usr_floatflag == 0) ? 1 : 0;
					goto imagestart;
				}
				if (kbdchar == 'i')  /* set 3d fractal parms */
				{
					get_fract3d_params(); /* get the parameters */
					goto imagestart;
				}
				if (kbdchar == 'g')
				{
					get_cmd_string(); /* get command string */
					goto imagestart;
				}
		/* buzzer(2); */                          /* unrecognized key */
			}

	zoomoff = TRUE;                 /* zooming is enabled */
	helpmode = HELPMAIN;         /* now use this help mode */
	resumeflag = 0;  /* allows taking goto inside big_while_loop() */

resumeloop:
#if defined(_WIN32)
	_ASSERTE(_CrtCheckMemory());
#endif

	param_history(0); /* save old history */
	/* this switch processes gotos that are now inside function */
	switch (big_while_loop(&kbdmore, &stacked, resumeflag))
	{
	case RESTART:
		goto restart;
	case IMAGESTART:
		goto imagestart;
	case RESTORESTART:
		goto restorestart;
	default:
		break;
	}

	return 0;
}

int check_key()
{
	int key = driver_key_pressed();
	if (key != 0)
	{
		if (g_show_orbit)
		{
			orbit_scrub();
		}
		if (key != 'o' && key != 'O')
		{
			return -1;
		}
		driver_get_key();
		if (!driver_diskp())
		{
			g_show_orbit = g_show_orbit ? FALSE : TRUE;
		}
	}
	return 0;
}

/* timer function:
	timer(TIMER_ENGINE, (*fractal)())		fractal engine
	timer(TIMER_DECODER, NULL, int width)	decoder
	timer(TIMER_ENCODER)					encoder
*/
#ifndef USE_VARARGS
int timer(int timertype, int(*subrtn)(), ...)
#else
int timer(va_alist)
va_dcl
#endif
{
	va_list arg_marker;  /* variable arg list */
	char *timestring;
	time_t ltime;
	FILE *fp = NULL;
	int out = 0;
	int i;
	int do_bench;

#ifndef USE_VARARGS
	va_start(arg_marker, subrtn);
#else
	int timertype;
	int (*subrtn)();
	va_start(arg_marker);
	timertype = va_arg(arg_marker, int);
	subrtn = (int (*)())va_arg(arg_marker, int *);
#endif

	do_bench = g_timer_flag; /* record time? */
	if (timertype == 2)   /* encoder, record time only if debug = 200 */
	{
		do_bench = (DEBUGFLAG_TIME_ENCODER == g_debug_flag);
	}
	if (do_bench)
	{
		fp = dir_fopen(g_work_dir, "bench", "a");
	}
	timer_start = clock_ticks();
	switch (timertype)
	{
	case TIMER_ENGINE:
		out = (*(int(*)(void))subrtn)();
		break;
	case TIMER_DECODER:
		i = va_arg(arg_marker, int);
		out = (int)decoder((short)i); /* not indirect, safer with overlays */
		break;
	case TIMER_ENCODER:
		out = encoder();            /* not indirect, safer with overlays */
		break;
	}
	/* next assumes CLK_TCK is 10^n, n >= 2 */
	timer_interval = (clock_ticks() - timer_start) / (CLK_TCK/100);

	if (do_bench)
	{
		time(&ltime);
		timestring = ctime(&ltime);
		timestring[24] = 0; /*clobber newline in time string */
		switch (timertype)
		{
		case TIMER_DECODER:
			fprintf(fp, "decode ");
			break;
		case TIMER_ENCODER:
			fprintf(fp, "encode ");
			break;
		}
		fprintf(fp, "%s type=%s resolution = %dx%d maxiter=%ld",
			timestring,
			curfractalspecific->name,
			xdots,
			ydots,
			maxit);
		fprintf(fp, " time= %ld.%02ld secs\n", timer_interval/100, timer_interval%100);
		if (fp != NULL)
		{
			fclose(fp);
		}
		}
	return out;
}
