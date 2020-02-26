/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: child.c,v 1.2 2006/12/19 08:33:14 hochstenbach Exp $
 * 
 */
#include "defs.h"
#include <stdarg.h>

/* The function fetches XML from the web and validates it.
 * Returns 0 on success or -1 on error
 */
int	process_request(request_struct *r) {
	time_t	t;
	int 	status, exit_status;

	char	*gwhome = getenv("GWHOME");

	char	*url;
	char	*database;

	char	*cache_app;
	char	cmdstring[2048];
	
	debug("process_request - started");

	url = r->session->url;

	if (url == NULL || strlen(url) == 0) {
		warn("process_request - no url set in session");
		return(-1);
	}

	database = r->session->database;

	if (database == NULL || strlen(database) == 0) {
		warn("process_request - no database set in session");
		return(-1);
	}

	cache_app = config_param(config,"create_cache");

	if (strlen(cache_app) == 0) {
		warn("process_request - no create_cache set in config file");
		return(-1);
	}

	/* Setting up caching process */
	sprintf(cmdstring, "%s/bin/%s %s %s", gwhome, cache_app, database, url);

	log("process_request - starting create_cache");

	status = system((char *)cmdstring);
	exit_status = WEXITSTATUS(status);
	
	log("process_request - finished create_cache : status %d", exit_status);

	if (exit_status != 0) {

		read_lock(r->session);

		log("process_request - create_cache failed");

		return(-1);
	}

	debug("process_request - ended successfully");

	return(0);
}
