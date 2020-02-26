/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: debug.c,v 1.2 2006/12/19 08:33:15 hochstenbach Exp $
 * 
 */
#include "debug.h"


FILE *logfh;

/* Set the log stream to a new value.
 *  */
void    set_log_stream(FILE *fh) {
	logfh = fh;
}

void	_logmsg(int on, const char *type, const char *fmt, ...) {
	va_list	arg;
	time_t  t;
	char	*dptr;

	time(&t);
        dptr = ctime(&t);
        dptr[strlen(dptr) - 1] = '\0';

	if (on != 0) {
		if (logfh == NULL) logfh = stderr;

		(void) fprintf(logfh,"%s [%d:%s] : ", dptr, getpid(), type);
		va_start(arg, fmt);
    		(void) vfprintf(logfh, fmt, arg);
    		va_end(arg);
		(void) fprintf(logfh,"\n");
		fflush(logfh);
	}
}

