/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: defs.h,v 1.2 2006/12/19 08:33:14 hochstenbach Exp $
 * 
 */
#ifndef	__defs_h
#define	__defs_h

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <debug.h>
#include <session.h>
#include <cconfig.h>

#define	SERVER_FORK	 1				/* Set to 1 to run the daemon automatically in the background */
#define	LOGFILE		 "/tmp/srepod.log"		/* The path to the rsepod log file */
#define	SLEEP_TIME	 30				/* Ammount of time to sleep after all requests are handled */
#define	MAX_REQUESTS	 5				/* Maximum number of requests to handle at one time */
#define MAX_URL_LENGTH	 1024				/* Maximum length of an url */
#define MAX_PATH_LENGTH	 1024				/* Maximum length of a pathname */	

enum	request_status {
	REQUEST_PENDING,
	REQUEST_FINISHED,
	REQUEST_FAILED,
};

typedef	struct {
	int			pid;
	enum request_status	status;
	session_struct		*session;
} request_struct;

int	process_request(request_struct *);

int	config;

#endif
