/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: session.h,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
 * 
 */
#ifndef __session_h
#define __session_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <debug.h>

#define SPOOL_DIRECTORY		"../spool"
#define	LOCK_PREFIX		"lock"
#define	MAX_PATH_LENGTH		1024	
#define	MAX_URL_LENGTH		1024
#define	MAX_LINE_LENGTH		2048

enum	session_status {
	SESSION_REQUEST = 0  ,			/* Lock file created for this database */
	SESSION_CONTACTING   , 			/* A srepod daemon is connecting to the url */
	SESSION_VALIDATING   , 			/* A srepod daemon is validating the data */
	SESSION_WRITING      ,			/* A srepod daemon is writing a new database file */
	SESSION_INVALID      , 			/* A srepod daemon found the data not valid */
	SESSION_NO_RESOURCES_LEFT , 		/* A srepod daemon found not enough resources to store cache*/
	SESSION_CONFIG_ERROR 			/* Some configuration error prevents the daemon to create a cache file */
};

enum	session_locktype {
	SESSION_ALL_TYPES ,
	SESSION_UNPROCESSED_TYPES
};

typedef struct {
	char	lockfile[MAX_PATH_LENGTH];
	char	database[MAX_PATH_LENGTH];
	char	url[MAX_URL_LENGTH];
	int	status;
	int	fd;
	time_t	modified;
} session_struct;

/* Given a string this function returns the name of a lock file
 * in the spool directory. The caller has to free the memory
 * allocated by this function. Returns NULL on error.
 */
char	*lock_name(const char *str, const char *prefix);

/* This functions creates a lock file given a database path and a url.
 * The function returns 0 on success or -1 on error.
 * The function sets the value of st to the content of the lock file.
 */
int	create_lock(const char *dbfile, const char *url, session_struct *st);

/* This function tries to find out of a database is locked. Returns 1
 * when a lock file was found, return 0 when no file was found, returns
 * -1 on error.
 */
int	is_locked(const char *dbfile);

/* This function deletes a lock file for a database from the system. 
 * Returns 0 on success or -1 on error.
 */
int	release_lock(session_struct *st);

/* This function closes a lock file but doesn't delete it from the
 * file system.
 * Returns 0 on success or -1 on error.
 */
int	close_lock(session_struct *st); 

/* This function scans the SPOOL directory for lock files. If
 * a lock file is found it wil be opened and the content will
 * be copied to session_struct. This session_struct should be
 * used for subsequent calls to this lockfile.
 * Returns 0 on success or -1 on error.
 */
int	next_lock(session_struct *st, enum session_locktype flag);

/* This function opens a lock file for reading, given a database.
 * The session_struct will be populated. Returns 0 on success, -1
 * on error.
 */
int	open_lock(const char* dbfile, session_struct *st);

/* This function opens a lock file for reading, given a filename.
 * The session_struct will be populated. Returns 0 on success, -1
 * on error.
 */
int	fopen_lock(const char* filename, session_struct *st);


/* This function reads the lock file and populates a session_struct 
 * with its data.
 * Returns 0 on success or -1 on error.
 */
int	read_lock(session_struct *st);

/* This function write the content of the session_struct to the lock
 * file.
 * Returns 0 on success or -1 on error.
 */
int	write_lock(session_struct *st);


#endif
