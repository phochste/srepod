/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: dbinstall.c,v 1.2 2006/12/19 08:33:14 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <gdbm.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

int	update_gdbm(char *source, char *dest);
int	create_gdbm(char *source, char *dest);
char	*errmsg;
char	errcode;

int	main(int argc, char *argv[]) {
	char		*source, *dest;
	struct	stat	buf;
	int		ret;

	if (argc != 3) {
		fprintf(stderr, "usage: %s source dest\n", argv[0]);
		exit(1);
	}

	source = argv[1];
	dest   = argv[2];

	/* If the dest file exists, then we do an update. 
	 * Otherwise we do a create.
	 */
	if (stat(dest, &buf) == -1) {
		ret = create_gdbm(source, dest);
	} 
	else {
		ret = update_gdbm(source, dest);
	}

	if (ret == -1) {
		fprintf(stderr, "%s failed : [%d] %s\n", argv[0], errcode, errmsg);
		return(errcode);
	}
	
	return(0);
}

int	update_gdbm(char *source, char *dest) {
	GDBM_FILE       dbf_source, dbf_dest;
	char            buffer[1024];
	time_t          t;
	datum           key,data;

	/* We edit some fields from the source, copy
	 * some fields from the dest and overwrite
	 * the dest with the source.
	 */
	dbf_source = gdbm_open((char *)source, 1024, GDBM_WRITER, 0644, 0);
	dbf_dest   = gdbm_open((char *)dest, 1024, GDBM_READER, 0644, 0);

	if (dbf_source == NULL) {
		errmsg  = "update_gdbm - failed to open source file for writing";
		errcode = 2;
		return(-1);
	}

	if (dbf_dest == NULL) {
		gdbm_close(dbf_source);
		errmsg = "update_gdbm - failed to open dest file for reading";
		errcode = 3;
		return(-1);
	}

        key.dptr   = "crt";
        key.dsize  = sizeof(key.dptr) - 1;

	data = gdbm_fetch(dbf_dest,key);

	if (data.dptr == NULL) {
		gdbm_close(dbf_source);
		gdbm_close(dbf_dest);
		errmsg = "update_gdbm - failed to read crt from dest";
		errcode = 4;
		return(-1);
	}

	if (gdbm_store(dbf_source,key,data,GDBM_REPLACE) != 0) {
		gdbm_close(dbf_source);
                gdbm_close(dbf_dest);
		errmsg = "update_gdbm - failed to update crt in source";
		errcode = 5;
		return(-1);
	}

	free(data.dptr);
	
	time(&t);
	sprintf(buffer, "%d", t);

	key.dptr   = "mod";
	key.dsize  = sizeof(key.dptr) - 1;

	data.dptr  = buffer;
	data.dsize = strlen(buffer);

	if (gdbm_store(dbf_source,key,data,GDBM_REPLACE) != 0) {
		gdbm_close(dbf_source);
		gdbm_close(dbf_dest);
		errmsg = "update_gdbm - failed to update mod in source";
		errcode = 6;
		return(-1);
	}

	key.dptr   = "lck";
	key.dsize  = sizeof(key.dptr) - 1;

	data.dptr  = "0";
	data.dsize = sizeof(data.dptr) - 1;

	if (gdbm_store(dbf_source,key,data,GDBM_REPLACE) != 0) {
		gdbm_close(dbf_source);
		gdbm_close(dbf_dest);
		errmsg = "update_gdbm - failed to update lck in source";
		errcode = 7;
		return(-1);
	}

	if (rename(source, dest) == -1) {
		errmsg = "update_gdbm - rename source to dest";
		errcode = 8;
		return(-1);
	}

	gdbm_close(dbf_source);
	gdbm_close(dbf_dest);

	return(0);
}

int	create_gdbm(char *source, char *dest) {
	GDBM_FILE	dbf;
	char		buffer[1024];
	time_t		t;
	datum           key,data;

	/* We edit some fields of the source and 
	 * move the gdbm to its final location.
	 */
        dbf = gdbm_open((char *)source, 1024, GDBM_WRITER, 0644, 0);

	if (dbf == NULL) {
		errmsg = "create_gdbm - failed to open source file for writing";
		errcode = 9;
		return(-1);
	}

	key.dptr   = "lck";
	key.dsize  = sizeof(key.dptr) - 1;

	data.dptr  = "0";
	data.dsize = sizeof(data.dptr) - 1;

	if (gdbm_store(dbf, key, data, GDBM_REPLACE) != 0) {
		gdbm_close(dbf);
		errmsg = "create_gdbm - failed to update lck";
		errcode = 10;
		return(-1);
	}

	time(&t);
	strftime(buffer, 1024, "%Y-%m-%dT%H:%M:%SZ", gmtime(&t));

	key.dptr   = "crt";
	key.dsize  = sizeof(key.dptr) - 1;

	data.dptr  = buffer;
	data.dsize = strlen(buffer);

	if (gdbm_store(dbf,key,data,GDBM_REPLACE) != 0) {
		gdbm_close(dbf);
		errmsg = "create_gdbm - failed to update crt";
		errcode = 11;
		return(-1);
	}

	sprintf(buffer, "%d", t);

	key.dptr   = "mod";
	key.dsize  = sizeof(key.dptr) - 1;

	data.dptr  = buffer;
	data.dsize = strlen(buffer);

	if (gdbm_store(dbf,key,data,GDBM_REPLACE) != 0) {
		gdbm_close(dbf);
		errmsg = "create_gdbm - failed to update mod";
		errcode = 12;
		return(-1);
	}

	gdbm_close(dbf);

	if (rename(source, dest) == -1) {
		errmsg = "create_gdbm - rename source to dest";
		errcode = 13;
		return(-1);
	}

	return(0);
}
