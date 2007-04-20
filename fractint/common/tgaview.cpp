
/* Routine to decode Targa 16 bit RGB file
	*/

/* 16 bit .tga files were generated for continuous potential "potfile"s
	from version 9.? thru version 14.  Replaced by double row gif type
	file (.pot) in version 15.  Delete this code after a few more revs.
*/

extern "C"
{
/* see Fractint.c for a description of the "include"  hierarchy */
#include "port.h"
#include "prototyp.h"
#include "targa_lc.h"
#include "drivers.h"
}

static FILE *fptarga = NULL;            /* FILE pointer           */

/* Main entry decoder */
extern "C" int tga_view()
{
	int i;
	int cs;
	unsigned int width;
	struct fractal_info info;

	fptarga = t16_open(g_read_name, (int *)&width, (int *)&g_height, &cs, (U8 *)&info);
	if (fptarga == NULL)
	{
		return -1;
	}

	g_row_count = 0;
	for (i = 0; i < (int)g_height; ++i)
	{
		t16_getline(fptarga, width, (U16 *)g_box_x);
		if ((*g_out_line)((BYTE *)g_box_x, width))
		{
			fclose(fptarga);
			fptarga = NULL;
			return -1;
		}
		if (driver_key_pressed())
		{
			fclose(fptarga);
			fptarga = NULL;
			return -1;
		}
	}
	fclose(fptarga);
	fptarga = NULL;
	return 0;
}

/* Outline function for 16 bit data with 8 bit g_fudge */
extern "C" int out_line_16(BYTE *buffer, int linelen)
{
	int i;
	U16 *buf;
	buf = (U16 *)buffer;
	for (i = 0; i < linelen; i++)
	{
		g_put_color(i, g_row_count, buf[i] >> 8);
	}
	g_row_count++;
	return 0;
}