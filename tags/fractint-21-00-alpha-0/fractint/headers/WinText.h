#if !defined(WINTEXT_H)
#define WINTEXT_H

extern HWND			wintext_hWndCopy;
extern void			wintext_addkeypress(unsigned int);
extern void			wintext_clear();
extern void			wintext_cursor(int, int, int);
extern void			wintext_destroy(void);
extern unsigned int	wintext_getkeypress(int);
extern BOOL			wintext_initialize(HINSTANCE, HWND, LPSTR);
extern int			wintext_look_for_activity(int);
extern void			wintext_paintscreen(int, int, int, int);
extern void			wintext_putstring(int, int, int, const char *, int *, int *);
extern void			wintext_scroll_up(int top, int bot);
extern void			wintext_set_attr(int row, int col, int attr, int count);
extern int			wintext_textoff(void);
extern int			wintext_texton(void);
extern BYTE *		wintext_screen_get(void);
extern void			wintext_screen_set(const BYTE *copy);
extern void			wintext_hide_cursor(void);
extern VOID CALLBACK wintext_timer_redraw(HWND window, UINT msg, UINT_PTR idEvent, DWORD dwTime);

#define WINTEXT_MAX_COL 80
#define WINTEXT_MAX_ROW 25

#endif