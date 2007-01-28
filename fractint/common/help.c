/*
 * help.c
 *
 *
 *
 * Revision history:
 *
 *   2-26-90  EAN     Initial version.
 *
 *
 */

#ifndef TEST /* kills all those assert macros in production version */
#define NDEBUG
#endif

#define INCLUDE_COMMON  /* include common code in helpcom.h */

#if !defined(XFRACT)
#include <io.h>
#endif
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
  /* see Fractint.c for a description of the "include"  hierarchy */
#include "port.h"
#include "prototyp.h"
#include "helpdefs.h"
#include "drivers.h"

#define MAX_HIST           16        /* number of pages we'll remember */
#define ACTION_CALL         0        /* values returned by help_topic() */
#define ACTION_PREV         1
#define ACTION_PREV2        2        /* special - go back two topics */
#define ACTION_INDEX        3
#define ACTION_QUIT         4
#define F_HIST              (1<<0)   /* flags for help_topic() */
#define F_INDEX             (1<<1)
#define MAX_PAGE_SIZE       (80*25)  /* no page of text may be larger */
#define TEXT_START_ROW      2        /* start print the help text here */

typedef struct
   {
   BYTE r, c;
   int           width;
   unsigned      offset;
   int           topic_num;
   unsigned      topic_off;
   } LINK;

typedef struct
   {
   int      topic_num;
   unsigned topic_off;
   } LABEL;

typedef struct
   {
   unsigned      offset;
   unsigned      len;
   int           margin;
   } PAGE;

typedef struct
   {
   int      topic_num;
   unsigned topic_off;
   int      link;
   } HIST;

struct help_sig_info
   {
   unsigned long sig;
   int           version;
   unsigned long base;     /* only if added to fractint.exe */
   } ;

void print_document(char *outfname, int (*msg_func)(int,int), int save_extraseg );
static int print_doc_msg_func(int pnum, int num_pages);

/* stuff from fractint */

static int            help_file = -1; /* help file handle */
static long           base_off;       /* offset to help info in help file */
static int            max_links;      /* max # of links in any page */
static int            max_pages;      /* max # of pages in any topic */
static int            num_label;      /* number of labels */
static int            num_topic;      /* number of topics */
static int            curr_hist = 0;  /* current pos in history */

/* these items alloc'ed in init_help... */

static long      *topic_offset;        /* 4*num_topic */
static LABEL     *label;               /* 4*num_label */
static HIST      *hist;                /* 6*MAX_HIST (96 bytes) */

/* these items alloc'ed only while help is active... */

static char       *buffer;           /* MAX_PAGE_SIZE (2048 bytes) */
static LINK       *link_table;       /* 10*max_links */
static PAGE       *page_table;       /* 4*max_pages  */

static void help_seek(long pos)
   {
   lseek(help_file, base_off+pos, SEEK_SET);
   }

static void displaycc(int row, int col, int color, int ch)
   {
	   static char s[] = "?";

   if (g_text_type == 1)   /* if 640x200x2 mode */
      {
      /*
       * This is REALLY ugly, but it works.  Non-current links (ones that
       * would be bold if 640x200 supported it) are in upper-case and the
       * current item is inversed.
       *
       */

      if (color & INVERSE)
         color = (signed int)INVERSE;
      else if (color & BRIGHT)
         {
         color = 0;   /* normal */
         if (ch>='a' && ch<='z')
            ch += 'A' - 'a';
         }
      else
         color = 0;   /* normal */
      }

   s[0] = (char)ch;
   driver_put_string(row, col, color, s);
   }

static void display_text(int row, int col, int color, char *text, unsigned len)
   {
   while (len-- != 0)
      {
      if (*text == CMD_LITERAL)
         {
         ++text;
         --len;
         }
      displaycc(row, col++, color, *text++);
      }
   }

static void display_parse_text(char *text, unsigned len, int start_margin, int *num_link, LINK *link)
   {
   char  *curr;
   int        row, col;
   int        tok;
   int        size,
              width;

   g_text_cbase = SCREEN_INDENT;
   g_text_rbase = TEXT_START_ROW;

   curr = text;
   row = 0;
   col = 0;

   size = width = 0;

   if (start_margin >= 0)
      tok = TOK_PARA;
   else
      tok = -1;

   while (1)
      {
      switch ( tok )
         {
         case TOK_PARA:
            {
            int indent,
                margin;

            if (size > 0)
               {
               ++curr;
               indent = *curr++;
               margin = *curr++;
               len  -= 3;
               }
            else
               {
               indent = start_margin;
               margin = start_margin;
               }

            col = indent;

            while (1)
               {
               tok = find_token_length(ONLINE, curr, len, &size, &width);

               if (tok == TOK_DONE || tok == TOK_NL || tok == TOK_FF )
                  break;

               if (tok == TOK_PARA)
                  {
                  col = 0;   /* fake a new-line */
                  row++;
                  break;
                  }

               if (tok == TOK_XONLINE || tok == TOK_XDOC)
                  {
                  curr += size;
                  len  -= size;
                  continue;
                  }

               /* now tok is TOK_SPACE or TOK_LINK or TOK_WORD */

               if (col+width > SCREEN_WIDTH)
                  {          /* go to next line... */
                  col = margin;
                  ++row;

                  if ( tok == TOK_SPACE )
                     width = 0;   /* skip spaces at start of a line */
                  }

               if (tok == TOK_LINK)
                  {
                  display_text(row, col, C_HELP_LINK, curr+1+3*sizeof(int), width);
                  if (num_link != NULL)
                     {
                     link[*num_link].r         = (BYTE)row;
                     link[*num_link].c         = (BYTE)col;
                     link[*num_link].topic_num = getint(curr+1);
                     link[*num_link].topic_off = getint(curr+1+sizeof(int));
                     link[*num_link].offset    = (unsigned) ((curr+1+3*sizeof(int)) - text);
                     link[*num_link].width     = width;
                     ++(*num_link);
                     }
                  }
               else if (tok == TOK_WORD )
                  display_text(row, col, C_HELP_BODY, curr, width);

               col += width;
               curr += size;
               len -= size;
               }

            width = size = 0;
            break;
            }

         case TOK_CENTER:
            col = find_line_width(ONLINE, curr, len);
            col = (SCREEN_WIDTH-col)/2;
            if (col < 0)
               col = 0;
            break;

         case TOK_NL:
            col = 0;
            ++row;
            break;

         case TOK_LINK:
            display_text(row, col, C_HELP_LINK, curr+1+3*sizeof(int), width);
            if (num_link != NULL)
               {
               link[*num_link].r         = (BYTE)row;
               link[*num_link].c         = (BYTE)col;
               link[*num_link].topic_num = getint(curr+1);
               link[*num_link].topic_off = getint(curr+1+sizeof(int));
               link[*num_link].offset    = (unsigned) ((curr+1+3*sizeof(int)) - text);
               link[*num_link].width     = width;
               ++(*num_link);
               }
            break;

         case TOK_XONLINE:  /* skip */
         case TOK_FF:       /* ignore */
         case TOK_XDOC:     /* ignore */
         case TOK_DONE:
         case TOK_SPACE:
            break;

         case TOK_WORD:
            display_text(row, col, C_HELP_BODY, curr, width);
            break;
         } /* switch */

      curr += size;
      len  -= size;
      col  += width;

      if (len == 0)
         break;

      tok = find_token_length(ONLINE, curr, len, &size, &width);
      } /* while (1) */

   g_text_cbase = 0;
   g_text_rbase = 0;
   }

static void color_link(LINK *link, int color)
   {
   g_text_cbase = SCREEN_INDENT;
   g_text_rbase = TEXT_START_ROW;

   if (g_text_type == 1)   /* if 640x200x2 mode */
      display_text(link->r, link->c, color, buffer+link->offset, link->width);
   else
      driver_set_attr(link->r, link->c, color, link->width);

   g_text_cbase = 0;
   g_text_rbase = 0;
   }

/* #define PUT_KEY(name, descrip)
		putstring(-1,-1,C_HELP_INSTR_KEYS,name),
		putstring(-1,-1,C_HELP_INSTR," "descrip"  ")
*/
#if defined(_WIN32)
#define PUT_KEY(name_,desc_) put_key(name_,desc_)
#else
#if !defined(XFRACT)
#define PUT_KEY(name, descrip)								\
	driver_put_string(-1,-1,C_HELP_INSTR,name),				\
	driver_put_string(-1,-1,C_HELP_INSTR,":"descrip"  ")
#else
#define PUT_KEY(name, descrip)						\
	driver_put_string(-1,-1,C_HELP_INSTR,name);		\
	driver_put_string(-1,-1,C_HELP_INSTR,":");		\
	driver_put_string(-1,-1,C_HELP_INSTR,descrip);	\
	driver_put_string(-1,-1,C_HELP_INSTR,"  ")
#endif
#endif

static void put_key(char *name, char *descrip)
{
	driver_put_string(-1,-1,C_HELP_INSTR,name);
	driver_put_string(-1,-1,C_HELP_INSTR,":");
	driver_put_string(-1,-1,C_HELP_INSTR,descrip);
	driver_put_string(-1,-1,C_HELP_INSTR,"  ");
}

static void helpinstr(void)
   {
   int ctr;

   for (ctr=0; ctr<80; ctr++)
     driver_put_string(24, ctr, C_HELP_INSTR, " ");

   driver_move_cursor(24, 1);
   PUT_KEY("F1",               "Index");
#if !defined(XFRACT) && !defined(_WIN32)
   PUT_KEY("\030\031\033\032", "Select");
#else
   PUT_KEY("K J H L", "Select");
#endif
   PUT_KEY("Enter",            "Go to");
   PUT_KEY("Backspace",        "Last topic");
   PUT_KEY("Escape",           "Exit help");
   }

static void printinstr(void)
   {
   int ctr;

   for (ctr=0; ctr<80; ctr++)
     driver_put_string(24, ctr, C_HELP_INSTR, " ");

   driver_move_cursor(24, 1);
   PUT_KEY("Escape", "Abort");
   }

#undef PUT_KEY

static void display_page(char *title, char *text, unsigned text_len, int page, int num_pages, int start_margin, int *num_link, LINK *link)
   {
   char temp[9];

   helptitle();
   helpinstr();
   driver_set_attr(2, 0, C_HELP_BODY, 80*22);
   putstringcenter(1, 0, 80, C_HELP_HDG, title);
   sprintf(temp, "%2d of %d", page+1, num_pages);
#if !defined(XFRACT) && !defined(_WIN32)
   driver_put_string(1, 79-(6 + ((num_pages>=10)?2:1)), C_HELP_INSTR, temp);
#else
   /* Some systems (Ultrix) mess up if you write to column 80 */
   driver_put_string(1, 78-(6 + ((num_pages>=10)?2:1)), C_HELP_INSTR, temp);
#endif

   if (text != NULL)
      display_parse_text(text, text_len, start_margin, num_link, link);

   driver_hide_text_cursor();
   }

/*
 * int overlap(int a, int a2, int b, int b2);
 *
 * If a, a2, b, and b2 are points on a line, this function returns the
 * distance of intersection between a-->a2 and b-->b2.  If there is no
 * intersection between the lines this function will return a negative number
 * representing the distance between the two lines.
 *
 * There are six possible cases of intersection between the lines:
 *
 *                      a                     a2
 *                      |                     |
 *     b         b2     |                     |       b         b2
 *     |---(1)---|      |                     |       |---(2)---|
 *                      |                     |
 *              b       |     b2      b       |      b2
 *              |------(3)----|       |------(4)-----|
 *                      |                     |
 *               b      |                     |   b2
 *               |------+--------(5)----------+---|
 *                      |                     |
 *                      |     b       b2      |
 *                      |     |--(6)--|       |
 *                      |                     |
 *                      |                     |
 *
 */

static int overlap(int a, int a2, int b, int b2)
   {
   if ( b < a )
      {
      if ( b2 >= a2 )
         return ( a2 - a );            /* case (5) */

      return ( b2 - a );               /* case (1), case (3) */
      }

   if ( b2 <= a2 )
      return ( b2 - b );               /* case (6) */

   return ( a2 - b );                  /* case (2), case (4) */
   }

static int dist1(int a, int b)
   {
   int t = a - b;

   return (abs(t));
   }

#ifdef __TURBOC__
#   pragma warn -def /* turn off "Possible use before definition" warning */
#endif

static int find_link_updown(LINK *link, int num_link, int curr_link, int up)
   {
   int       ctr,
             curr_c2,
             best_overlap = 0,
             temp_overlap;
   LINK *curr,
        *temp,
        *best;
   int       temp_dist;

   curr    = &link[curr_link];
   best    = NULL;
   curr_c2 = curr->c + curr->width - 1;

   for (ctr=0, temp=link; ctr<num_link; ctr++, temp++)
      {
      if ( ctr != curr_link &&
           ( (up && temp->r < curr->r) || (!up && temp->r > curr->r) ) )
         {
         temp_overlap = overlap(curr->c, curr_c2, temp->c, temp->c+temp->width-1);
         /* if >= 3 lines between, prioritize on vertical distance: */
         if ((temp_dist = dist1(temp->r, curr->r)) >= 4)
            temp_overlap -= temp_dist * 100;

         if (best != NULL)
            {
            if ( best_overlap >= 0 && temp_overlap >= 0 )
               {     /* if they're both under curr set to closest in y dir */
               if ( dist1(best->r, curr->r) > temp_dist )
                  best = NULL;
               }
            else
               {
               if ( best_overlap < temp_overlap )
                  best = NULL;
               }
            }

         if (best == NULL)
            {
            best = temp;
            best_overlap = temp_overlap;
            }
         }
      }

   return ( (best==NULL) ? -1 : (int)(best-link) );
   }

static int find_link_leftright(LINK *link, int num_link, int curr_link, int left)
   {
   int       ctr,
             curr_c2,
             best_c2 = 0,
             temp_c2,
             best_dist = 0,
             temp_dist;
   LINK *curr,
        *temp,
        *best;

   curr    = &link[curr_link];
   best    = NULL;
   curr_c2 = curr->c + curr->width - 1;

   for (ctr=0, temp=link; ctr<num_link; ctr++, temp++)
      {
      temp_c2 = temp->c + temp->width - 1;

      if ( ctr != curr_link &&
           ( (left && temp_c2 < (int)curr->c) || (!left && (int)temp->c > curr_c2) ) )
         {
         temp_dist = dist1(curr->r, temp->r);

         if (best != NULL)
            {
            if ( best_dist == 0 && temp_dist == 0 )  /* if both on curr's line... */
               {
               if ( (  left && dist1(curr->c, best_c2) > dist1(curr->c, temp_c2) ) ||
                    ( !left && dist1(curr_c2, best->c) > dist1(curr_c2, temp->c) ) )
                  best = NULL;
               }
            else
               {
               if ( best_dist >= temp_dist )   /* if temp is closer... */
                  best = NULL;
               }
            } /* if (best...) */

         if (best == NULL)
            {
            best      = temp;
            best_dist = temp_dist;
            best_c2   = temp_c2;
            }
         }
      } /* for */

   return ( (best==NULL) ? -1 : (int)(best-link) );
   }

#ifdef __TURBOC__
#   pragma warn .def   /* back to default */
#   pragma warn -par   /* now turn off "Parameter not used" warning */
#endif

#ifdef __CLINT__
#   pragma argsused
#endif

static int find_link_key(LINK *link, int num_link, int curr_link, int key)
   {
   link = NULL;   /* just for warning */
   switch (key)
      {
      case FIK_TAB:      return ( (curr_link>=num_link-1) ? -1 : curr_link+1 );
      case FIK_SHF_TAB: return ( (curr_link<=0)          ? -1 : curr_link-1 );
      default:       assert(0);  return (-1);
      }
   }

#ifdef __TURBOC__
#   pragma warn .par /* back to default */
#endif

static int do_move_link(LINK *link, int num_link, int *curr, int (*f)(LINK *,int,int,int), int val)
   {
   int t;

   if (num_link > 1)
      {
      if ( f == NULL )
         t = val;
      else
         t = (*f)(link, num_link, *curr, val);

      if ( t >= 0 && t != *curr )
         {
         color_link(&link[*curr], C_HELP_LINK);
         *curr = t;
         color_link(&link[*curr], C_HELP_CURLINK);
         return (1);
         }
      }

   return (0);
   }

static int help_topic(HIST *curr, HIST *next, int flags)
   {
   int       len;
   int       key;
   int       num_pages;
   int       num_link;
   int       page;
   int       curr_link;
   char      title[81];
   long      where;
   int       draw_page;
   int       action;
   BYTE ch;

   where     = topic_offset[curr->topic_num]+sizeof(int); /* to skip flags */
   curr_link = curr->link;

   help_seek(where);

   read(help_file, (char *)&num_pages, sizeof(int));
   assert(num_pages>0 && num_pages<=max_pages);

   read(help_file, (char *)page_table, 3*sizeof(int)*num_pages);

   read(help_file, &ch, 1);
   len = ch;
   assert(len<81);
   read(help_file, (char *)title, len);
   title[len] = '\0';

   where += sizeof(int) + num_pages*3*sizeof(int) + 1 + len + sizeof(int);

   for (page=0; page<num_pages; page++)
      if (curr->topic_off >= page_table[page].offset &&
          curr->topic_off <  page_table[page].offset+page_table[page].len )
         break;

   assert(page < num_pages);

   action = -1;
   draw_page = 2;

   do
      {
      if (draw_page)
         {
         help_seek(where+page_table[page].offset);
         read(help_file, buffer, page_table[page].len);

         num_link = 0;
         display_page(title, buffer, page_table[page].len, page, num_pages,
                      page_table[page].margin, &num_link, link_table);

         if (draw_page==2)
            {
            assert(num_link<=0 || (curr_link>=0 && curr_link<num_link));
            }
         else if (draw_page==3)
            curr_link = num_link - 1;
         else
            curr_link = 0;

         if (num_link > 0)
            color_link(&link_table[curr_link], C_HELP_CURLINK);

         draw_page = 0;
         }

      key = driver_get_key();

      switch (key)
         {
         case FIK_PAGE_DOWN:
            if (page<num_pages-1)
               {
               page++;
               draw_page = 1;
               }
            break;

         case FIK_PAGE_UP:
            if (page>0)
               {
               page--;
               draw_page = 1;
               }
            break;

         case FIK_HOME:
            if ( page != 0 )
               {
               page = 0;
               draw_page = 1;
               }
            else
               do_move_link(link_table, num_link, &curr_link, NULL, 0);
            break;

         case FIK_END:
            if ( page != num_pages-1 )
               {
               page = num_pages-1;
               draw_page = 3;
               }
            else
               do_move_link(link_table, num_link, &curr_link, NULL, num_link-1);
            break;

         case FIK_TAB:
            if ( !do_move_link(link_table, num_link, &curr_link, find_link_key, key) &&
                 page<num_pages-1 )
               {
               ++page;
               draw_page = 1;
               }
            break;

         case FIK_SHF_TAB:
            if ( !do_move_link(link_table, num_link, &curr_link, find_link_key, key) &&
                 page>0 )
               {
               --page;
               draw_page = 3;
               }
            break;

         case FIK_DOWN_ARROW:
            if ( !do_move_link(link_table, num_link, &curr_link, find_link_updown, 0) &&
                 page<num_pages-1 )
               {
               ++page;
               draw_page = 1;
               }
            break;

         case FIK_UP_ARROW:
            if ( !do_move_link(link_table, num_link, &curr_link, find_link_updown, 1) &&
                 page>0 )
               {
               --page;
               draw_page = 3;
               }
            break;

         case FIK_LEFT_ARROW:
            do_move_link(link_table, num_link, &curr_link, find_link_leftright, 1);
            break;

         case FIK_RIGHT_ARROW:
            do_move_link(link_table, num_link, &curr_link, find_link_leftright, 0);
            break;

         case FIK_ESC:         /* exit help */
            action = ACTION_QUIT;
            break;

         case FIK_BACKSPACE:   /* prev topic */
         case FIK_ALT_F1:
            if (flags & F_HIST)
               action = ACTION_PREV;
            break;

         case FIK_F1:    /* help index */
            if (!(flags & F_INDEX))
               action = ACTION_INDEX;
            break;

         case FIK_ENTER:
         case FIK_ENTER_2:
            if (num_link > 0)
               {
               next->topic_num = link_table[curr_link].topic_num;
               next->topic_off = link_table[curr_link].topic_off;
               action = ACTION_CALL;
               }
            break;
         } /* switch */
      }
   while ( action == -1 );

   curr->topic_off = page_table[page].offset;
   curr->link      = curr_link;

   return (action);
   }

int help(int action)
{
	HIST      curr;
	int       oldlookatmouse;
	int       oldhelpmode;
	int       flags;
	HIST      next;

	if (helpmode == -1)   /* is help disabled? */
	{
		return 0;
	}

	if (help_file == -1)
    {
		driver_buzzer(BUZZER_ERROR);
		return 0;
    }

	buffer = (char *) malloc((long) MAX_PAGE_SIZE);
	link_table = (LINK *) malloc(sizeof(LINK)*max_links);
	page_table = (PAGE *) malloc(sizeof(PAGE)*max_pages);

	if ((buffer == NULL) || (NULL == link_table) || (NULL == page_table))
    {
		driver_buzzer(BUZZER_ERROR);
		return 0;
    }

	oldlookatmouse = lookatmouse;
	lookatmouse = 0;
	timer_start -= clock_ticks();
	driver_stack_screen();

	if (helpmode >= 0)
    {
		next.topic_num = label[helpmode].topic_num;
		next.topic_off = label[helpmode].topic_off;
    }
	else
    {
		next.topic_num = helpmode;
		next.topic_off = 0;
    }

	oldhelpmode = helpmode;

	if (curr_hist <= 0)
		action = ACTION_CALL;  /* make sure it isn't ACTION_PREV! */

	do
	{
		switch (action)
        {
        case ACTION_PREV2:
			if (curr_hist > 0)
				curr = hist[--curr_hist];

			/* fall-through */

        case ACTION_PREV:
			if (curr_hist > 0)
				curr = hist[--curr_hist];
			break;

        case ACTION_QUIT:
			break;

        case ACTION_INDEX:
			next.topic_num = label[FIHELP_INDEX].topic_num;
			next.topic_off = label[FIHELP_INDEX].topic_off;

			/* fall-through */

        case ACTION_CALL:
			curr = next;
			curr.link = 0;
			break;
        } /* switch */

		flags = 0;
		if (curr.topic_num == label[FIHELP_INDEX].topic_num)
			flags |= F_INDEX;
		if (curr_hist > 0)
			flags |= F_HIST;

		if ( curr.topic_num >= 0 )
			action = help_topic(&curr, &next, flags);
		else
        {
			if ( curr.topic_num == -100 )
			{
				print_document("FRACTINT.DOC", print_doc_msg_func, 1);
				action = ACTION_PREV2;
			}
			else if ( curr.topic_num == -101 )
				action = ACTION_PREV2;
			else
            {
				display_page("Unknown Help Topic", NULL, 0, 0, 1, 0, NULL, NULL);
				action = -1;
				while (action == -1)
				{
					switch (driver_get_key())
					{
					case FIK_ESC:      action = ACTION_QUIT;  break;
					case FIK_ALT_F1:   action = ACTION_PREV;  break;
					case FIK_F1:       action = ACTION_INDEX; break;
					} /* switch */
				} /* while */
			}
        } /* else */

		if ( action != ACTION_PREV && action != ACTION_PREV2 )
        {
			if (curr_hist >= MAX_HIST)
            {
				int ctr;

				for (ctr=0; ctr<MAX_HIST-1; ctr++)
					hist[ctr] = hist[ctr+1];

				curr_hist = MAX_HIST-1;
			}
			hist[curr_hist++] = curr;
        }
    }
	while (action != ACTION_QUIT);

	free(buffer);
	free(link_table);
	free(page_table);

	driver_unstack_screen();
	lookatmouse = oldlookatmouse;
	helpmode = oldhelpmode;
	timer_start += clock_ticks();

	return 0;
}

#if !defined(XFRACT) && !defined(_WIN32)
static int dos_version(void)
   {
   union REGS r;

   r.x.ax = 0x3000;
   intdos(&r, &r);

   return (r.h.al*100 + r.h.ah);
   }

#endif

static int can_read_file(char *path)
   {
   int handle;

#ifdef __TURBOC__
   if ( (handle=open(path, O_RDONLY|O_DENYWRITE)) != -1)
#else
   if ( (handle=open(path, O_RDONLY)) != -1)
#endif
      {
      close(handle);
      return (1);
      }
   else
      return (0);
   }


static int exe_path(char *filename, char *path)
   {
#if !defined(XFRACT) && !defined(_WIN32)
   char *ptr;

   if (dos_version() >= 300)  /* DOS version 3.00+ ? */
      {
#ifdef __TURBOC__
      strcpy(path, _argv[0]);
#else  /* assume MSC */
      extern char **__argv;
      strcpy(path, __argv[0]);   /* note: __argv may be undocumented in MSC */
#endif
      if (strcmp(filename,"FRACTINT.EXE")==0)
         if (can_read_file(path))
            return (1);
      ptr = strrchr(path, SLASHC);
      if (ptr == NULL)
         ptr = path;
      else
         ++ptr;
      strcpy(ptr, filename);
      return (1);
      }

   return (0);
#else
   strcpy(path,SRCDIR);
   strcat(path,"/");
   strcat(path,filename);
   return 1;
#endif
   }

static int find_file(char *filename, char *path)
{
	if (exe_path(filename, path))
	{
		if (can_read_file(path))
		{
			return 1;
		}
	}
	findpath(filename, path);
	return path[0] ? 1 : 0;
}

static int _read_help_topic(int topic, int off, int len, VOIDPTR buf)
   {
   static int  curr_topic = -1;
   static long curr_base;
   static int  curr_len;
   int         read_len;

   if ( topic != curr_topic )
      {
      int t;
      char ch;

      curr_topic = topic;

      curr_base = topic_offset[topic];

      curr_base += sizeof(int);                 /* skip flags */

      help_seek(curr_base);
      read(help_file, (char *)&t, sizeof(int)); /* read num_pages */
      curr_base += sizeof(int) + t*3*sizeof(int); /* skip page info */

      if (t>0)
         help_seek(curr_base);
      read(help_file, &ch, 1);                  /* read title_len */
      t = ch;
      curr_base += 1 + t;                       /* skip title */

      if (t>0)
         help_seek(curr_base);
      read(help_file, (char *)&curr_len, sizeof(int)); /* read topic len */
      curr_base += sizeof(int);
      }

   read_len = (off+len > curr_len) ? curr_len - off : len;

   if (read_len > 0)
      {
      help_seek(curr_base + off);
      read(help_file, (char *)buf, read_len);
      }

   return ( curr_len - (off+len) );
   }

int read_help_topic(int label_num, int off, int len, VOIDPTR buf)
   /*
    * reads text from a help topic.  Returns number of bytes from (off+len)
    * to end of topic.  On "EOF" returns a negative number representing
    * number of bytes not read.
    */
   {
   int ret;
   ret = _read_help_topic(label[label_num].topic_num,
                          label[label_num].topic_off + off, len, buf);
   return ( ret );
   }

#define PRINT_BUFFER_SIZE  (32767)       /* max. size of help topic in doc. */
#define TEMP_FILE_NAME     "HELP.$$$"    /* temp file for storing extraseg  */
                                         /*    while printing document      */
#define MAX_NUM_TOPIC_SEC  (10)          /* max. number of topics under any */
                                         /*    single section (CONTENT)     */

typedef struct PRINT_DOC_INFO
   {
   int       cnum;          /* current CONTENT num */
   int       tnum;          /* current topic num */

   long      content_pos;   /* current CONTENT item offset in file */
   int       num_page;      /* total number of pages in document */

   int       num_contents,  /* total number of CONTENT entries */
             num_topic;     /* number of topics in current CONTENT */

   int       topic_num[MAX_NUM_TOPIC_SEC]; /* topic_num[] for current CONTENT entry */

   char *buffer;        /* text buffer */

   char      id[81];        /* buffer to store id in */
   char      title[81];     /* buffer to store title in */

#if !defined(XFRACT) && !defined(_WIN32)
   int     (*msg_func)(int pnum, int num_page);
#else
   int     (*msg_func)();
   int pnum;
#endif

   FILE     *file;          /* file to sent output to */
   int       margin;        /* indent text by this much */
   int       start_of_line; /* are we at the beginning of a line? */
   int       spaces;        /* number of spaces in a row */
   } PRINT_DOC_INFO;

void print_document(char *outfname, int (*msg_func)(int,int), int save_extraseg );

static void printerc(PRINT_DOC_INFO *info, int c, int n)
   {
   while ( n-- > 0 )
      {
      if (c==' ')
         ++info->spaces;

      else if (c=='\n' || c=='\f')
         {
         info->start_of_line = 1;
         info->spaces = 0;   /* strip spaces before a new-line */
         fputc(c, info->file);
         }

      else
         {
         if (info->start_of_line)
            {
            info->spaces += info->margin;
            info->start_of_line = 0;
            }

         while (info->spaces > 0)
            {
            fputc(' ', info->file);
            --info->spaces;
            }

         fputc(c, info->file);
         }
      }
   }

static void printers(PRINT_DOC_INFO *info, char *s, int n)
   {
   if (n > 0)
      {
      while ( n-- > 0 )
         printerc(info, *s++, 1);
      }
   else
      {
      while ( *s != '\0' )
         printerc(info, *s++, 1);
      }
   }

static int print_doc_get_info(int cmd, PD_INFO *pd, PRINT_DOC_INFO *info)
   {
	   int tmp;
   int t;
   BYTE ch;

   switch (cmd)
      {
      case PD_GET_CONTENT:
         if ( ++info->cnum >= info->num_contents )
            return (0);

         help_seek( info->content_pos );

         read(help_file, (char *)&t, sizeof(int));      /* read flags */
         info->content_pos += sizeof(int);
         pd->new_page = (t & 1) ? 1 : 0;

         read(help_file, &ch, 1);       /* read id len */
         t = ch;
		 if (t >= 80)
		 {
			 tmp = tell(help_file);
		 }
         assert(t<80);
         read(help_file, (char *)info->id, t);  /* read the id */
         info->content_pos += 1 + t;
         info->id[t] = '\0';

         read(help_file, (char *)&ch, 1);       /* read title len */
         t = ch;
         assert(t<80);
         read(help_file, (char *)info->title, t); /* read the title */
         info->content_pos += 1 + t;
         info->title[t] = '\0';

         read(help_file, (char *)&ch, 1);       /* read num_topic */
         t = ch;
         assert(t<MAX_NUM_TOPIC_SEC);
         read(help_file, (char *)info->topic_num, t*sizeof(int));  /* read topic_num[] */
         info->num_topic = t;
         info->content_pos += 1 + t*sizeof(int);

         info->tnum = -1;

         pd->id = info->id;
         pd->title = info->title;
         return (1);

      case PD_GET_TOPIC:
         if ( ++info->tnum >= info->num_topic )
            return (0);

         t = _read_help_topic(info->topic_num[info->tnum], 0, PRINT_BUFFER_SIZE, info->buffer);

         assert(t <= 0);

         pd->curr = info->buffer;
         pd->len  = PRINT_BUFFER_SIZE + t;   /* same as ...SIZE - abs(t) */
         return (1);

      case PD_GET_LINK_PAGE:
         pd->i = getint(pd->s+sizeof(long));
         return ( (pd->i == -1) ? 0 : 1 );

      case PD_RELEASE_TOPIC:
         return (1);

      default:
         return (0);
      }
   }

static int print_doc_output(int cmd, PD_INFO *pd, PRINT_DOC_INFO *info)
   {
   switch (cmd)
      {
      case PD_HEADING:
         {
         char line[81];
         char buff[40];
         int  width = PAGE_WIDTH + PAGE_INDENT;
         int  keep_going;

         if ( info->msg_func != NULL )
            keep_going = (*info->msg_func)(pd->pnum, info->num_page);
         else
            keep_going = 1;

         info->margin = 0;

         memset(line, ' ', 81);
         sprintf(buff, "Fractint Version %d.%01d%c",g_release/100, (g_release%100)/10,
                                ( (g_release%10) ? '0'+(g_release%10) : ' ') );
         memmove(line + ((width-(int)(strlen(buff))) / 2)-4, buff, strlen(buff));

         sprintf(buff, "Page %d", pd->pnum);
         memmove(line + (width - (int)strlen(buff)), buff, strlen(buff));

         printerc(info, '\n', 1);
         printers(info, line, width);
         printerc(info, '\n', 2);

         info->margin = PAGE_INDENT;

         return ( keep_going );
         }

      case PD_FOOTING:
         info->margin = 0;
         printerc(info, '\f', 1);
         info->margin = PAGE_INDENT;
         return (1);

      case PD_PRINT:
         printers(info, pd->s, pd->i);
         return (1);

      case PD_PRINTN:
         printerc(info, *pd->s, pd->i);
         return (1);

      case PD_PRINT_SEC:
         info->margin = TITLE_INDENT;
         if (pd->id[0] != '\0')
            {
            printers(info, pd->id, 0);
            printerc(info, ' ', 1);
            }
         printers(info, pd->title, 0);
         printerc(info, '\n', 1);
         info->margin = PAGE_INDENT;
         return (1);

      case PD_START_SECTION:
      case PD_START_TOPIC:
      case PD_SET_SECTION_PAGE:
      case PD_SET_TOPIC_PAGE:
      case PD_PERIODIC:
         return (1);

      default:
         return (0);
      }
   }

static int print_doc_msg_func(int pnum, int num_pages)
   {
   char temp[10];
   int  key;

   if ( pnum == -1 )    /* successful completion */
      {
      driver_buzzer(BUZZER_COMPLETE);
      putstringcenter(7, 0, 80, C_HELP_LINK, "Done -- Press any key");
      driver_get_key();
      return (0);
      }

   if ( pnum == -2 )   /* aborted */
      {
      driver_buzzer(BUZZER_INTERRUPT);
      putstringcenter(7, 0, 80, C_HELP_LINK, "Aborted -- Press any key");
      driver_get_key();
      return (0);
      }

   if (pnum == 0)   /* initialization */
      {
      helptitle();
      printinstr();
      driver_set_attr(2, 0, C_HELP_BODY, 80*22);
      putstringcenter(1, 0, 80, C_HELP_HDG, "Generating FRACTINT.DOC");

      driver_put_string(7, 30, C_HELP_BODY, "Completed:");

      driver_hide_text_cursor();
      }

   sprintf(temp, "%d%%", (int)( (100.0 / num_pages) * pnum ) );
   driver_put_string(7, 41, C_HELP_LINK, temp);

   while ( driver_key_pressed() )
      {
      key = driver_get_key();
      if ( key == FIK_ESC )
         return (0);    /* user abort */
      }

   return (1);   /* AOK -- continue */
   }

int makedoc_msg_func(int pnum, int num_pages)
{
	char buffer[80] = "";
	int result = 0;

	if (pnum >= 0)
	{
		sprintf(buffer, "\rcompleted %d%%", (int) ((100.0 / num_pages) * pnum));
		result = 1;
	}
	else if (pnum == -2)
	{
		sprintf(buffer, "\n*** aborted\n");
	}
	stopmsg(0, buffer);
	return result;
}

void print_document(char *outfname, int (*msg_func)(int,int), int save_extraseg )
   {
   PRINT_DOC_INFO info;
   int            success   = 0;
   int            temp_file = -1;
   char      *msg = NULL;

   /* TODO: allocate real memory, not reuse shared segment */
   info.buffer = extraseg;

/*   help_seek((long)sizeof(int)+sizeof(long));         Strange -- should be 8 -- CWM */
   help_seek(16L);                               /* indeed it should - Bert */
   read(help_file, (char *)&info.num_contents, sizeof(int));
   read(help_file, (char *)&info.num_page, sizeof(int));

   info.cnum = info.tnum = -1;
   info.content_pos = 6*sizeof(int) + num_topic*sizeof(long) + num_label*2*sizeof(int);
   info.msg_func = msg_func;

   if ( msg_func != NULL )
      msg_func(0, info.num_page);   /* initialize */

   if ( save_extraseg )
      {
      if ( (temp_file=open(TEMP_FILE_NAME, O_RDWR|O_CREAT|O_TRUNC|O_BINARY, S_IREAD|S_IWRITE)) == -1 )
         {
         msg = "Unable to create temporary file.\n";
         goto ErrorAbort;
         }

      if ( write(temp_file, info.buffer, PRINT_BUFFER_SIZE) != PRINT_BUFFER_SIZE )
         {
         msg = "Error writing temporary file.\n";
         goto ErrorAbort;
         }
      }

   if ( (info.file = fopen(outfname, "wt")) == NULL )
      {
      msg = "Unable to create output file.\n";
      goto ErrorAbort;
      }

   info.margin = PAGE_INDENT;
   info.start_of_line = 1;
   info.spaces = 0;

   success = process_document((PD_FUNC)print_doc_get_info,
                              (PD_FUNC)print_doc_output,   &info);
   fclose(info.file);

   if ( save_extraseg )
      {
      if ( lseek(temp_file, 0L, SEEK_SET) != 0L )
         {
         msg = "Error reading temporary file.\nSystem may be corrupt!\nSave your image and re-start FRACTINT!\n";
         goto ErrorAbort;
         }

      if ( read(temp_file, info.buffer, PRINT_BUFFER_SIZE) != PRINT_BUFFER_SIZE )
         {
         msg = "Error reading temporary file.\nSystem may be corrupt!\nSave your image and re-start FRACTINT!\n";
         goto ErrorAbort;
         }
      }

ErrorAbort:
   if (temp_file != -1)
      {
      close(temp_file);
      remove(TEMP_FILE_NAME);
      temp_file = -1;
      }

   if ( msg != NULL )
      {
      helptitle();
      stopmsg(STOPMSG_NO_STACK, msg);
      }

   else if ( msg_func != NULL )
      msg_func((success) ? -1 : -2, info.num_page );
   }

int init_help(void)
{
	struct help_sig_info hs = { 0 };
	char path[FILE_MAX_PATH+1];

	help_file = -1;

#ifndef WINFRACT
#if !defined(XFRACT) && !defined(_WIN32)
	if (help_file == -1)         /* now look for help files in FRACTINT.EXE */
    {
/*
      static char err_not_in_exe[] = "Help not found in FRACTINT.EXE!\n";
*/

		if (find_file("FRACTINT.EXE", path))
        {
#ifdef __TURBOC__
			if ((help_file = open(path, O_RDONLY|O_BINARY|O_DENYWRITE)) != -1)
#else
			if ((help_file = open(path, O_RDONLY|O_BINARY)) != -1)
#endif
			{
				long help_offset;

				for (help_offset = -((long)sizeof(hs)); help_offset >= -128L; help_offset--)
				{
					lseek(help_file, help_offset, SEEK_END);
					read(help_file, (char *)&hs, sizeof(hs));
					if (hs.sig == HELP_SIG)
					{
						break;
					}
				}

				if (hs.sig != HELP_SIG)
				{
					close(help_file);
					help_file = -1;
				}
				else
				{
					if (hs.version != FIHELP_VERSION)
					{
						close(help_file);
						help_file = -1;
						stopmsg(STOPMSG_NO_STACK, "Wrong help version in FRACTINT.EXE!\n");
					}
					else
					{
						base_off = hs.base;
					}
				}
			}
			else
			{
				stopmsg(STOPMSG_NO_STACK, "Help system was unable to open FRACTINT.EXE!\n");
			}
		}
		else
		{
			stopmsg(STOPMSG_NO_STACK, "Help system couldn't find FRACTINT.EXE!\n");
		}
	}
#endif
#endif

	if (help_file == -1)            /* look for FRACTINT.HLP */
	{
		if (find_file("fractint.hlp", path))
		{
#ifdef __TURBOC__
			if ((help_file = open(path, O_RDONLY|O_BINARY|O_DENYWRITE)) != -1)
#else
			if ((help_file = open(path, O_RDONLY|O_BINARY)) != -1)
#endif
			{
				read(help_file, (char *)&hs, sizeof(long)+sizeof(int));

				if (hs.sig != HELP_SIG)
				{
					close(help_file);
					stopmsg(STOPMSG_NO_STACK, "Invalid help signature in FRACTINT.HLP!\n");
				}
				else if (hs.version != FIHELP_VERSION)
				{
					close(help_file);
					stopmsg(STOPMSG_NO_STACK, "Wrong help version in FRACTINT.HLP!\n");
				}
				else
				{
					base_off = sizeof(long)+sizeof(int);
				}
			}
		}
	}

	if (help_file == -1)         /* Can't find the help files anywhere! */
	{
		static char msg[] =
#if !defined(XFRACT) && !defined(_WIN32)
			{"Help Files aren't in FRACTINT.EXE, and couldn't find FRACTINT.HLP!\n"};
#else
			{"Couldn't find fractint.hlp; set FRACTDIR to proper directory with setenv.\n"};
#endif
		stopmsg(STOPMSG_NO_STACK, msg);
	}

	help_seek(0L);

	read(help_file, (char *)&max_pages, sizeof(int));
	read(help_file, (char *)&max_links, sizeof(int));
	read(help_file, (char *)&num_topic, sizeof(int));
	read(help_file, (char *)&num_label, sizeof(int));
	help_seek((long)6*sizeof(int));  /* skip num_contents and num_doc_pages */

	assert(max_pages > 0);
	assert(max_links >= 0);
	assert(num_topic > 0);
	assert(num_label > 0);

	/* allocate all three arrays */
	topic_offset = (long *) malloc(sizeof(long)*num_topic);
	label = (LABEL *) malloc(sizeof(LABEL)*num_label);
	hist = (HIST *) malloc(sizeof(HIST)*MAX_HIST);

	if ((topic_offset == NULL) || (NULL == label) || (NULL == hist))
	{
		close(help_file);
		help_file = -1;
		stopmsg(STOPMSG_NO_STACK, "Not enough memory for help system!\n");

		return (-2);
	}

	/* read in the tables... */
	read(help_file, topic_offset, num_topic*sizeof(long));
	read(help_file, label, num_label*sizeof(LABEL));

	/* finished! */

	return 0;  /* success */
}

void end_help(void)
   {
   if (help_file != -1)
      {
      close(help_file);
      free(topic_offset);
	  free(label);
	  free(hist);
      help_file = -1;
      }
   }