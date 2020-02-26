/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: session_set.c,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include "session.h"

int	main(int argc, char *argv[]) {
	session_struct session;
	int	       found = 0;

	char	*lock;
	char 	buffer[1024];
	int 	i,n;

	if (argc != 3) {
		fprintf(stderr,"usage : %s database status\n", argv[0]);
		exit(1);
	}

	if (!is_locked(argv[1])) {
		fprintf(stderr,"error : %s is not locked(1)\n", argv[1]);
		exit(1);
	}

	while(next_lock(&session, SESSION_ALL_TYPES) == 0) {
		if (strcmp(session.database,argv[1]) == 0) {
			found = 1;
			break;
		}
	}

	if (!found) {
		fprintf(stderr,"error : %s is not locked(2)\n", argv[1]);
		exit(2);
	}

	if (strcmp(argv[2],"SESSION_REQUEST") == 0) {
		session.status = SESSION_REQUEST;
	}
	else if (strcmp(argv[2],"SESSION_CONTACTING") == 0) {
		session.status = SESSION_CONTACTING;
	}
	else if (strcmp(argv[2],"SESSION_VALIDATING") == 0) { 
		session.status = SESSION_VALIDATING;
	}
	else if (strcmp(argv[2],"SESSION_WRITING") == 0) {
		session.status = SESSION_WRITING;
	}
	else if (strcmp(argv[2],"SESSION_INVALID") == 0) {
		session.status = SESSION_INVALID;
	}
	else if (strcmp(argv[2],"SESSION_NO_RESOURCES_LEFT") == 0) {
		session.status = SESSION_NO_RESOURCES_LEFT;
	}
	else if (strcmp(argv[2],"SESSION_CONFIG_ERROR") == 0) {
		session.status = SESSION_CONFIG_ERROR;
	}
	else {
		fprintf(stderr,"error : %s is an unknown status\n", argv[2]);
		exit(2);
	}

	write_lock(&session);

	close_lock(&session);

	return(0);
}
