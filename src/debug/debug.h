/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: debug.h,v 1.2 2006/12/19 08:33:15 hochstenbach Exp $
 * 
 */
#ifndef __debug_h
#define __debug_h

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#ifndef DEBUG
 #define	DEBUG  0
#endif

/* Set the log stream to a new value.
 */
void 	set_log_stream(FILE *fh);

/* This function writes a log message to stderr.
 *
 * Usage:  log(const char *format, ...)'
 */
#define log( format, args...) _logmsg(1,"log",format,##args)

/* This function writes warning messges to stderr.
 *
 * Usage:  warn(const char *format, ...);
 */
#define	warn( format, args...) _logmsg(1,"warn",format,##args)

/* This function writes warning messges to stderr
 * only if global constant DEBUG is set to a non
 * zero value.
 *
 * Usage:  debug(const char *format, ...);
 */
#define	debug( format, args...) _logmsg(DEBUG,"debug",format,##args)


void	_logmsg(int on, const char *type, const char *fmt, ...);

#endif
