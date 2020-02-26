/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: session_test.c,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include "session.h"

#define	DBFILE "kjfdghdsjhfkjhdskjfkjfjakf/sdlkjffkjshfkjh/dsfdsf/das"
#define URL    "http://somewhere.com/dfdsf/dsfsdf/fsdfdsfsdf.xml"

#define DBFILE2 "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq"
#define URL2    "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.lanl.gov"

int	main(void) {
	session_struct s;
	session_struct sarr[1024];

	char 	buffer[1024];
	int 	i,n;
	char	*lock;

	if (create_lock(DBFILE,URL,&s) != 0) {
		fprintf(stderr,"create_lock failed\n");
		exit(1);
	}

	if (is_locked(DBFILE)) {
		fprintf(stdout,"%s is locked\n",DBFILE);
	}
	else {
		fprintf(stdout,"%s is free (:-S ?!\n",DBFILE);
	}

	close_lock(&s);

	n = 0;

	while ( (i = next_lock(&sarr[n],SESSION_UNPROCESSED_TYPES)) == 0) {
		n++;
	}

	for( i = 0 ; i < n ; i++) {
		close_lock(&sarr[i]);
	}

	if (n == 0) {
		fprintf(stderr,"next_lock found no lock files?!\n");
		exit(1);
	}
	else if (n > 1) {
		fprintf(stderr,"warning: found %d lock files...\n", n);
	}
	else {
		fprintf(stdout,"found %d lock file\n", n);
	}

	fprintf(stdout,"reading first lock file...\n");

	next_lock(&s,SESSION_UNPROCESSED_TYPES);

	fprintf(stdout,"database : %s\n",s.database);
	fprintf(stdout,"url      : %s\n",s.url);
	fprintf(stdout,"lockfile : %s\n",s.lockfile);
	fprintf(stdout,"status   : %d\n",s.status);
	fprintf(stdout,"modified : %s"  , ctime(&(s.modified)));

	strcpy(s.database,URL2);
	strcpy(s.url,URL2);
	s.status=15;

	if (write_lock(&s) == -1) {
		fprintf(stderr,"write_lock failed\n");
		exit(1);
	}

        fprintf(stdout,"database : %s\n",s.database);
        fprintf(stdout,"url      : %s\n",s.url);
        fprintf(stdout,"lockfile : %s\n",s.lockfile);
        fprintf(stdout,"status   : %d\n",s.status);
        fprintf(stdout,"modified : %s"  , ctime(&(s.modified)));	

	if (release_lock(&s) == -1) {
		fprintf(stderr,"release_lock failed\n");
		exit(1);
	}

/*	fprintf(stderr,"Doing loop\n");
 *
 *	for (i = 0 ; i < 10000 ; i++) {
 *		if (create_lock(DBFILE,URL,&s) == -1) {
 *			fprintf(stderr,"create_lock failed\n");
 *			exit(1);
 *		}
 *
 *		if (release_lock(&s) == -1) {
 *			fprintf(stderr,"release_lock failed\n");
 *			exit(1);
 *		}
 *	}
 */
	return(0);
}
