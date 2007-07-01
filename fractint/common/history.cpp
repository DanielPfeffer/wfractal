/*  HISTORY.C
	History routines taken out of framain2.c to make them accessable
	to WinFract */

/* see Fractint.cpp for a description of the include hierarchy */
#include <string.h>

#include "port.h"
#include "prototyp.h"
#include "fractype.h"

#include "history.h"

#include "EscapeTime.h"
#include "FiniteAttractor.h"
#include "Formula.h"
#include "ThreeDimensionalState.h"

struct HISTORY_ITEM
{
	short fractal_type;
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	double c_real;
	double c_imag;
	double potential[3];
	short random_seed;
	bool use_fixed_random_seed;
	short biomorph;
	short inside;
	long logmap;
	double invert[3];
	short decomposition;
	short symmetry;
	short init_3d[16];
	short previewfactor;
	short xtrans;
	short ytrans;
	short red_crop_left;
	short red_crop_right;
	short blue_crop_left;
	short blue_crop_right;
	short red_bright;
	short blue_bright;
	short xadjust;
	short eyeseparation;
	short glassestype;
	short outside;
	double x_3rd;
	double y_3rd;
	long distance_test;
	short bailoutold;
	BYTE function_index[4];
	short finattract;
	double initial_orbit_z[2];
	short periodicity;
	bool potential_16bit;
	short release;
	short save_release;
	short flag3d;
	short transparent[2];
	short ambient;
	short haze;
	short randomize;
	short rotate_lo;
	short rotate_hi;
	short distance_test_width;
	double dparm3;
	double dparm4;
	short fill_color;
	double mxmaxfp;
	double mxminfp;
	double mymaxfp;
	double myminfp;
	short zdots;
	float originfp;
	float depthfp;
	float heightfp;
	float widthfp;
	float screen_distance_fp;
	float eyesfp;
	short orbittype;
	short juli3Dmode;
	short major_method;
	short minor_method;
	double dparm5;
	double dparm6;
	double dparm7;
	double dparm8;
	double dparm9;
	double dparm10;
	long bail_out;
	short bailoutest;
	long iterations;
	short bf_math;
	short bflength;
	short yadjust;
	bool old_demm_colors;
	char filename[FILE_MAX_PATH];
	char itemname[ITEMNAMELEN+1];
	unsigned char dac[256][3];
	char  max_fn;
	char stdcalcmode;
	char three_pass;
	char use_initial_orbit_z;
	short logcalc;
	short stop_pass;
	bool is_mandelbrot;
	double proximity;
	bool no_bof;
	double math_tolerance[2];
	short orbit_delay;
	long orbit_interval;
	double oxmin;
	double oxmax;
	double oymin;
	double oymax;
	double ox3rd;
	double oy3rd;
	bool keep_screen_coordinates;
	char draw_mode;
};

static HISTORY_ITEM *s_history = NULL;		/* history storage */
static int s_history_index = -1;			/* user pointer into history tbl  */
static int s_save_index = 0;				/* save ptr into history tbl      */
static bool s_history_flag;				/* are we backing off in history? */

void _fastcall history_save_info()
{
	HISTORY_ITEM current = { 0 };
	HISTORY_ITEM last;

	if (g_max_history <= 0 || g_bf_math || !s_history)
	{
		return;
	}
#if defined(_WIN32)
	_ASSERTE(s_save_index >= 0 && s_save_index < g_max_history);
#endif
	last = s_history[s_save_index];

	memset((void *) &current, 0, sizeof(HISTORY_ITEM));
	current.fractal_type = (short) g_fractal_type;
	current.x_min = g_escape_time_state.m_grid_fp.x_min();
	current.x_max = g_escape_time_state.m_grid_fp.x_max();
	current.y_min = g_escape_time_state.m_grid_fp.y_min();
	current.y_max = g_escape_time_state.m_grid_fp.y_max();
	current.c_real = g_parameters[0];
	current.c_imag = g_parameters[1];
	current.dparm3 = g_parameters[2];
	current.dparm4 = g_parameters[3];
	current.dparm5 = g_parameters[4];
	current.dparm6 = g_parameters[5];
	current.dparm7 = g_parameters[6];
	current.dparm8 = g_parameters[7];
	current.dparm9 = g_parameters[8];
	current.dparm10 = g_parameters[9];
	current.fill_color = (short) g_fill_color;
	current.potential[0] = g_potential_parameter[0];
	current.potential[1] = g_potential_parameter[1];
	current.potential[2] = g_potential_parameter[2];
	current.use_fixed_random_seed = g_use_fixed_random_seed;
	current.random_seed = (short) g_random_seed;
	current.inside = (short) g_inside;
	current.logmap = g_log_palette_mode;
	current.invert[0] = g_inversion[0];
	current.invert[1] = g_inversion[1];
	current.invert[2] = g_inversion[2];
	current.decomposition = (short) g_decomposition[0];
	current.biomorph = (short) g_biomorph;
	current.symmetry = (short) g_force_symmetry;
	g_3d_state.get_raytrace_parameters(&current.init_3d[0]);
	current.previewfactor = (short) g_3d_state.preview_factor();
	current.xtrans = (short) g_3d_state.x_trans();
	current.ytrans = (short) g_3d_state.y_trans();
	current.red_crop_left = (short) g_3d_state.red().crop_left();
	current.red_crop_right = (short) g_3d_state.red().crop_right();
	current.blue_crop_left = (short) g_3d_state.blue().crop_left();
	current.blue_crop_right = (short) g_3d_state.blue().crop_right();
	current.red_bright = (short) g_3d_state.red().bright();
	current.blue_bright = (short) g_3d_state.blue().bright();
	current.xadjust = (short) g_3d_state.x_adjust();
	current.yadjust = (short) g_3d_state.y_adjust();
	current.eyeseparation = (short) g_3d_state.eye_separation();
	current.glassestype = (short) g_3d_state.glasses_type();
	current.outside = (short) g_outside;
	current.x_3rd = g_escape_time_state.m_grid_fp.x_3rd();
	current.y_3rd = g_escape_time_state.m_grid_fp.y_3rd();
	current.stdcalcmode = g_user_standard_calculation_mode;
	current.three_pass = (char) g_three_pass ? 1 : 0;
	current.stop_pass = (short) g_stop_pass;
	current.distance_test = g_distance_test;
	current.function_index[0] = BYTE(g_function_index[0]);
	current.function_index[1] = BYTE(g_function_index[1]);
	current.function_index[2] = BYTE(g_function_index[2]);
	current.function_index[3] = BYTE(g_function_index[3]);
	current.finattract = (short) g_finite_attractor;
	current.initial_orbit_z[0] = g_initial_orbit_z.x;
	current.initial_orbit_z[1] = g_initial_orbit_z.y;
	current.use_initial_orbit_z = static_cast<char>(g_use_initial_orbit_z);
	current.periodicity = (short) g_periodicity_check;
	current.potential_16bit = g_disk_16bit;
	current.release = (short) g_release;
	current.save_release = (short) g_save_release;
	current.flag3d = (short) g_display_3d;
	current.ambient = (short) g_3d_state.ambient();
	current.randomize = (short) g_3d_state.randomize_colors();
	current.haze = (short) g_3d_state.haze();
	current.transparent[0] = (short) g_3d_state.transparent0();
	current.transparent[1] = (short) g_3d_state.transparent1();
	current.rotate_lo = (short) g_rotate_lo;
	current.rotate_hi = (short) g_rotate_hi;
	current.distance_test_width = (short) g_distance_test_width;
	current.mxmaxfp = g_m_x_max_fp;
	current.mxminfp = g_m_x_min_fp;
	current.mymaxfp = g_m_y_max_fp;
	current.myminfp = g_m_y_min_fp;
	current.zdots = (short) g_z_dots;
	current.originfp = g_origin_fp;
	current.depthfp = g_depth_fp;
	current.heightfp = g_height_fp;
	current.widthfp = g_width_fp;
	current.screen_distance_fp = g_screen_distance_fp;
	current.eyesfp = g_eyes_fp;
	current.orbittype = (short) g_new_orbit_type;
	current.juli3Dmode = (short) g_juli_3d_mode;
	current.max_fn = static_cast<char>(g_formula_state.max_fn());
	current.major_method = (short) g_major_method;
	current.minor_method = (short) g_minor_method;
	current.bail_out = g_bail_out;
	current.bailoutest = (short) g_bail_out_test;
	current.iterations = g_max_iteration;
	current.old_demm_colors = g_old_demm_colors;
	current.logcalc = (short) g_log_dynamic_calculate;
	current.is_mandelbrot = g_is_mand;
	current.proximity = g_proximity;
	current.no_bof = g_no_bof;
	current.orbit_delay = (short) g_orbit_delay;
	current.orbit_interval = g_orbit_interval;
	current.oxmin = g_orbit_x_min;
	current.oxmax = g_orbit_x_max;
	current.oymin = g_orbit_y_min;
	current.oymax = g_orbit_y_max;
	current.ox3rd = g_orbit_x_3rd;
	current.oy3rd = g_orbit_y_3rd;
	current.keep_screen_coordinates = g_keep_screen_coords;
	current.draw_mode = (char) g_orbit_draw_mode;
	memcpy(current.dac, g_dac_box, 256*3);
	switch (g_fractal_type)
	{
	case FRACTYPE_FORMULA:
	case FRACTYPE_FORMULA_FP:
		strncpy(current.filename, g_formula_filename, FILE_MAX_PATH);
		strncpy(current.itemname, g_formula_name, ITEMNAMELEN + 1);
		break;
	case FRACTYPE_IFS:
	case FRACTYPE_IFS_3D:
		strncpy(current.filename, g_ifs_filename, FILE_MAX_PATH);
		strncpy(current.itemname, g_ifs_name, ITEMNAMELEN + 1);
		break;
	case FRACTYPE_L_SYSTEM:
		strncpy(current.filename, g_l_system_filename, FILE_MAX_PATH);
		strncpy(current.itemname, g_l_system_name, ITEMNAMELEN + 1);
		break;
	default:
		*(current.filename) = 0;
		*(current.itemname) = 0;
		break;
	}
	if (s_history_index == -1)        /* initialize the history file */
	{
		int i;
		for (i = 0; i < g_max_history; i++)
		{
			s_history[i] = current;
		}
		s_history_flag = false;
		s_save_index = 0;
		s_history_index = 0;   /* initialize history ptr */
	}
	else if (s_history_flag)
	{
		s_history_flag = false;   /* coming from user history command, don't save */
	}
	else if (memcmp(&current, &last, sizeof(HISTORY_ITEM)))
	{
		if (++s_save_index >= g_max_history)  /* back to beginning of circular buffer */
		{
			s_save_index = 0;
		}
		if (++s_history_index >= g_max_history)  /* move user pointer in parallel */
		{
			s_history_index = 0;
		}
#if defined(_WIN32)
		_ASSERTE(s_save_index >= 0 && s_save_index < g_max_history);
#endif
		s_history[s_save_index] = current;
	}
}

void _fastcall history_restore_info()
{
	HISTORY_ITEM last;
	if (g_max_history <= 0 || g_bf_math || !s_history)
	{
		return;
	}
#if defined(_WIN32)
	_ASSERTE(s_history_index >= 0 && s_history_index < g_max_history);
#endif
	last = s_history[s_history_index];

	g_invert = 0;
	g_calculation_status = CALCSTAT_PARAMS_CHANGED;
	g_resuming = false;
	g_fractal_type = last.fractal_type;
	g_escape_time_state.m_grid_fp.x_min() = last.x_min;
	g_escape_time_state.m_grid_fp.x_max() = last.x_max;
	g_escape_time_state.m_grid_fp.y_min() = last.y_min;
	g_escape_time_state.m_grid_fp.y_max() = last.y_max;
	g_parameters[0] = last.c_real;
	g_parameters[1] = last.c_imag;
	g_parameters[2] = last.dparm3;
	g_parameters[3] = last.dparm4;
	g_parameters[4] = last.dparm5;
	g_parameters[5] = last.dparm6;
	g_parameters[6] = last.dparm7;
	g_parameters[7] = last.dparm8;
	g_parameters[8] = last.dparm9;
	g_parameters[9] = last.dparm10;
	g_fill_color = last.fill_color;
	g_potential_parameter[0] = last.potential[0];
	g_potential_parameter[1] = last.potential[1];
	g_potential_parameter[2] = last.potential[2];
	g_use_fixed_random_seed = last.use_fixed_random_seed;
	g_random_seed = last.random_seed;
	g_inside = last.inside;
	g_log_palette_mode = last.logmap;
	g_inversion[0] = last.invert[0];
	g_inversion[1] = last.invert[1];
	g_inversion[2] = last.invert[2];
	g_decomposition[0] = last.decomposition;
	g_user_biomorph = last.biomorph;
	g_biomorph = last.biomorph;
	g_force_symmetry = last.symmetry;
	g_3d_state.set_raytrace_parameters(&last.init_3d[0]);
	g_3d_state.set_preview_factor(last.previewfactor);
	g_3d_state.set_x_trans(last.xtrans);
	g_3d_state.set_y_trans(last.ytrans);
	g_3d_state.set_red().set_crop_left(last.red_crop_left);
	g_3d_state.set_red().set_crop_right(last.red_crop_right);
	g_3d_state.set_blue().set_crop_left(last.blue_crop_left);
	g_3d_state.set_blue().set_crop_right(last.blue_crop_right);
	g_3d_state.set_red().set_bright(last.red_bright);
	g_3d_state.set_blue().set_bright(last.blue_bright);
	g_3d_state.set_x_adjust(last.xadjust);
	g_3d_state.set_y_adjust(last.yadjust);
	g_3d_state.set_eye_separation(last.eyeseparation);
	g_3d_state.set_glasses_type(GlassesType(last.glassestype));
	g_outside = last.outside;
	g_escape_time_state.m_grid_fp.x_3rd() = last.x_3rd;
	g_escape_time_state.m_grid_fp.y_3rd() = last.y_3rd;
	g_user_standard_calculation_mode = last.stdcalcmode;
	g_standard_calculation_mode = last.stdcalcmode;
	g_three_pass = (last.three_pass != 0);
	g_stop_pass = last.stop_pass;
	g_distance_test = last.distance_test;
	g_user_distance_test = last.distance_test;
	g_function_index[0] = last.function_index[0];
	g_function_index[1] = last.function_index[1];
	g_function_index[2] = last.function_index[2];
	g_function_index[3] = last.function_index[3];
	g_finite_attractor = last.finattract;
	g_initial_orbit_z.x = last.initial_orbit_z[0];
	g_initial_orbit_z.y = last.initial_orbit_z[1];
	g_use_initial_orbit_z = static_cast<InitialZType>(last.use_initial_orbit_z);
	g_periodicity_check = last.periodicity;
	g_user_periodicity_check = last.periodicity;
	g_disk_16bit = last.potential_16bit;
	g_release = last.release;
	g_save_release = last.save_release;
	g_display_3d = last.flag3d;
	g_3d_state.set_ambient(last.ambient);
	g_3d_state.set_randomize_colors(last.randomize);
	g_3d_state.set_haze(last.haze);
	g_3d_state.set_transparent0(last.transparent[0]);
	g_3d_state.set_transparent1(last.transparent[1]);
	g_rotate_lo = last.rotate_lo;
	g_rotate_hi = last.rotate_hi;
	g_distance_test_width = last.distance_test_width;
	g_m_x_max_fp = last.mxmaxfp;
	g_m_x_min_fp = last.mxminfp;
	g_m_y_max_fp = last.mymaxfp;
	g_m_y_min_fp = last.myminfp;
	g_z_dots = last.zdots;
	g_origin_fp = last.originfp;
	g_depth_fp = last.depthfp;
	g_height_fp = last.heightfp;
	g_width_fp = last.widthfp;
	g_screen_distance_fp = last.screen_distance_fp;
	g_eyes_fp = last.eyesfp;
	g_new_orbit_type = last.orbittype;
	g_juli_3d_mode = last.juli3Dmode;
	g_formula_state.set_max_fn(static_cast<int>(last.max_fn));
	g_major_method = static_cast<MajorMethodType>(last.major_method);
	g_minor_method = static_cast<MinorMethodType>(last.minor_method);
	g_bail_out = last.bail_out;
	g_bail_out_test = (enum bailouts) last.bailoutest;
	g_max_iteration = last.iterations;
	g_old_demm_colors = last.old_demm_colors;
	g_current_fractal_specific = &g_fractal_specific[g_fractal_type];
	g_potential_flag = (g_potential_parameter[0] != 0.0);
	if (g_inversion[0] != 0.0)
	{
		g_invert = 3;
	}
	g_log_dynamic_calculate = last.logcalc;
	g_is_mand = last.is_mandelbrot;
	g_proximity = last.proximity;
	g_no_bof = last.no_bof;
	g_orbit_delay = last.orbit_delay;
	g_orbit_interval = last.orbit_interval;
	g_orbit_x_min = last.oxmin;
	g_orbit_x_max = last.oxmax;
	g_orbit_y_min = last.oymin;
	g_orbit_y_max = last.oymax;
	g_orbit_x_3rd = last.ox3rd;
	g_orbit_y_3rd = last.oy3rd;
	g_keep_screen_coords = last.keep_screen_coordinates;
	if (g_keep_screen_coords)
	{
		g_set_orbit_corners = true;
	}
	g_orbit_draw_mode = (int) last.draw_mode;
	g_user_float_flag = (g_current_fractal_specific->isinteger != 0);
	memcpy(g_dac_box, last.dac, 256*3);
	memcpy(g_old_dac_box, last.dac, 256*3);
	if (g_map_dac_box)
	{
		memcpy(g_map_dac_box, last.dac, 256*3);
	}
	spindac(0, 1);
	g_save_dac = fractal_type_julibrot(g_fractal_type) ? SAVEDAC_NO : SAVEDAC_YES;
	switch (g_fractal_type)
	{
	case FRACTYPE_FORMULA:
	case FRACTYPE_FORMULA_FP:
		strncpy(g_formula_filename, last.filename, FILE_MAX_PATH);
		strncpy(g_formula_name, last.itemname, ITEMNAMELEN + 1);
		break;
	case FRACTYPE_IFS:
	case FRACTYPE_IFS_3D:
		strncpy(g_ifs_filename, last.filename, FILE_MAX_PATH);
		strncpy(g_ifs_name, last.itemname, ITEMNAMELEN + 1);
		break;
	case FRACTYPE_L_SYSTEM:
		strncpy(g_l_system_filename, last.filename, FILE_MAX_PATH);
		strncpy(g_l_system_name, last.itemname, ITEMNAMELEN + 1);
		break;
	default:
		break;
	}
}

void history_allocate()
{
	while (g_max_history > 0) /* decrease history if necessary */
	{
		s_history = (HISTORY_ITEM *) malloc(sizeof(HISTORY_ITEM)*g_max_history);
		if (s_history)
		{
			break;
		}
		g_max_history--;
	}
}

void history_free()
{
	if (s_history != NULL)
	{
		free(s_history);
	}
}

void history_back()
{
	--s_history_index;
	if (s_history_index <= 0)
	{
		s_history_index = g_max_history - 1;
	}
	s_history_flag = true;
}

void history_forward()
{
	++s_history_index;
	if (s_history_index >= g_max_history)
	{
		s_history_index = 0;
	}
	s_history_flag = true;
}
