#if !defined(DRIVERS_H)
#define DRIVERS_H

#ifndef PORT_H
#include "port.h"
#endif
#ifndef PROTOTYP_H
#include "prototyp.h"
#endif

/*------------------------------------------------------------
 * Driver Methods:
 *
 * init
 * Initialize the driver and return non-zero on success.
 *
 * terminate
 * flush
 * schedule_alarm
 * start_video
 * end_video

 * window
 * Do whatever is necessary to open up a window after the screen size
 * has been set.  In a windowed environment, the window may provide
 * scroll bars to pan a cropped view across the screen.
 *
 * resize
 * redraw
 * read_palette, write_palette
 * read_pixel, write_pixel
 * read_span, write_span
 * set_line_mode
 * draw_line
 * get_key
 * shell
 * set_video_mode
 * put_string
 * set_for_text, set_for_graphics, set_clear
 * find_font
 * move_cursor
 * hide_text_cursor
 * set_attr
 * scroll_up
 * stack_screen, unstack_screen, discard_screen
 */
typedef struct tagDriver Driver;
struct tagDriver {
  const char *name;			/* name of driver */
  int (*init)(Driver *drv, int *argc, char **argv);  /* init the driver */
  void (*terminate)(Driver *drv);	/* shutdown the driver */
  void (*flush)(Driver *drv);		/* flush pending updates */
  void (*schedule_alarm)(Driver *drv, int secs);	/* refresh alarm */

  int (*start_video)(Driver *drv);
  int (*end_video)(Driver *drv);

  void (*window)(Driver *drv);			/* creates a window */
  int (*resize)(Driver *drv);			/* handles window resize.  */
  void (*redraw)(Driver *drv);			/* redraws the screen */

  int (*read_palette)(Driver *drv);		/* reads palette into dacbox */
  int (*write_palette)(Driver *drv);		/* writes dacbox into palette */

  int (*read_pixel)(Driver *drv, int x, int y);
  void (*write_pixel)(Driver *drv, int x, int y, int color);
					/* reads a line of pixels */
  void (*read_span)(Driver *drv, int y, int x, int lastx, BYTE *pixels);
					/* writes a line of pixels */
  void (*write_span)(Driver *drv, int y, int x, int lastx, BYTE *pixels);

  void (*set_line_mode)(Driver *drv, int mode);	/* set copy/xor line */
  void (*draw_line)(Driver *drv, int x1, int y1, int x2, int y2); /* draw line */
  
  int (*get_key)(Driver *drv, int block);		/* poll or block for a key */
  void (*shell)(Driver *drv);			/* invoke a command shell */
  void (*set_video_mode)(Driver *drv, int ax, int bx, int cx, int dx);
  void (*put_string)(Driver *drv, int row, int col, int attr, const char *msg);

  void (*set_for_text)(Driver *drv);		/* set for text mode & save gfx */
  void (*set_for_graphics)(Driver *drv);	/* restores graphics and data */
  void (*set_clear)(Driver *drv);		/* clears text screen */

  BYTE *(*find_font)(Driver *drv, int parm);		/* for palette editor */

  /* text screen functions */
  void (*move_cursor)(Driver *drv, int row, int col);
  void (*hide_text_cursor)(Driver *drv);
  void (*set_attr)(Driver *drv, int row, int col, int attr, int count);
  void (*scroll_up)(Driver *drv, int top, int bot);
  void (*stack_screen)(Driver *drv);
  void (*unstack_screen)(Driver *drv);
  void (*discard_screen)(Driver *drv);

  /* sound routines */
  int (*init_fm)(Driver *drv);
  void (*delay)(Driver *drv, long time);
  void (*buzzer)(Driver *drv, int kind);
  int (*sound_on)(Driver *drv, int frequency);
  void (*sound_off)(Driver *drv);
  
  int (*diskp)(Driver *drv);
};

#define STD_DRIVER_STRUCT(name_) \
  { \
    "##name_##", \
    name_##_init, \
    name_##_terminate, \
    name_##_flush, \
    name_##_schedule_alarm, \
    name_##_start_video, \
    name_##_end_video, \
    name_##_window, \
    name_##_resize, \
    name_##_redraw, \
    name_##_read_palette, \
    name_##_write_palette, \
    name_##_read_pixel, \
    name_##_write_pixel, \
    name_##_read_span, \
    name_##_write_span, \
    name_##_set_line_mode, \
    name_##_draw_line, \
    name_##_get_key, \
    name_##_shell, \
    name_##_set_video_mode, \
    name_##_put_string, \
    name_##_set_for_text, \
    name_##_set_for_graphics, \
    name_##_set_clear, \
    name_##_find_font, \
    name_##_move_cursor, \
    name_##_hide_text_cursor, \
    name_##_set_attr, \
    name_##_scroll_up, \
    name_##_stack_screen, \
    name_##_unstack_screen, \
    name_##_discard_screen, \
    name_##_init_fm, \
    name_##_delay, \
    name_##_buzzer, \
    name_##_sound_on, \
    name_##_sound_off, \
    name_##_diskp \
  }

#if 0
#define HAVE_X11_DRIVER 1
#endif
#if 0
#define HAVE_DISK_DRIVER 1
#endif
#if 1
#define HAVE_ALLEGRO_DRIVER 1
#endif

extern int init_drivers(int *argc, char **argv);
extern void add_video_mode(Driver *drv, VIDEOINFO *mode);
extern void close_drivers(void);

extern Driver *display;			/* current driver in use */

#define USE_DRIVER_FUNCTIONS 1

#if defined(USE_DRIVER_FUNCTIONS)

extern void driver_terminate(void);
extern void driver_flush(void);
extern void driver_schedule_alarm(int secs);
extern int driver_start_video(void);
extern int driver_end_video(void);
extern void driver_window(void);
extern int driver_resize(void);
extern void driver_redraw(void);
extern int driver_read_palette(void);
extern int driver_write_palette(void);
extern int driver_read_pixel(int x, int y);
extern void driver_write_pixel(int x, int y, int color);
extern void driver_read_span(int y, int x, int lastx, BYTE *pixels);
extern void driver_write_span(int y, int x, int lastx, BYTE *pixels);
extern void driver_set_line_mode(int mode);
extern void driver_draw_line(int x1, int y1, int x2, int y2);
extern int driver_get_key(int block);
extern void driver_shell(void);
extern void driver_set_video_mode(int ax, int bx, int cx, int dx);
extern void driver_put_string(int row, int col, int attr, const char *msg);
extern void driver_set_for_text(void);
extern void driver_set_for_graphics(void);
extern void driver_set_clear(void);
extern BYTE *driver_find_font(int parm);
extern void driver_move_cursor(int row, int col);
extern void driver_hide_text_cursor(void);
extern void driver_set_attr(int row, int col, int attr, int count);
extern void driver_scroll_up(int top, int bot);
extern void driver_stack_screen(void);
extern void driver_unstack_screen(void);
extern void driver_discard_screen(void);
extern int driver_init_fm(void);
extern void driver_delay(long time);
extern void driver_buzzer(int kind);
extern int driver_sound_on(int frequency);
extern void driver_sound_off(void);
extern int driver_diskp(void);

#else

#define driver_terminate()		(*display->terminate)(display)
#define driver_flush()			(*display->flush)(display)
#define void driver_schedule_alarm(_secs) \
	(*display->schedule_alarm)(display, _secs)
#define driver_start_video()		(*display->start_video)(display)
#define driver_end_video()		(*display->end_video)(display)
#define driver_window()			(*display->window)(display)
#define driver_resize()			(*display->resize)(display)
#define driver_redraw()			(*display->redraw)(display)
#define driver_read_palette()		(*display->read_palette)(display)
#define driver_write_palette()		(*display->write_palette)(display)
#define driver_read_pixel(_x, _y) \
	(*display->read_pixel)(display, _x, _y)
#define driver_write_pixel(_x, _y, _color) \
	(*display->write_pixel)(display, _x, _y, _color)
#define driver_read_span(_y, _x, _lastx, _pixels) \
	(*display->read_span(_y, _x, _lastx, _pixels)
#define driver_write_span(_y, _x, _lastx, _pixels) \
	(*display->write_span)(display, _y, _x, _lastx, _pixels)
#define driver_set_line_mode(_m)	(*display->set_line_mode)(display, _m)
#define driver_draw_line(x1_, y1_, x2_, y2_) \
	(*display->draw_line)(x1_, y1_, x1_, y2_)
#define driver_get_key(_block)		(*display->get_key)(display, _block)
#define driver_shell()			(*display->shell)(display)
#define driver_set_video_mode(_ax, _bx, _cx, _dx) \
	(*display->set_video_mode)(display, _ax, _bx, _cx, _dx)
#define driver_put_string(_row, _col, _attr, _msg) \
	(*display->put_string)(display, _row, _col, _attr, _msg)
#define driver_set_for_text()		(*display->set_for_text)(display)
#define driver_set_for_graphics()	(*display->set_for_graphics)(display)
#define driver_set_clear()		(*display->set_clear)(display)
#define driver_find_font(_parm)		(*display->find_font)(display, _parm)
#define driver_move_cursor(_row, _col) \
	(*display->move_cursor)(display, _row, _col)
#define driver_hide_text_cursor()	(*display->hide_text_cursor)(display)
#define driver_set_attr(_row, _col, _attr, _count) \
	(*display->set_attr)(display, _row, _col, _attr, _count)
#define driver_scroll_up(_top, _bot) \
	(*display->scroll_up)(display, _top, _bot)
#define driver_stack_screen()		(*display->stack_screen)(display)
#define driver_unstack_screen()		(*display->unstack_screen)(display)
#define driver_discard_screen()		(*display->discard_screen)(display)
#define driver_init_fm()		(*display->init_fm)(display)
#define driver_delay(_time)             (*display->delay)(display, _time)
#define driver_buzzer(_kind)		(*display->buzzer)(display, _kind)
#define driver_sound_on(_freq)		(*display->sound_on)(display, _freq)
#define driver_sound_off()		(*display->sound_off)(display)
#define driver_diskp()			(*display->diskp)(display)

#endif

#endif /* DRIVERS_H */
