#include <string.h>
#include <limits.h>
#include <malloc.h>

#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
#include <io.h>
#endif

#ifndef USE_VARARGS
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <ctype.h>

#include "port.h"
#include "prototyp.h"
#include "drivers.h"

/* Memory allocation routines. */

/* For far memory: */
#define FAR_RESERVE   8192L    /* amount of far mem we will leave avail. */
/* For disk memory: */
#define DISKWRITELEN 2048L /* max # bytes transferred to/from disk mem at once */

BYTE *charbuf = NULL;
//int numEXThandles;
//long ext_xfer_size;
//U16 start_avail_extra = 0;

#define MAXHANDLES 256   /* arbitrary #, suitably big */
char memfile[] = "handle.$$$";
int numTOTALhandles;

char memstr[3][9] = {{"nowhere"}, {"memory"}, {"disk"}};

struct nowhere {
   enum stored_at_values stored_at; /* first 2 entries must be the same */
   long size;                       /* for each of these data structures */
   };

struct linearmem {
   enum stored_at_values stored_at;
   long size;
   BYTE *memory;
   };

struct disk {
   enum stored_at_values stored_at;
   long size;
   FILE *file;
   };

union mem {
   struct nowhere Nowhere;
   struct linearmem Linearmem;
   struct disk Disk;
   };

union mem handletable[MAXHANDLES];

/* Routines in this module */
static int _fastcall  CheckDiskSpace(long howmuch);
static int check_for_mem(int stored_at, long howmuch);
static U16 next_handle(void);
static int CheckBounds (long start, long length, U16 handle);
static void WhichDiskError(int);
static void DisplayError(int stored_at, long howmuch);

/* Routines in this module, visible to outside routines */

void DisplayMemory (void);
void DisplayHandle (U16 handle);
int MemoryType (U16 handle);
void InitMemory (void);
void ExitCheck (void);
U16 MemoryAlloc(U16 size, long count, int stored_at);
void MemoryRelease(U16 handle);
int MoveToMemory(BYTE *buffer,U16 size,long count,long offset,U16 handle);
int MoveFromMemory(BYTE *buffer,U16 size,long count,long offset,U16 handle);
int SetMemory(int value,U16 size,long count,long offset,U16 handle);

/* Memory handling support routines */

static int _fastcall CheckDiskSpace(long howmuch)
{
	/* TODO */
	return TRUE;
}

static void WhichDiskError(int I_O)
{
	/* Set I_O == 1 after a file create, I_O == 2 after a file set value */
	/* Set I_O == 3 after a file write, I_O == 4 after a file read */
	char buf[MSGLEN];
	char *pats[4] =
	{
		"Create file error %d:  %s",
		"Set file error %d:  %s",
		"Write file error %d:  %s",
		"Read file error %d:  %s"
	};
	sprintf(buf, pats[(1 <= I_O && I_O <= 4) ? (I_O-1) : 0], errno, strerror(errno));
	if (debugflag == 10000)
		if(stopmsg(STOPMSG_CANCEL | STOPMSG_NO_BUZZER,(char *)buf) == -1)
			goodbye(); /* bailout if ESC */
}

int MemoryType(U16 handle)
{
   return handletable[handle].Nowhere.stored_at;
}

static void DisplayError(int stored_at, long howmuch)
{
/* This routine is used to display an error message when the requested */
/* memory type cannot be allocated due to insufficient memory, AND there */
/* is also insufficient disk space to use as memory. */

   char buf[MSGLEN*2];
   char nmsg[MSGLEN*2];
   static char fmsg[] = {"Allocating %ld Bytes of %s memory failed.\nAlternate disk space is also insufficient. Goodbye"};
   strcpy(nmsg,fmsg);
   sprintf(buf,nmsg,howmuch,memstr[stored_at]);
   stopmsg(0,(char *)buf);
}

static int check_for_mem(int stored_at, long howmuch)
{
/* This function returns an adjusted stored_at value. */
/* This is where the memory requested can be allocated. */

   long maxmem;
   BYTE *temp;
   int use_this_type;

   use_this_type = NOWHERE;
   maxmem = (long)USHRT_MAX; /* limit EXTRA and FARMEM to 64K */
                             /* limit EXPANDED and EXTENDED to 64K blocks */

   if (debugflag == 420)
      stored_at = DISK;
   if (debugflag == 422)
      stored_at = MEMORY;

   switch (stored_at)
   {
   case MEMORY: /* check_for_mem */
      if (maxmem > howmuch) {
         temp = (BYTE *)malloc(howmuch + FAR_RESERVE);
         if (temp != NULL) { /* minimum free space + requested amount */
            free(temp);
            use_this_type = MEMORY;
            break;
         }
      }

   case DISK: /* check_for_mem */
   default: /* just in case a nonsense number gets used */
      if (CheckDiskSpace(howmuch)) {
         use_this_type = DISK;
         break;
      }
 /* failed, fall through, no memory available */

   case NOWHERE: /* check_for_mem */
      use_this_type = NOWHERE;
      break;

   } /* end of switch */

   return(use_this_type);
}

static U16 next_handle()
{
   U16 counter = 1; /* don't use handle 0 */

   while(handletable[counter].Nowhere.stored_at != NOWHERE &&
         counter < MAXHANDLES)
      counter++;
   return (counter);
}

static int CheckBounds (long start, long length, U16 handle)
{
   if(handletable[handle].Nowhere.size - start - length < 0)
      {
         static char msg[] = {"Memory reference out of bounds."};
       stopmsg(STOPMSG_INFO_ONLY | STOPMSG_NO_BUZZER,msg);
       DisplayHandle(handle);
       return (1);
      }
   if(length > (long)USHRT_MAX)
      {
         static char msg[] = {"Tried to move > 65,535 bytes."};
       stopmsg(STOPMSG_INFO_ONLY | STOPMSG_NO_BUZZER,msg);
       DisplayHandle(handle);
       return (1);
      }
   if(handletable[handle].Nowhere.stored_at == DISK &&
         (stackavail() <= DISKWRITELEN) )
      {
         static char msg[] = {"Stack space insufficient for disk memory."};
       stopmsg(STOPMSG_INFO_ONLY | STOPMSG_NO_BUZZER,msg);
       DisplayHandle(handle);
       return (1);
      }
   if(length <= 0)
      {
         static char msg[] = {"Zero or negative length."};
       stopmsg(STOPMSG_INFO_ONLY | STOPMSG_NO_BUZZER,msg);
       DisplayHandle(handle);
       return (1);
      }
   if(start < 0)
      {
         static char msg[] = {"Negative offset."};
       stopmsg(STOPMSG_INFO_ONLY | STOPMSG_NO_BUZZER,msg);
       DisplayHandle(handle);
       return (1);
      }
   return (0);
}

void DisplayMemory (void)
{
   long tmpfar;
   U32 tmpdisk;
   char buf[MSGLEN];
   extern unsigned long GetDiskSpace(void);

   tmpdisk = GetDiskSpace(); /* fix this for XFRACT ????? */
   tmpfar = fr_farfree();
   sprintf(buf, "memory=%ld, disk=%lu", tmpfar, tmpdisk);
   stopmsg(STOPMSG_INFO_ONLY | STOPMSG_NO_BUZZER,(char *)buf);
}

void DisplayHandle (U16 handle)
{
   char buf[MSGLEN];
   char nmsg[MSGLEN];
   static char fmsg[] = {"Handle %u, type %s, size %li"};

   strcpy(nmsg,fmsg);
   sprintf(buf,nmsg,handle,memstr[handletable[handle].Nowhere.stored_at],
           handletable[handle].Nowhere.size);
   if(stopmsg(STOPMSG_CANCEL | STOPMSG_NO_BUZZER,(char *)buf) == -1)
     goodbye(); /* bailout if ESC, it's messy, but should work */
}

void InitMemory (void)
{
	int counter;
#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
	long longtmp;
#endif

	numTOTALhandles = 0;
	for (counter = 0; counter < MAXHANDLES; counter++)
	{
		handletable[counter].Nowhere.stored_at = NOWHERE;
		handletable[counter].Nowhere.size = 0;
	}
#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
	numEXThandles = 0;
	longtmp = fr_farfree();
	ext_xfer_size = XMMWRITELEN; /* 8192 */
	if (longtmp < (long)XMMWRITELEN * 8)
	{
		ext_xfer_size = XMMWRITELEN / 2; /* 4096 */
	}
	if (longtmp < (long)XMMWRITELEN)
	{
		ext_xfer_size = XMMWRITELEN / 8; /* 1024, won't work, try anyway */
	}
#endif
}

void ExitCheck (void)
{
   U16 i;
   if(numTOTALhandles != 0) {
        static char msg[] = {"Error - not all memory released, I'll get it."};
      stopmsg(0,msg);
      for (i = 1; i < MAXHANDLES; i++)
         if (handletable[i].Nowhere.stored_at != NOWHERE) {
            char buf[MSGLEN];
            char nmsg[MSGLEN];
            static char fmsg[] = {"Memory type %s still allocated.  Handle = %i."};
            strcpy(nmsg,fmsg);
            sprintf(buf,nmsg,memstr[handletable[i].Nowhere.stored_at],i);
            stopmsg(0,(char *)buf);
            MemoryRelease(i);
         }
   }
}

/* * * * * */
/* Memory handling routines */

U16 MemoryAlloc(U16 size, long count, int stored_at)
{
/* Returns handle number if successful, 0 or NULL if failure */
   U16 handle = 0;
   int success, use_this_type;
   long toallocate;

#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   BYTE *temp;
   long longtmp;
   int mempages = 0;
   struct XMM_Move MoveStruct;
#endif

   success = FALSE;
   toallocate = count * size;
   if (toallocate <= 0)     /* we failed, can't allocate > 2,147,483,647 */
      return((U16)success); /* or it wraps around to negative */

/* check structure for requested memory type (add em up) to see if
   sufficient amount is available to grant request */

   use_this_type = check_for_mem(stored_at, toallocate);
   if (use_this_type == NOWHERE) {
      DisplayError(stored_at, toallocate);
      goodbye();
   }

/* get next available handle */

   handle = next_handle();

   if (handle >= MAXHANDLES || handle <= 0) {
      DisplayHandle(handle);
      return((U16)success);
   /* Oops, do something about this! ????? */
   }

/* attempt to allocate requested memory type */
   switch (use_this_type)
   {
   default:
   case NOWHERE: /* MemoryAlloc */
      use_this_type = NOWHERE; /* in case nonsense value is passed */
      break;

#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   case EXTRA: /* MemoryAlloc */
      handletable[handle].Extra.size = toallocate;
      handletable[handle].Extra.stored_at = EXTRA;
      handletable[handle].Extra.extramemory = (BYTE *) extraseg,start_avail_extra);
      start_avail_extra += (U16)toallocate;
      numTOTALhandles++;
      success = TRUE;
      break;
#endif

   case MEMORY: /* MemoryAlloc */
/* Availability of memory checked in check_for_mem() */
      handletable[handle].Linearmem.memory = (BYTE *)malloc(toallocate);
      handletable[handle].Linearmem.size = toallocate;
      handletable[handle].Linearmem.stored_at = MEMORY;
      numTOTALhandles++;
      success = TRUE;
      break;

#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   case EXPANDED: /* MemoryAlloc */
      mempages = (int)((toallocate + 16383) >> 14);
      if (emmquery() != NULL) {
         handletable[handle].Expanded.mempages = mempages;
         handletable[handle].Expanded.expmemory = emmquery();
         if ((handletable[handle].Expanded.emmhandle = emmallocate(mempages)) != 0) {
            handletable[handle].Expanded.oldexppage = 0;
            handletable[handle].Expanded.size = toallocate;
            handletable[handle].Expanded.stored_at = EXPANDED;
            numTOTALhandles++;
            success = TRUE;
            break;
         }
      }

   case EXTENDED: /* MemoryAlloc */
   /* This is ugly!  Need memory to use extended memory. */
      if (charbuf == NULL) { /* first time through, allocate buffer */
         temp = (BYTE *)malloc((long)ext_xfer_size + FAR_RESERVE);
         if (temp != NULL) /* minimum free space + requested amount */
         {
            free(temp);
            charbuf = (BYTE *)malloc((long)ext_xfer_size);
         }
         else
            goto dodisk;
      }
      if (toallocate < ext_xfer_size)
         longtmp = (ext_xfer_size + 1023) >> 10;
      else
         longtmp = (toallocate + 1023) >> 10;
      if (xmmquery() != 0
        && (handletable[handle].Extended.xmmhandle = xmmallocate((U16)(longtmp))) != 0)
      {
         longtmp /= (ext_xfer_size >> 10);
         handletable[handle].Extended.mempages = (int)longtmp;
         memset(charbuf, 0, (int)ext_xfer_size); /* zero the buffer */
         MoveStruct.SourceHandle = 0;    /* Source is in conventional memory */
         MoveStruct.SourceOffset = (U32)charbuf;
         MoveStruct.DestHandle   = handletable[handle].Extended.xmmhandle;
         MoveStruct.DestOffset   = 0;
         MoveStruct.Length       = (long)ext_xfer_size;
         while (--longtmp >= 0) {
            if ((success = xmmmoveextended(&MoveStruct)) == 0) break;
            MoveStruct.DestOffset += ext_xfer_size;
         }
         if (success) {
            /* put in structure */
            success = TRUE;
            handletable[handle].Extended.size = toallocate;
            handletable[handle].Extended.stored_at = EXTENDED;
            numTOTALhandles++;
            numEXThandles++;
            break;
         }
         xmmdeallocate(handletable[handle].Extended.xmmhandle); /* Clear the memory */
         handletable[handle].Extended.xmmhandle = 0;            /* Signal same */
      }
      /* need to fall through and use disk memory, or will crash fractint */
dodisk:
#endif
   case DISK: /* MemoryAlloc */
      memfile[9] = (char)(handle % 10 + (int)'0');
      memfile[8] = (char)((handle % 100) / 10 + (int)'0');
      memfile[7] = (char)((handle % 1000) / 100 + (int)'0');
      if (disktarga)
         handletable[handle].Disk.file = dir_fopen(workdir,light_name, "a+b");
      else
         handletable[handle].Disk.file = dir_fopen(tempdir,memfile, "w+b");
      rewind(handletable[handle].Disk.file);
      if (fseek(handletable[handle].Disk.file,toallocate,SEEK_SET) != 0)
         handletable[handle].Disk.file = NULL;
      if (handletable[handle].Disk.file == NULL) {
         handletable[handle].Disk.stored_at = NOWHERE;
         use_this_type = NOWHERE;
         WhichDiskError(1);
         DisplayMemory();
         driver_buzzer(3);
         break;
      }
      numTOTALhandles++;
      success = TRUE;
      fclose(handletable[handle].Disk.file); /* so clusters aren't lost if we crash while running */
      if (disktarga)
         handletable[handle].Disk.file = dir_fopen(workdir,light_name, "r+b");
      else
         handletable[handle].Disk.file = dir_fopen(tempdir,memfile,"r+b"); /* reopen */
      rewind(handletable[handle].Disk.file);
      handletable[handle].Disk.size = toallocate;
      handletable[handle].Disk.stored_at = DISK;
      use_this_type = DISK;
      break;
   } /* end of switch */

   if (stored_at != use_this_type && debugflag == 10000) {
      char buf[MSGLEN];
      char nmsg[MSGLEN];
      static char fmsg[] = {"Asked for %s, allocated %lu bytes of %s, handle = %u."};
      strcpy(nmsg,fmsg);
      sprintf(buf,nmsg,memstr[stored_at],toallocate,memstr[use_this_type],handle);
      stopmsg(STOPMSG_INFO_ONLY | STOPMSG_NO_BUZZER,(char *)buf);
      DisplayMemory();
   }

   if (success)
      return (handle);
   else      /* return 0 if failure */
      return 0;
}

void MemoryRelease(U16 handle)
{
   switch(handletable[handle].Nowhere.stored_at)
   {
   case NOWHERE: /* MemoryRelease */
      break;

#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   case EXTRA: /* MemoryRelease */
/* Deallocate in the reverse order of allocation. */
      start_avail_extra -= (U16)handletable[handle].Extra.size;
      handletable[handle].Extra.size = 0;
      handletable[handle].Extra.stored_at = NOWHERE;
      handletable[handle].Extra.extramemory = NULL;
      numTOTALhandles--;
      break;
#endif

   case MEMORY: /* MemoryRelease */
      free(handletable[handle].Linearmem.memory);
      handletable[handle].Linearmem.memory = NULL;
      handletable[handle].Linearmem.size = 0;
      handletable[handle].Linearmem.stored_at = NOWHERE;
      numTOTALhandles--;
      break;

#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   case EXPANDED: /* MemoryRelease */
      emmdeallocate(handletable[handle].Expanded.emmhandle);
      handletable[handle].Expanded.emmhandle = 0;
      handletable[handle].Expanded.size = 0;
      handletable[handle].Expanded.stored_at = NOWHERE;
      numTOTALhandles--;
      break;

   case EXTENDED: /* MemoryRelease */
 /* memory allocated for this must be released */
      numEXThandles--;
      xmmdeallocate(handletable[handle].Extended.xmmhandle);
      if (numEXThandles == 0) {
         free(charbuf);
         charbuf = NULL;
      }
      handletable[handle].Extended.xmmhandle = 0;
      handletable[handle].Extended.size = 0;
      handletable[handle].Extended.stored_at = NOWHERE;
      numTOTALhandles--;
      break;
#endif

   case DISK: /* MemoryRelease */
      memfile[9] = (char)(handle % 10 + (int)'0');
      memfile[8] = (char)((handle % 100) / 10 + (int)'0');
      memfile[7] = (char)((handle % 1000) / 100 + (int)'0');
      fclose(handletable[handle].Disk.file);
      dir_remove(tempdir,memfile);
      handletable[handle].Disk.file = NULL;
      handletable[handle].Disk.size = 0;
      handletable[handle].Disk.stored_at = NOWHERE;
      numTOTALhandles--;
      break;
   } /* end of switch */
}

int MoveToMemory(BYTE *buffer,U16 size,long count,long offset,U16 handle)
{ /* buffer is a pointer to local memory */
/* Always start moving from the beginning of buffer */
/* offset is the number of units from the start of the allocated "Memory" */
/* to start moving the contents of buffer to */
/* size is the size of the unit, count is the number of units to move */
/* Returns TRUE if successful, FALSE if failure */
   BYTE diskbuf[DISKWRITELEN];
   long start; /* offset to first location to move to */
   long tomove; /* number of bytes to move */
   U16 numwritten;
   int success;

   success = FALSE;
   start = (long)offset * size;
   tomove = (long)count * size;
   if (debugflag == 10000)
      if (CheckBounds(start, tomove, handle))
         return(success); /* out of bounds, don't do it */

   switch(handletable[handle].Nowhere.stored_at)
   {
   case NOWHERE: /* MoveToMemory */
      DisplayHandle(handle);
      break;

   case MEMORY: /* MoveToMemory */
		memcpy(handletable[handle].Linearmem.memory + start, buffer, size*count);
		success = TRUE; /* No way to gauge success or failure */
		break;

   case DISK: /* MoveToMemory */
      rewind(handletable[handle].Disk.file);
      fseek(handletable[handle].Disk.file,start,SEEK_SET);
      while (tomove > DISKWRITELEN)
      {
         memcpy(diskbuf,buffer,(U16)DISKWRITELEN);
         numwritten = (U16)write1(diskbuf,(U16)DISKWRITELEN,1,handletable[handle].Disk.file);
         if (numwritten != 1) {
            WhichDiskError(3);
            goto diskerror;
         }
         tomove -= DISKWRITELEN;
         buffer += DISKWRITELEN;
      }
      memcpy(diskbuf,buffer,(U16)tomove);
      numwritten = (U16)write1(diskbuf,(U16)tomove,1,handletable[handle].Disk.file);
      if (numwritten != 1) {
         WhichDiskError(3);
         break;
      }
      success = TRUE;
diskerror:
      break;
   } /* end of switch */
   if (!success && debugflag == 10000)
      DisplayHandle(handle);
   return (success);
}

int MoveFromMemory(BYTE *buffer,U16 size,long count,long offset,U16 handle)
{ /* buffer points is the location to move the data to */
/* offset is the number of units from the beginning of buffer to start moving */
/* size is the size of the unit, count is the number of units to move */
/* Returns TRUE if successful, FALSE if failure */
#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   int currpage;
   long tmplength;
   struct XMM_Move MoveStruct;
#endif
   BYTE diskbuf[DISKWRITELEN];
   long start; /* first location to move */
   long tomove; /* number of bytes to move */
   U16 numread, i;
   int success;

   success = FALSE;
   start = (long)offset * size;
   tomove = (long)count * size;
   if (debugflag == 10000)
      if (CheckBounds(start, tomove, handle))
         return(success); /* out of bounds, don't do it */

   switch(handletable[handle].Nowhere.stored_at)
   {
   case NOWHERE: /* MoveFromMemory */
      DisplayHandle(handle);
      break;

#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   case EXTRA: /* MoveFromMemory */
      memcpy(buffer, handletable[handle].Extra.extramemory+start, (U16)tomove);
      success = TRUE; /* No way to gauge success or failure */
      break;
#endif

   case MEMORY: /* MoveFromMemory */
      for(i=0;i<size;i++) {
         memcpy(buffer, handletable[handle].Linearmem.memory+start, (U16)count);
         start += count;
         buffer += count;
      }
      success = TRUE; /* No way to gauge success or failure */
      break;

#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   case EXPANDED: /* MoveFromMemory */
      currpage = (int)(start / EXPWRITELEN);
      exp_seek(handle, currpage);
/* Not on a page boundary, move data up to next page boundary */
      tmplength = (currpage + 1) * EXPWRITELEN - start;
      start -= (currpage * EXPWRITELEN);
      if (tmplength > tomove)
         tmplength = tomove;
      memcpy(buffer, handletable[handle].Expanded.expmemory+start, (U16)tmplength);
      buffer += tmplength;
      tomove -= tmplength;
/* At a page boundary, move until less than a page left */
      while (tomove >= EXPWRITELEN)
      {
         currpage++;
         exp_seek(handle, currpage);
         memcpy(buffer, handletable[handle].Expanded.expmemory, (U16)EXPWRITELEN);
         buffer += EXPWRITELEN;
         tomove -= EXPWRITELEN;
      }
/* Less than a page left, move it */
      if (tomove > 0) /* still some left */
      {
         currpage++;
         exp_seek(handle, currpage);
         memcpy(buffer, handletable[handle].Expanded.expmemory, (U16)tomove);
      }
      exp_seek(handle, 0); /* flush the last page out of the page frame */
      success = TRUE; /* No way to gauge success or failure */
      break;

   case EXTENDED: /* MoveFromMemory */
      MoveStruct.Length = ext_xfer_size;
      MoveStruct.SourceHandle = handletable[handle].Extended.xmmhandle;
      MoveStruct.SourceOffset = (U32)start;
      MoveStruct.DestHandle = 0; /* Destination is conventional memory */
      MoveStruct.DestOffset = (U32)charbuf;
      while (tomove > ext_xfer_size)
      {
         xmmmoveextended(&MoveStruct);
         memcpy(buffer,charbuf,(int)ext_xfer_size);
         buffer += ext_xfer_size;
         tomove -= ext_xfer_size;
         start += ext_xfer_size;
         MoveStruct.SourceOffset = (U32)start;
      }
      MoveStruct.Length = (tomove % 2) ? tomove + 1 : tomove; /* must be even */
      success = xmmmoveextended(&MoveStruct);
      memcpy(buffer,charbuf,(U16)tomove);
      break;
#endif

   case DISK: /* MoveFromMemory */
      rewind(handletable[handle].Disk.file);
      fseek(handletable[handle].Disk.file,start,SEEK_SET);
      while (tomove > DISKWRITELEN)
      {
         numread = (U16)fread(diskbuf,(U16)DISKWRITELEN,1,handletable[handle].Disk.file);
         if (numread != 1 && !feof(handletable[handle].Disk.file)) {
            WhichDiskError(4);
            goto diskerror;
         }
         memcpy(buffer,diskbuf,(U16)DISKWRITELEN);
         tomove -= DISKWRITELEN;
         buffer += DISKWRITELEN;
      }
      numread = (U16)fread(diskbuf,(U16)tomove,1,handletable[handle].Disk.file);
      if (numread != 1 && !feof(handletable[handle].Disk.file)) {
         WhichDiskError(4);
         break;
      }
      memcpy(buffer,diskbuf,(U16)tomove);
      success = TRUE;
diskerror:
      break;
   } /* end of switch */
   if (!success && debugflag == 10000)
      DisplayHandle(handle);
   return (success);
}

int SetMemory(int value,U16 size,long count,long offset,U16 handle)
{ /* value is the value to set memory to */
/* offset is the number of units from the start of allocated memory */
/* size is the size of the unit, count is the number of units to set */
/* Returns TRUE if successful, FALSE if failure */
#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   int currpage;
   long tmplength;
   struct XMM_Move MoveStruct;
#endif
   BYTE diskbuf[DISKWRITELEN];
   long start; /* first location to set */
   long tomove; /* number of bytes to set */
   U16 numwritten, i;
   int success;

   success = FALSE;
   start = (long)offset * size;
   tomove = (long)count * size;
   if (debugflag == 10000)
      if (CheckBounds(start, tomove, handle))
         return(success); /* out of bounds, don't do it */

   switch(handletable[handle].Nowhere.stored_at)
   {
   case NOWHERE: /* SetMemory */
      DisplayHandle(handle);
      break;

#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   case EXTRA: /* SetMemory */
      memset(handletable[handle].Extra.extramemory+start, value, (U16)tomove);
      success = TRUE; /* No way to gauge success or failure */
      break;
#endif

   case MEMORY: /* SetMemory */
      for(i=0;i<size;i++) {
         memset(handletable[handle].Linearmem.memory+start, value, (U16)count);
         start += count;
      }
      success = TRUE; /* No way to gauge success or failure */
      break;

#if (!defined(XFRACT) && !defined(WINFRACT) && !defined(_WIN32))
   case EXPANDED: /* SetMemory */
      currpage = (int)(start / EXPWRITELEN);
      exp_seek(handle, currpage);
/* Not on a page boundary, move data up to next page boundary */
      tmplength = (currpage + 1) * EXPWRITELEN - start;
      start -= (currpage * EXPWRITELEN);
      if (tmplength > tomove)
         tmplength = tomove;
      memset(handletable[handle].Expanded.expmemory+start, value, (U16)tmplength);
      tomove -= tmplength;
/* At a page boundary, move until less than a page left */
      while (tomove >= EXPWRITELEN)
      {
         currpage++;
         exp_seek(handle, currpage);
         memcpy(handletable[handle].Expanded.expmemory, &value, (U16)EXPWRITELEN);
         tomove -= EXPWRITELEN;
      }
/* Less than a page left, move it */
      if (tomove > 0) /* still some left */
      {
         currpage++;
         exp_seek(handle, currpage);
         memcpy(handletable[handle].Expanded.expmemory, &value, (U16)tomove);
      }
      exp_seek(handle, 0); /* flush the last page out of the page frame */
      success = TRUE; /* No way to gauge success or failure */
      break;

   case EXTENDED: /* SetMemory */
      MoveStruct.Length = ext_xfer_size;
      MoveStruct.SourceHandle = 0; /* Source is conventional memory */
      MoveStruct.SourceOffset = (U32)charbuf;
      MoveStruct.DestHandle = handletable[handle].Extended.xmmhandle;
      MoveStruct.DestOffset = (U32)start;
      memset(charbuf, value, (int)ext_xfer_size);
      while (tomove > ext_xfer_size)
      {
         xmmmoveextended(&MoveStruct);
         start += ext_xfer_size;
         tomove -= ext_xfer_size;
         MoveStruct.DestOffset = (U32)(start);
      }
      MoveStruct.Length = (tomove % 2) ? tomove + 1 : tomove; /* must be even */
      success = xmmmoveextended(&MoveStruct);
      break;
#endif

   case DISK: /* SetMemory */
      memset(diskbuf, value, (U16)DISKWRITELEN);
      rewind(handletable[handle].Disk.file);
      fseek(handletable[handle].Disk.file,start,SEEK_SET);
      while (tomove > DISKWRITELEN)
      {
         numwritten = (U16)write1(diskbuf,(U16)DISKWRITELEN,1,handletable[handle].Disk.file);
         if (numwritten != 1) {
            WhichDiskError(2);
            goto diskerror;
         }
         tomove -= DISKWRITELEN;
      }
      numwritten = (U16)write1(diskbuf,(U16)tomove,1,handletable[handle].Disk.file);
      if (numwritten != 1) {
         WhichDiskError(2);
         break;
      }
      success = TRUE;
diskerror:
      break;
   } /* end of switch */
   if (!success && debugflag == 10000)
      DisplayHandle(handle);
   return (success);
}
