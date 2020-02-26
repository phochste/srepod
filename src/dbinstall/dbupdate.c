/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: dbupdate.c,v 1.2 2006/12/19 08:33:14 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <gdbm.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

int	main(int argc, char *argv[]) {
	struct stat	buf;
	GDBM_FILE       dbf;
	char		*filename;
	char		buffer[1024];
	time_t		t;
	datum           key,data;

	if (argc != 3) {
		fprintf(stderr, "usage: %s dbfile last_modified_str\n", argv[0]);
		exit(1);
	}

	filename      = argv[1];

	/* If the dest file exists, then update it
	 * otherwise return 0.
	 */
	if (stat(filename, &buf) == -1) {
		return(0);
	} 

	dbf = gdbm_open((char *)filename, 1024, GDBM_WRITER, 0644, 0);	

	if (dbf == NULL) {
		fprintf(stderr, "failed to open database for writing - gdbm_errno %d\n", gdbm_errno);
		return(2);
	}

	key.dptr   = "lmf";
	key.dsize  = sizeof(key.dptr) - 1;

	data.dptr  = argv[2];

	data.dsize = strlen(argv[2]);

	if (gdbm_store(dbf,key,data,GDBM_REPLACE) != 0) {
		gdbm_close(dbf);
		fprintf(stderr, "failed to (un)lock database");
		return(3);
	}

	time(&t);

	sprintf(buffer, "%d", t);

	key.dptr   = "mod";
	key.dsize  = sizeof(key.dptr) - 1;

	data.dptr  = buffer;
	data.dsize = strlen(buffer);

	if (gdbm_store(dbf,key,data,GDBM_REPLACE) != 0) {
		gdbm_close(dbf);
		fprintf(stderr, "failed to update database modification time");
		return(4);
	}


	gdbm_close(dbf);

	return(0);
}
