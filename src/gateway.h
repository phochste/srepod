/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: gateway.h,v 1.2 2006/12/19 08:33:11 hochstenbach Exp $
 * 
 * All cmd_ functions return the number of bytes written
 * to stdout, or -1 on error. On error the cmd_ functions
 * *MUST* set the errmsg and errcode variables
 * or call one of the has_ functions.
 * The has_ functions return 1 success and 0 on error. On
 * error the errmsg and errcode variables are set.
 */
#ifndef __cyar_h
#define __cyar_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <timefunc.h>

#define MAX_PATH_LENGTH		1024
#define	GATEWAY_LOG		"gateway.log"
#define	RETRY_AFTER_SECS 	60	/* Number of seconds to let a harvester wait after sending a 503 */

enum    error_type {
        ERR_BADARGUMENT,
	ERR_BADRESUMPTIONTOKEN,
        ERR_BADVERB,
	ERR_CANNOTDISSEMINATEFORMAT,
        ERR_IDDOESNOTEXIST,
	ERR_NOMETADATAFORMATS,
	ERR_NORECORDSMATCH,
        ERR_NOSETHIERARCHY,
	ERR_HTTP_400 = 400,
	ERR_HTTP_404 = 404,
	ERR_HTTP_500 = 500,
	ERR_HTTP_501 = 501,
	ERR_HTTP_502 = 502,
	ERR_HTTP_503 = 503,
	ERR_HTTP_504 = 504,
	ERR_HTTP_505 = 505,
	ERR_UNKNOWN  = 999
};

enum	list_type {
	COUNT=0,
	IDENTIFIERS=1,
	RECORDS=2
};

char    *errmsg = "";
int     errcode = 0;

/* All cmd_ functions return the number of bytes written
 * to stdout, or -1 on error. On error the cmd_ functions
 * *MUST* set the errmsg and errcode variables
 * or call one of the has_ functions.
 */
int     cmd_error(void);
int     cmd_header(const char **valid_keys);
int     cmd_footer(void);

int     cmd_identify(void);
int     cmd_listsets(void);
int     cmd_listmetadataformats(void);
int     cmd_list(enum list_type);
int     cmd_getrecord(void);

/* This subroutine checks some environment variables for security
 * reasons. Returns 1 if everything is okay, returns 0 on error.
 */
#define	SEC_MAX_CONTENT_LENGTH 1024
int	has_secure_env(void);

/* This subroutine checks if all the keys in the CGI query are
 * valid, but doesn't check the content of the keys except
 * for the 'verb' key which is mandatory.
 * Returns 1 when all keys are valid, return 0 on error.
 */
int     has_valid_keys(const char** valid_keys);

/* This subroutine returns 1 if both from and until have a valid
 * syntax, granularity and from is earlier than until if set.
 * The subroutine returns 0 in all other cases. 
 */
int     has_valid_dates(const char *from,const char *until);

/* This subroutine returns 0 if the argument string is a 
 * valid uri. Return -1 otherwise.
 */
int	is_uri(const char *s);

#endif
