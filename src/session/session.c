/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: session.c,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
 * 
 */
#include "session.h"
#include "md5.h"
#include "hash.h"

HASH	lock_hash;

/* Library initialization function */
void	_init(void) {
	lock_hash = hash_new();
}

/* Library finalization function */
void	_fini(void) {
	hash_free(lock_hash);
}

/* Given a string this function returns the name of a lock file
 * in the spool directory. The caller has to free the memory
 * allocated by this function. Returns NULL on error.
 */
char	*lock_name(const char *str, const char *prefix);

/* Given a string this function returns the name of a lock file
 * in the spool directory. The caller has to free the memory
 * allocated by this function. Returns NULL on error.
 */
char	*lock_name(const char *str, const char *prefix) {
	md5_state_t 	state;
	md5_byte_t 	digest[16];
	char 		hex_output[16*2 + 1]; 
	int		di;
	char		*lock_file;
	char		*spooldir;

	if (getenv("GWSPOOL") != NULL) {
		spooldir = getenv("GWSPOOL");
	}
	else {
		spooldir = SPOOL_DIRECTORY;
	}

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *) str, strlen(str));
	md5_finish(&state, digest);

	for (di = 0; di < 16; ++di)
            sprintf(hex_output + di * 2, "%02x", digest[di]);

	lock_file = (char *) malloc(sizeof(char)*(strlen(spooldir) + sizeof("/") + strlen(prefix) + strlen(hex_output) + 1));

	if (lock_file == NULL) 
		return(NULL);

	sprintf(lock_file, "%s/%s%s", spooldir, prefix, hex_output);

	return(lock_file);
}

/* This functions creates a lock file given a database path and a url.
 * The function returns 0 on success or -1 on error.
 * The function sets the value of st to the content of the lock file.
 */
int	create_lock(const char *dbfile, const char *url, session_struct *st) {
	FILE	*fp;
	int	fd,fd_dup;
	char	*lf;
	time_t 	tm;
	char	*world_access;

	lf = lock_name(dbfile, LOCK_PREFIX); 

	if (lf == NULL) {
		return(-1);
	}

	log("create_lock - creating lock file %s", lf);

	fd = open(lf, O_CREAT | O_EXCL | O_RDWR , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	
	world_access = getenv("GWWORLDACCESS");

	if (world_access != NULL && strcmp(world_access,"1") == 0) {
		if (fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1) {
			warn("create_lock - failed to fchmod()");
		}
		else {
			warn("create_lock - creating world writable lock");	
		}
	}
	
	if (fd == -1) {
		if (errno == EEXIST) {
			log("create_lock - failed : lock file exists");
		}
		else {
			log("create_lock - failed : %s", strerror(errno));
		}
		free(lf);
		return(-1);
	}

	/* We need to dup the file descriptor which we can pass to the fdopen.
	 * This is needed because we need to call a fclose() to free memory
	 * allocated by fdopen.
	 */
	fd_dup = dup(fd);

	if (fd_dup == -1) {
		warn("create_lock - dup() failed");
		close(fd);
		free(lf);
		return(-1);
	}

	fp = fdopen(fd_dup,"w");

	if (fp == NULL) {
		warn("create_lock - fdopen() failed");
		close(fd);
		close(fd_dup);
		free(lf);
		return(-1);
	}

	fprintf(fp, "database=%s\n", dbfile);
	fprintf(fp, "url=%s\n", url);
	fprintf(fp, "status=%d\n" , SESSION_REQUEST);

	fflush(fp);

	strncpy(st->lockfile,lf,MAX_PATH_LENGTH);
	st->lockfile[MAX_PATH_LENGTH - 1] = '\0';

	strncpy(st->database,dbfile,MAX_PATH_LENGTH);
	st->database[MAX_PATH_LENGTH - 1] = '\0';

	strncpy(st->url,url,MAX_URL_LENGTH);
	st->url[MAX_URL_LENGTH - 1] = '\0';

	st->status = SESSION_REQUEST;
	st->fd = fd;

	time(&tm);
	st->modified = tm;

	free(lf);
	fclose(fp);

	return(0);
}

/* This function tries to find out of a database is locked. Returns 1
 * when a lock file was found, return 0 when no file was found, returns
 * -1 on error.
 */
int	is_locked(const char *dbfile) {
	int	fd;
	char	*lf;

	lf = lock_name(dbfile, LOCK_PREFIX);

	if (lf == NULL) {
		return(-1);
	}

	fd = open(lf, O_RDONLY , 0600);

	free(lf);

	if (fd == -1) {
		close(fd);
		return(0);
	}
	else {
		close(fd);
		return(1);
	}
}

/* This function deletes a lock file for a database from the file system. 
 * Returns 0 on success or -1 on error.
 */
int	release_lock(session_struct *st) {

	hash_del(lock_hash,st->lockfile);

	unlink(st->lockfile);

	close(st->fd);

	return(0);
}

/* This function closes a lock file but doesn't delete it from the
 * file system.
 * Returns 0 on success or -1 on error.
 */
int	close_lock(session_struct *st) {

	hash_del(lock_hash,st->lockfile);

	close(st->fd);

	return(0);
}

/* This function scans the SPOOL directory for lock files. If
 * a lock file is found it wil be opened and the content will
 * be copied to session_struct. This session_struct should be
 * used for subsequent calls to this lockfile.
 * Set flag to 
 * Returns 0 on success or -1 on error.
 */ 
int	next_lock(session_struct *st, enum session_locktype flag) {
	DIR 		*dp;
	struct dirent	*dirp;
	int		fd;
	char		pathname[MAX_PATH_LENGTH];
	char		*spooldir;

	if (getenv("GWSPOOL") != NULL) {
		spooldir = getenv("GWSPOOL");
	}
	else {
		spooldir = SPOOL_DIRECTORY;
	}

	debug("next_lock - scanning spool dir %s", spooldir);

	if ( (dp = opendir(spooldir)) == NULL )  {
		warn("next_lock - failed to open spool dir %s", spooldir);
		return(-1);
	}

	/* Read the directory */
	while ( (dirp = readdir(dp)) != NULL ) {
		/* Test if we have a lock file */
		if (memcmp(dirp->d_name,"lock",4) != 0) 
			continue;

		/* Try to optain a lock for it */
		sprintf(pathname, "%s/%s", spooldir, dirp->d_name);
		
		fd = open(pathname, O_RDWR, S_IRUSR | S_IWUSR);

		if (fd == -1) {
			if (errno == EACCES) {
				warn("next_lock - (%d;%d) doesn't have rw access to %s"
						, geteuid()
						, getegid()
						, pathname);
			}
			else {
				warn("next_lock - %s not accessible : %s" , pathname, strerror(errno));
			}
			/* The lock file is not accessible */
			continue;
		}

		if (hash_val(lock_hash,pathname) != NULL) {
			/* The file is locked by another process */
			debug("next_lock - skipping %s it is locked by other process", pathname);
			close(fd);
			continue;
		}

		hash_add(lock_hash,pathname,"locked");

		/* Set the file descriptor for this lock file */
		st->fd = fd;
		
		/* Copy the pathname to this lock file */
		memcpy(st->lockfile,pathname,MAX_PATH_LENGTH);

		/* Read the content of this lock file */
		if (read_lock(st) != 0) {
			debug("next_lock - skipping %s can't read lock", pathname);
			close(fd);
			continue;
		}

		/* Check if this the status of this lock is 0 */
		if (flag == SESSION_UNPROCESSED_TYPES &&  st->status != SESSION_REQUEST) { \
			debug("next_lock - skipping %s it has been processed in the past : status = %d", pathname, st->status);
			close(fd);
			continue;
		}

		closedir(dp);
		return(0);
	}

	closedir(dp);

	return(-1);
}

/* This function opens a lock file for reading, given a database.
 * The session_struct will be populated. Returns 0 on success, -1
 * on error.
 */
int	open_lock(const char* dbfile, session_struct *st) {
	char 	*lf;
	int 	ret;

	lf = lock_name(dbfile, LOCK_PREFIX); 

	ret = fopen_lock(lf, st);

	free(lf);

	return ret;
}

/* This function opens a lock file for reading, given a filename.
 * The session_struct will be populated. Returns 0 on success, -1
 * on error.
 */
int	fopen_lock(const char* filename, session_struct *st) {
	int 	fd;

	fd = open(filename, O_RDONLY);

	if (fd == -1) {
		if (errno == EACCES) {
			warn("open_lock - (%d;%d) doesn't have r access to %s"
				, geteuid()
				, getegid()
				, filename);
			}
		else {
			warn("open_lock - %s not accessible : %s" , filename, strerror(errno));
		}

		/* The lock file is not accessible */
		return(-1);
	}

	strncpy(st->lockfile, filename, MAX_PATH_LENGTH);
	st->lockfile[MAX_PATH_LENGTH - 1] = '\0';

	st->fd = fd;

	return(0);
}

/* This function reads the lock file and populates a session_struct 
 * with its data.
 * Returns 0 on success or -1 on error.
 */
int 	read_lock(session_struct *st) {
	FILE		*fp;
	int		fd_dup;
	char		buffer[MAX_LINE_LENGTH];
	struct	stat	sbuf;

	/* We need to dup the file descriptor which we can pass to the fdopen.
	 * This is needed because we need to call a fclose() to free memory
	 * allocated by fdopen.
	 */
	fd_dup = dup(st->fd);

	if (fd_dup == -1) {
		warn("read_lock - failed to dup() : %s", strerror(errno));
		return(-1);
	}

	/* Create a file stream for reading and writing */
	fp = fdopen(fd_dup, "r");

	if (fp == NULL) {
		warn("read_lock - failed to fdopen() : %s", strerror(errno));
		close(fd_dup);
		return(-1);
	}

	/* Rewind the stream */
	rewind(fp);
	
	while ( fgets(buffer, MAX_LINE_LENGTH, fp) != NULL ) {
		debug("read_lock - reading %s",buffer);
		if (memcmp(buffer,"database=",9) == 0) 
			sscanf(buffer,"database=%1024s\n",st->database);
		else if (memcmp(buffer,"url=",4) == 0)
			sscanf(buffer,"url=%1024s\n",st->url);
		else if (memcmp(buffer,"status=",7) == 0)
			sscanf(buffer,"status=%d\n",&(st->status));
	}

	/* Read the modification time */
	if (fstat(st->fd,&sbuf) != 0) {
		warn("read_lock - faled t fstat() : %s", strerror(errno));
		fclose(fp);
		return(-1);
	}

	st->modified = sbuf.st_mtime;
		
	fclose(fp);

	return(0);
}

/* This function write the content of the session_struct to the lock
 * file.
 * Returns 0 on success or -1 on error.
 */
int	write_lock(session_struct *st) {
	FILE 	*fp;
	int	fd_dup;

	/* We need to dup the file descriptor which we can pass to the fdopen.
	 * This is needed because we need to call a fclose() to free memory
	 * allocated by fdopen.
	 */
	fd_dup = dup(st->fd);

	if (fd_dup == -1) {
		warn("write_lock - failed to dup() : %s", strerror(errno));
		return(-1);
	}
	
	/* Create a file stream for reading and writing */
	fp = fdopen(fd_dup,"w");

	if (fp == NULL) {
		warn("write_lock - failed to fdopen() : %s", strerror(errno));
		close(fd_dup);
		return(-1);
	}

	/* Rewind the stream */
	rewind(fp);

	/* First truncate the file to size zero */
	if (ftruncate(st->fd,0) == -1) {
		warn("write_lock - failed to truncate lock");
		fclose(fp);
		return(-1);
	}

	log("write_lock - updating lockfile %s", st->lockfile);
	fprintf(fp, "database=%s\n", st->database);
	fprintf(fp, "url=%s\n", st->url);
	fprintf(fp, "status=%d\n" , st->status);
	fflush(fp);

	/* Update session_struct */
	
	if (read_lock(st) == -1) {
		warn("write_lock - failed to read_lock()");
		fclose(fp);
		return(-1);
	}

	fclose(fp);

	return(0);
}

