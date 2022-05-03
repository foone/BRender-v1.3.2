/*
 * log.c : logging and exit functions
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <stdarg.h>

//#include "log.h"

#define FLUSH_LOGFILE


#define LOG_FILENAME        "log"
static FILE *logfile = NULL;
int nolog = 0;
int debug_mode = 0;


/* Universal logging function */
void _cdecl syslog(char *s, ...)
{

  va_list ap;

  return;


  if (!nolog) {
    va_start(ap, s);
    vfprintf(logfile, s, ap);
    fprintf(logfile, "\n");
#ifdef FLUSH_LOGFILE
    fflush(logfile);
#endif
    va_end(ap);
  }

}


void log_open(void)
{
  if (nolog) return;

  return;
  logfile = fopen(LOG_FILENAME, "w");  
  if (!logfile) {
    perror("log_open");
    exit(0);
  }
  syslog("Logfile opened successfully");

}



void log_close(void)
{
  if (nolog) return;

  if (logfile) {
    fprintf(logfile, "Closing logfile\n");
    fclose(logfile);
  }
}




#if 0

/* Universal debug crash/non-debug warning */
void derror(char *s, ...)
{
  va_list ap;

  /* Send the error message */
  if (debug_mode) log("*** EMERGENCY EXIT ***");
  else log("*** TERMINATION ERROR ***");

  /* Do the file logging */
  if (!nolog) {
    va_start(ap, s);
    vfprintf(logfile, s, ap);
    fprintf(logfile, "\n");
    va_end(ap);
  }

  /* If this is an error, exit, else just return */
  if (debug_mode) {
    log_close();
    va_start(ap, s);
    vprintf(s, ap);
    printf("\n");
    va_end(ap);

    exit(0);
  }

}


void error(char *s, ...)
{
  va_list ap;

  log("*** EMERGENCY EXIT ***");
  if (!nolog) {
    va_start(ap, s);
    vfprintf(logfile, s, ap);
    fprintf(logfile, "\n");
    va_end(ap);
  }
  log_close();

  va_start(ap, s);
  vprintf(s, ap);
  printf("\n");
  va_end(ap);

  exit(0);
}

#endif
