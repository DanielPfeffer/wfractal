#if defined(LINUX)
#include <sys/statfs.h>
#endif

#include "port.h"
#include "cmplx.h"
#include "fractint.h"
#include "mpmath.h"

/* Global variables (yuck!) */
int MPOverflow = 0;
struct MP Ans = { 0 };
BYTE block[4096] = { 0 };
int g_checked_vvs = 0;
int g_cpu, g_fpu;                        /* cpu, fpu flags */
static char extrasegment[0x18000] = { 0 };
void *extraseg = &extrasegment[0];
long g_initial_x_l = 0;
long g_initial_y_l = 0;
BYTE g_old_dac_box[256][3];
long g_save_base = 0;				/* base clock ticks */ 
long g_save_ticks = 0;				/* save after this many ticks */ 
unsigned int strlocn[10*1024] = { 0 };
char supervga_list[] =
{
	'a', 'h', 'e', 'a', 'd', 'a',		//supervga_list   db      "aheada"
	0, 0,								//aheada  dw      0
	'a', 't', 'i', ' ', ' ', ' ',		//        db      "ati   "
	0, 0,								//ativga  dw      0
	'c', 'h', 'i', ' ', ' ', ' ',		//        db      "chi   "
	0, 0,								//chipstech dw    0
	'e', 'v', 'e', ' ', ' ', ' ',		//        db      "eve   "
	0, 0,								//everex  dw      0
	'g', 'e', 'n', ' ', ' ', ' ',		//        db      "gen   "
	0, 0,								//genoa   dw      0
	'n', 'c', 'r', ' ', ' ', ' ',		//        db      "ncr   "
	0, 0,								//ncr     dw      0
	'o', 'a', 'k', ' ', ' ', ' ',		//        db      "oak   "
	0, 0,								//oaktech dw      0
	'p', 'a', 'r', ' ', ' ', ' ',		//        db      "par   "
	0, 0,								//paradise dw     0
	't', 'r', 'i', ' ', ' ', ' ',		//        db      "tri   "
	0, 0,								//trident dw      0
	't', 's', 'e', 'n', 'g', '3',		//        db      "tseng3"
	0, 0,								//tseng   dw      0
	't', 's', 'e', 'n', 'g', '4',		//        db      "tseng4"
	0, 0,								//tseng4  dw      0
	'v', 'i', 'd', ' ', ' ', ' ',		//        db      "vid   "
	0, 0,								//video7  dw      0
	'a', 'h', 'e', 'a', 'd', 'b',		//        db      "aheadb"
	0, 0,								//aheadb  dw      0
	'v', 'e', 's', 'a', ' ', ' ',		//        db      "vesa  "
	0, 0,								//vesa    dw      0
	'c', 'i', 'r', 'r', 'u', 's',		//        db      "cirrus"
	0, 0,								//cirrus  dw      0
	't', '8', '9', '0', '0', ' ',		//        db      "t8900 "
	0, 0,								//t8900   dw      0
	'c', 'o', 'm', 'p', 'a', 'q',		//        db      "compaq"
	0, 0,								//compaq  dw      0
	'x', 'g', 'a', ' ', ' ', ' ',		//        db      "xga   "
	0, 0,								//xga     dw      0
	' ', ' ', ' ', ' ', ' ', ' ',		//        db      "      "        ; end-of-the-list
	0, 0								//        dw      0
};
int g_svga_type = 0;
char g_text_stack[4096] = { 0 };

/* g_video_table
 *
 *  |--Adapter/Mode-Name------|-------Comments-----------|
 *  |------INT 10H------|Dot-|--Resolution---|
 *  |key|--AX---BX---CX---DX|Mode|--X-|--Y-|Color|
 */
VIDEOINFO g_video_table[MAXVIDEOMODES] = { 0 };
int g_vxdots = 0;

/* Global variables that should be phased out (old video mode stuff) */
int g_video_vram = 0;
int g_virtual_screens = 0;

/* converts relative path to absolute path */
int expand_dirname(char *dirname, char *drive)
{
	return -1;
}

unsigned long get_disk_space()
{
	struct statfs space;
	if (!statfs(".", &space))
	{
		return 0;
	}

	return space.f_bavail != -1 ? space.f_bavail*512 : 0;
}

void init_failure(const char *message)
{
	fprintf(stderr, message);
}

int main(int argc, char *argv[])
{
	return application_main(argc, argv);
}
