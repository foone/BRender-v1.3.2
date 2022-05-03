/*
 * log.c : logging functions for debugging
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <stdarg.h>

#define NO_LOG

#ifdef NO_LOG

void log_open(void){}
void log_close(void){}
void syslog(char *s, ...){}

#else


#ifdef WIN32
#include <windows.h>
#endif

#include "log.h"

#define FLUSH_LOGFILE


#define LOG_FILENAME        "log"
static FILE *logfile;
int nolog = 0;
int debug_mode = 0;


static char debug_string[256];


void log_open(void)
{
#ifdef WIN32
   OutputDebugString("Log open\n");
#else
  logfile = fopen(LOG_FILENAME, "w");  
  if (!logfile) {
    perror("log_open");
    exit(0);
  }
  syslog("Logfile opened successfully");
#endif
}



void log_close(void)
{
#ifdef WIN32
   OutputDebugString("Log Close\n");
#else
  fprintf(logfile, "Closing logfile\n");
  fclose(logfile);
#endif
}



/* Universal logging function */
void syslog(char *s, ...)
{
  va_list ap;

#ifdef WIN32
  if (!nolog)
   {
   va_start(ap, s);
   vsprintf(debug_string, s, ap);
   OutputDebugString(debug_string);
   OutputDebugString("\n");
   va_end(ap);
   }
#else
  if (!nolog)
   {
   va_start(ap, s);
   vfprintf(logfile, s, ap);
   fprintf(logfile, "\n");
#ifdef FLUSH_LOGFILE
   fflush(logfile);
#endif
   va_end(ap);
   }
#endif
}



#endif // NO_LOG
