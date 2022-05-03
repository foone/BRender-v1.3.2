#ifndef _LOG_H_
#define _LOG_H_

void log_open(void);
void log_close(void);
void syslog(char *s, ...);
#endif
