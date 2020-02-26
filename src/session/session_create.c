/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: session_create.c,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "session.h"

int	main(int argc, char *argv[]) {
	session_struct s;

	char	*lock;
	char 	buffer[1024];
	int 	i,n;

	if (argc != 3) {
		fprintf(stderr,"usage : %s database url\n", argv[0]);
		exit(1);
	}

	if (create_lock(argv[1],argv[2],&s) != 0 ) {
		fprintf(stderr,"create_lock failed\n");
		exit(1);
	}

	if (is_locked(argv[1])) {
		fprintf(stdout,"%s is locked as %s\n",argv[1], s.lockfile);
	}
	else {
		fprintf(stderr,"error : %s is free?!\n",argv[1]);
		exit(1);
	}

	/* We make this file world readable so that the daemon can access it */
	if (fchmod(s.fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1) {
		fprintf(stderr,"error : failed to fchmod\n");
		exit(1);
	}
	else {
		fprintf(stdout,"%s is world writable\n", s.lockfile);
	}

	close_lock(&s);

	return(0);
}
