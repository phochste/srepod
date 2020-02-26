/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: registry.c,v 1.6 2006/12/19 08:33:17 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <cgi-util.h>
#include <debug.h>
#include <cconfig.h>
#include <session.h>
#include <template.h>
#include "md5.h"

#define REGISTRY_PROG		"registry.cgi"
#define REGISTRY_LOG		"registry.log"
#define MD5SECRET		"uuled1782HHGWH762ghgsqhdq5762109JkhdcuhiJGSHYGSSVBVHg12198192jhfgkdf"
#define MAX_PATH_LENGTH		1024
#define MAX_URL_LENGTH		1024
#define SEC_MAX_CONTENT_LENGTH 	1024
#define TMPDBPREFIX		"tmpdbf"
#define	TMPLOCKPOSTFIX		"#registry"
#define NUMSUBDIR		32.0

#define REFRESH_HEAD		"<head><meta http-equiv=\"Refresh\" Content=\"10;\"></head>"

extern int 	errno;
const char  	*errmsg;

int	dorefresh = 0;

char 	*dbpath(const char *key);
int	dbexists(const char *url);
char	*lockstatus(const char *key);
int	is_file(const char *filename);
int	valid_url(const char *url);
int  	valid_env(void);
int	cmd_registry(const char *url, const char *email);
int	cmd_success(const char *fmt, ...);
int 	cmd_error(void);
int	process_request(void);
int	register_database(const char *url, const char *dbfile, char *gwurl);
char	*substr(const char *str, int start, int length);
char	*secdigest(const char *key);

int	main(void) {
	char	gwconf[MAX_PATH_LENGTH];
	int 	ret;
	FILE 	*fp;

	/* Open the log file */
	if (getenv("GWSPOOL") != NULL) {
		sprintf(gwconf, "%s/%s", getenv("GWSPOOL"), REGISTRY_LOG);
        	fp = fopen(gwconf,"a");
        	set_log_stream(fp);
	}

	debug("main - checking CGI environment");
	if (! valid_env()) {
                warn("main - CGI environment insecure!");
                (void) cmd_error();
                exit(1);
        }

	/* Parse the CGI query */
        ret = cgi_init();

	if (ret != CGIERR_NONE) {
		warn("main - recieved bad request from %s", getenv("REMOTE_ADDR"));
		errmsg  = "Bad request";
		(void) cmd_error();
		exit(1);
	}
	else {
		log("main - requery recieved from %s", getenv("REMOTE_ADDR"));
	}

	if ( process_request() != 0 ) {
		warn("main - process_request failed");
		(void) cmd_error();
		exit(1);
	}

	return(0);
}

int	process_request(void) {
	const char *url;
	const char *email;
	const char *md5;
	const char *init_url;
	const char *init_email;
	const char *md5_test;
	char	   *dbfile;
	char	   *status;
	int	   registered;
	int	   domd5 = 1;
	time_t	   tm;
	char	   *p;
	char	   gwurl[MAX_URL_LENGTH];
	char	   lockcopy[MAX_URL_LENGTH];
	session_struct st;

	init_url   = cgi_getentrystr("init_url");
	init_email = cgi_getentrystr("init_email");
	url        = cgi_getentrystr("url");
	email      = cgi_getentrystr("email");
	md5        = cgi_getentrystr("md5");


	if (init_url == NULL) init_url = "";
	if (init_email == NULL) init_email = "";
	if (url == NULL) url = "";
	if (email == NULL) email = "";
	if (md5 == NULL) md5 = "";

	log("process_request - recieved url(%s) email(%s) md5(%s)", url, email, md5);	

	/* If we only get an URL, then display the registry page */
	if (strlen(init_url) > 0) {
		cmd_registry(init_url, init_email);
		return(0);
	}

	if ( ! valid_url(url) ) {
		errmsg = "invalid url";
		warn("process_request - url not valid");
		return(-1);
	}

	/* Check if we need md5 secured tests */
	if (getenv("GWMD5TEST") != NULL && strcmp(getenv("GWMD5TEST"),"0") == 0) {
		domd5 = 0;
	}

	if (getenv("GWCONFIRM") != NULL && strcmp(getenv("GWCONFIRM"),"0") == 0) {
		domd5 = 0;
	}

	if (domd5) {
		/* We only accept valid md5 secured requests */
		md5_test = secdigest(url);

		if (strlen(md5) == 0) {
			cmd_success("Request registered, please click on <a href=\"%s?url=%s&email=%s&md5=%s\">this</a> link to proceed.", REGISTRY_PROG, url, email, md5_test);
			exit(0);
		}
		else if (strlen(md5) != 16*2 || strncmp(md5, md5_test, 16*2) != 0) {
			errmsg = "checksum not valid";
			return(-1);
		}
	}

	/* Check if the url is already registered */
	registered = dbexists(url);

	if (registered == -1) {
		errmsg = "configuration error";
		warn("process_request - failed to dbexists for %s", url);
		return(-1);
	}
	else if (registered == 1) {
		errmsg = "static repository already registered!";
		warn("process_request - tried to register an existsing url %s", url);
		return(-1);
	}

	/* The dbfile is a temporary file in the spool directory which
	 * contains the gdbm file created by the srepod daemon. 
	 */
	dbfile = lock_name(url, TMPDBPREFIX);

	if (dbfile == NULL) {
		errmsg = "configuration error";
		warn("process_request - failed to create a temp database path for %s", url);
		return(-1);
	}

	sprintf(lockcopy, "%s%s", dbfile, TMPLOCKPOSTFIX);

	/* If there exists a lock file for this dbfile, then the srepod 
	 * daemon is processing the request
	 */
	if (is_locked(dbfile)) {
		status = lockstatus(dbfile);
	
		log("process_request - %s is locked status : %s", url, status);

		if (status == NULL) {
			cmd_success("Failed to process request");
		}
		else {
			cmd_success("Processing request : %s", status);
		}

		/* Remove the lock file copy.
		 */
		if (dorefresh == 0 && unlink(lockcopy) == -1) {
		 	warn("process_request - failed to unlink %s", lockcopy);
		}

		exit(0);
	}

	/* If the dbfile exists in the spool directory, then the gdbm file has
	 * succesfully created the gdbm file
	 */
	if (is_file(dbfile)) {
		log("process_request - daemon left gdbm file %s in spool", dbfile);

		if (register_database(url, dbfile, gwurl) == 0) {
			log("process_request - register_database() succeeded");
			cmd_success("Request succeeded<p>Your repository is available at: <a href=\"%s\">%s</a>", gwurl, gwurl);
		}
		else {
			log("process_request - register_database() failed ");
			errmsg = "Failed to register the database";
			cmd_error();			
		}

		/* Remove the lock file copy.
		 */
		if (unlink(lockcopy) == -1) {
		 	warn("process_request - failed to unlink %s", lockcopy);
		}

		exit(0);
	}

	/* The lockcopy is a hard link to a lock file (see below).
	 * We read the status of the linked lock file and send an error 
	 * message to the users. In normal situation the hard linked
	 * copy should only exist if the daemon found an error when
	 * processing the request.
	 */
	if (is_file(lockcopy)) {
		log("process_request - registry left lock copy %s in spool", lockcopy);

		/* HACK: we can only read the status of a lock file if 
		 * it was created using create_lock with the url as key.
		 * We trick the system by renaming the lockcopy back to
		 * its old name using the lock_name() function.
		 */ 
		
		 p = lock_name(dbfile, LOCK_PREFIX);

		 if (p == NULL) {
			 errmsg = "configuration error";
			 warn("process_request - lock_name() failed");
			 return(-1);
		 }

		 if (rename(lockcopy, p) == -1) {
			 errmsg = "configuration error";
			 warn("process_request - rename() failed");
			 return(-1);
		 }

		 /* Now the lockcopy is renamed to the old lock file, we can
		  * read its status with lockstatus
		  */
		 status = lockstatus(dbfile);
	
		 log("process_request - %s is locked status : %s", url, status);

		 if (status == NULL) {
			cmd_success("Failed to process request");
		 }
		 else {
			cmd_success("Processing request : %s", status);
		 }

		 /* Remove the lock file, so that a user can re-register a bad static
		  * repository
		  */
		 if (dorefresh == 0 && unlink(p) == -1) {
			 warn("process_request - failed to unlink %s", p);
		 }

		 free(p);

		 exit(0);
	}
		
	/* Try to create a lock file to process the request.
	 *
	 * Warning: environment parameter GWSPOOL should be set
	 * to the spool directory.
	 */
	if (getenv("GWSPOOL") == NULL) {
		warn("process_request - GWSPOOL is not set");
	}

	if (create_lock(dbfile,url,&st) != 0) {
		errmsg = "url is locked";
		warn("process_request - %s is locked", url);
		return(-1);
	}
	else {
		log("process_request - locked %s as %s", url, st.lockfile);
	}

	/* The daemon process will delete the lock file when it its
	 * finished processing the request. We create a hard link to
	 * the lock file to keep a safe copy of the lock file
	 */
	if (link(st.lockfile, lockcopy) == -1) {
		errmsg = "configuration error";
		warn("process_request - failed to link()");
		return(-1);
	}

	close_lock(&st);				

	dorefresh = 1;

	cmd_success("Processing request : please wait ...");

	return(0);
}

/* This function moves temporary database file to its final destination
 * and register the database in the gateway configuration file and
 * friends configuration file. The value of gwurl is set to the location
 * of the new repository.
 * Returns 0 on success, -1 on error.
 */
int	register_database(const char *url, const char *dbfile, char *gwurl) {
	struct	stat	buf;
	char	**parse_args;
	char	*gwbaseurl   = getenv("GWBASEURL");
	char    *gwdbconf    = getenv("GWDBCONF");
	char	*gwdbfriends = getenv("GWDBFRIENDS");
	char	*db;
	char	*frag;
	char	*xml;
	char	tmpfile[MAX_PATH_LENGTH];
	int	config;
	int	fd;
	FILE	*fp;

	if (gwbaseurl == NULL) {
		warn("process_request - GWBASEURL is not set");
		return(-1);
	}

	if (gwdbconf == NULL) {
		warn("process_request - GWDBCONF is not set");
		return(-1);
	}

	/* Strip everything after the http scheme from the url and
	 * return this string as fragment.
	 */
	frag = substr(url,strlen("http:/"),strlen(url));

	/* Set gwurl */
	sprintf(gwurl, "%s%s", gwbaseurl, frag);

	config = config_open(gwdbconf, READ_WRITE);

	if (config == -1) {
		warn("register_database - failed to open %s for appending : %s", gwdbconf, strerror(errno));
		free(frag);
		return(-1);
	}

	db = dbpath(dbfile);

	if (db == NULL) {
		warn("register_database - failed to dbpath()");
		free(frag);
		config_free(config);
		return(-1);
	}

	if (rename(dbfile, db) == -1) {
		warn("register_database - failed to rename %s to %s : %s", dbfile, db, strerror(errno));
		free(frag);
		config_free(config);
		return(-1);
	}


	config_param_add(config, frag, db);
	config_save(config);
	config_free(config);

	log("register_database - registered %s/%s", frag, db);

	/* Only update the friends when the environmental variable is defined */
	if (gwdbfriends == NULL) {
		return(0);
	}

	sprintf(tmpfile,"%s/friends.tmp", getenv("GWSPOOL"));

	fd = open(tmpfile, O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if (fd == -1) {
		warn("register_database - failed to open %s", tmpfile);
		return(-1);
	}

#ifdef __svr4__	
	if (lockf(fd, F_LOCK, 0) == -1) {
#else	
	if (flock(fd, LOCK_EX) == -1) {
#endif		
		warn("register_database - failed to lock %s : %s", tmpfile, strerror(errno));
		free(frag);
		return(-1);
	}


	fp = fdopen(fd,"w");

	if (fp == NULL) {
		warn("register_database - failed to fdopen()");
		free(frag);
		close(fd);
		return(-1);
	}

	xml = (char *) malloc(sizeof(char)*(strlen("<baseUrl>") + MAX_URL_LENGTH + strlen("</baseUrl>") + 1));

	if (xml == NULL) {
		warn("register_database - memory allocation error");
		free(frag);
		fclose(fp);
		return(-1);
	}

	sprintf(xml,"<baseURL>%s%s</baseURL>", gwbaseurl, frag);

	parse_args = args_template(4, xml, "$0\n$1", NULL);

	if (parse_args == NULL) {
		warn("register_database - failed to parse_args()");
		free(frag);
		free(xml);
		fclose(fp);
		return(-1);
	}
	
	if (parse_template(fp, gwdbfriends, parse_args, PARSE_LOOSE) == -1) {
		warn("register_database - failed to parse_template()");
		free(frag);
		free(parse_args);
		free(xml);
		fclose(fp);
		return(-1);
	}

	if (stat(gwdbfriends, &buf) == -1) {
		warn("register_database - failed to stat %s: %s", gwdbfriends, strerror(errno));
		free(frag);
		free(parse_args);
		free(xml);
		fclose(fp);
		return(-1);
	}

	if (rename(tmpfile, gwdbfriends) == -1) {
		warn("register_database - failed to rename %s to %s: %s", tmpfile, gwdbfriends, strerror(errno));
		free(frag);
		free(parse_args);
		free(xml);
		fclose(fp);
		return(-1);
	}

	chmod(gwdbfriends, buf.st_mode);

	log("register_database - added %s/%s to friends", gwbaseurl, frag);

	fclose(fp);

	free(frag);
	free(parse_args);
	free(xml);

	return(0);
}


/* This function returns 1 of the filename exists on the system,
 * return 0 otherwise
 */
int	is_file(const char *filename) {
	struct stat buf;

	/* Test if the filename exists, otherwise create it */
	if (stat(filename,&buf) != 0) {
		return(0);
	}

	return(1);
}

/* This function returns the status of a lockfile given
 * the name of a lock key (a string that was used to create
 * the lock file with the create_lock command).
 *
 * Returns a pointer to the status message or NULL on error.
 *
 * This function sets the dorefresh global variable. Based on
 * this variable the client needs to reconnect to this program
 * to wait for a valid status respons.
 */
char	*lockstatus(const char *key) {
	session_struct st;

	if (open_lock(key, &st) == -1) {
		warn("lockstatus - failed to open_lock");
		return(NULL);
	}

	if (read_lock(&st) == -1) {
		warn("lockstatus - failed to read_lock");
		return(NULL);
	}

	switch(st.status) {
		case SESSION_REQUEST :
			dorefresh = 1;
			return("request is pending...please wait...");
		case SESSION_CONTACTING :
			dorefresh = 1;
			return("contacting server...please wait...");
		case SESSION_VALIDATING :
			dorefresh = 1;
			return("validating reponse...please wait...");
		case SESSION_WRITING :
			dorefresh = 1;
			return("writing repository...please wait...");
		case SESSION_INVALID :
			dorefresh = 0;
			return("invalid repository - request failed");
		case SESSION_NO_RESOURCES_LEFT :	
			dorefresh = 0;
			return("gateway doesn't have any resources left");
		case SESSION_CONFIG_ERROR :	
			dorefresh = 0;
			return("gateway has a configuration error");
		default:
			dorefresh = 0;
			return(NULL);
	}

	return(NULL);
}

/* This function checks if a database file exists for the
 * given url. Returns 1 if a database file exists or 0
 * when not or -1 on error.
 */
int	dbexists(const char *url) {
	char       *gwdbconf  = getenv("GWDBCONF");
	char 	   *frag;
	char	   *database;
	int	   db_conf;
	int	   registered = 0;

	/* Strip everything after the http scheme from the url and
	 * return this string as fragment.
	 */
	frag = substr(url,strlen("http:/"),strlen(url));

	/* Check if we don't have this fragment already in the 
	 * gateway.conf file
	 */
	if (gwdbconf == NULL) {
		warn("process_request - GWDBCONF is not set");
		free(frag);
		return(-1);
	}

	if ((db_conf = config_load(gwdbconf)) < 0) {
		warn("process_request - failed to open %s", gwdbconf);
		free(frag);
		return(-1);
	}

	database = config_param(db_conf, frag);

	if (strlen(database) > 0) {
		/* Database registered in configuration file */
		registered = 1;
	}
	else {
		registered = 0;
	}

	config_free(db_conf);

	free(frag);

	return(registered);
}

/* Create, based on the key in the argument, a pathname on
 * disk which contain the database file. Returns a pointer
 * to the pathname or NULL on error.
 */
char	*dbpath(const char* key) {
	char	*md5;
	struct stat buf;
	int 	r;
	time_t	tm;
	char	dirname[2];
	char 	*base_path;
	char	*p;

	base_path = getenv("GWDBPATH");

	if (base_path == NULL) {
		warn("dbpath - error GWDBPATH not set");
	}

	if (base_path[0] != '/') {
		warn("dbpath - error GWDBPATH is not absolute!");
		return(NULL);
	}

	/* Select a directory randomly to store the database */
	time(&tm);
	srand((unsigned int)tm);
	r = 1+(int)(NUMSUBDIR*rand()/(RAND_MAX+1.0));

	sprintf(dirname,"%.2x",r);

	debug("dbpath - dirname : %s", dirname);
	
	p = (char *) malloc(sizeof(char) * MAX_PATH_LENGTH);

	if (p == NULL) {
		return(NULL);
	}

	sprintf(p, "%s/%s", base_path, dirname);

	/* Test if the new directory exists, otherwise create it */
	if (stat(p,&buf) != 0) {
		if (errno == ENOENT) {
			if (mkdir(p,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1) {
				warn("dbpath - failed to create %s : %s", p, strerror(errno));
				return(NULL);
			}
			else {
				log("dbpath - created directory %s", p);
			}
		}
		else {
			warn("dbpath - failed to access directory %s : %s", p, strerror(errno));
			return(NULL);
		}
	}

	md5 = secdigest(key);

	if (md5 == NULL) {
		warn("dbpath - failed to secdigest()");
		return(NULL);
	}

	sprintf(p, "%s/%s/%s.dbf", base_path, dirname, md5);
	
	debug("dbpath - suggested path : %s", p);

	free(md5);

	return(p);
}

char	*substr(const char *str, int start, int l) {
	int  i;
	char *p;

	p = (char *) malloc(sizeof(char *) * l + 1);

	if (p == NULL) {
		warn("substr - memory allocation error");
		return(NULL);
	}

	for (i = start ; i - start < l || str[i] == '\0' ; i++) {
		p[i-start] = str[i];
	}

	p[i-start] = '\0';

	return p;
}

int	cmd_registry(const char *url, const char *email) {
	char	**parse_args;
	int 	n = 0;

	n += printf("Status: 200 - OK\r\n"); fflush(stdout);
        n += printf("Content-type: text/html\r\n\r\n"); fflush(stdout);

	parse_args = args_template(4, url, email, NULL);

	if (parse_args == NULL) {
		errmsg = "internal error";
		warn("process_request - failed to parse_args()");
		return(-1);
	}
	
	n += parse_template(stdout, getenv("GWREGISTRYHTML"), parse_args, PARSE_LOOSE);

	free(parse_args);

	return(n);
}

int	cmd_success(const char *fmt, ...) {
	va_list arg;
	time_t  tm;
	char    *p;
	char * const parse_args[] = { NULL };	
	char * const parse_args_refresh[] = { REFRESH_HEAD, NULL };	
	int 	n = 0;

	time(&tm);
	p = ctime(&tm);
		
	n += printf("Status: 200 - OK\r\n"); fflush(stdout);
	n += printf("Content-type: text/html\r\n\r\n"); fflush(stdout);

	if (dorefresh) {
		n += parse_template(stdout, getenv("GWHEADERHTML"), parse_args_refresh, PARSE_LOOSE);
		n += printf("<h1>Processing request</h1><p>This page will refresh in 10 seconds</p><p>[%s] ", p);
	}
	else {
		n += parse_template(stdout, getenv("GWHEADERHTML"), parse_args, PARSE_LOOSE);
		n += printf("<h1>Processing request</h1><p>[%s] ", p);
	}

	va_start(arg, fmt);
	n += vfprintf(stdout, fmt, arg);
	va_end(arg);

	n += parse_template(stdout, getenv("GWFOOTERHTML"), parse_args, PARSE_LOOSE);

	fflush(stdout);

	return(n);
}

int     cmd_error(void) {
	int 	n = 0;
	time_t 	tm;
	char 	*p;
	char * const parse_args[] = { NULL };	

	time(&tm);
	p = ctime(&tm);
	
	n += printf("Status: 200 - OK\r\n"); fflush(stdout);
        n += printf("Content-type: text/html\r\n\r\n"); fflush(stdout);

	n += parse_template(stdout, getenv("GWHEADERHTML"), parse_args, PARSE_LOOSE);

        n += printf("<h1>Error</h1><p>[%s] Failed to process request - reason : %s", p, errmsg); fflush(stdout);

	n += parse_template(stdout, getenv("GWFOOTERHTML"), parse_args, PARSE_LOOSE);

        return(n);
}

/* This subroutine returns 1 if the url given as argument looks
 * like a valid url. Returns 0 otherwise.
 */
int	valid_url(const char *url) {
	char  	 *p;
	int 	 valid;

	/* Check if we have a http scheme ... */
	if (strncmp(url,"http://",7) != 0) {
		debug("valid_url - invalid url : no http:// scheme");
		return(0);
	}

	/* Check if the url end with .xml */
	p = substr(url,strlen(url)-4,4);

	if (strncmp(p,".xml",4) != 0) {
		debug("valid_url - invalid url : no .xml postfix");
		free(p);
		return(0);
	}

	free(p);

	/* We allow only chars: [a-z] [A-Z] [0-9] / + - % . : _ ~ */
	valid = 1;
	p = (char *)url;
	while (*p) {
		if ( 
			(*p >= 48 && *p <= 57)  /* 0-9 */
				  ||
			(*p >= 65 && *p <= 90)  /* A-Z */
			          ||
			(*p >= 97 && *p <= 122) /* a-z */
			          ||
			 *p == 37               /* % */	  
			          ||
			 *p == 43               /* + */
			          ||
			 *p == 45               /* - */
			          ||
			 *p == 46               /* . */
			          ||
			 *p == 47               /* / */
			          ||
			 *p == 58               /* : */	  
				  ||
			 *p == 95               /* _ */
			          ||
			 *p == 126              /* ~ */
		) {
			p++;
		}
		else {
			debug("valid_url - url contains invalid char : ascii(%d)", *p);
			valid = 0;
			break;
		}
	}

	if ( ! valid ) {
		return(0);
	}

	return(1);
}

/* This subroutine checks some environment variables for security
 * reasons. Returns 1 if everything is okay, returns 0 on error.
 */
int	valid_env(void) {
	char	*p;

	p = getenv("REQUEST_METHOD");

	errmsg  = "Bad request";

	if (p == NULL) return 0;

	if (strlen(p) > 4) return 0;
	
	if (strcmp(p,"GET") == 0) {
		p = getenv("QUERY_STRING");

		if (p == NULL) return 0;

		if (strlen(p) > SEC_MAX_CONTENT_LENGTH) return 0;
	}
	else if (strcmp(p,"POST") == 0)	 {
		p = getenv ("CONTENT_LENGTH");

		if (p == NULL) return 0;
		
		if (strlen(p) > SEC_MAX_CONTENT_LENGTH) return 0;
	}
	else {
		return 0;
	}

	p = getenv("PATH_INFO");

	if (p != NULL) {
		if (strlen(p) > SEC_MAX_CONTENT_LENGTH) return 0;

//		if (sscanf(p,"%[^0-9A-Za-z%~.]*") > 0) return 0;
	}

	return 1;
}


/* Returns a digest of the given key, return NULL on error.
 * Caller has to free the digest.
 */
char	*secdigest(const char *key) {
	char		*remote_addr = getenv("REMOTE_ADDR");
	md5_state_t 	state;
	md5_byte_t 	digest[16];
	char		*sec_key;
	char 		*hex_output; 
	int		di;

	/* Protect the program against segmentation faults */
	if (remote_addr == NULL) {
		remote_addr = "127.0.0.1";
	}

	hex_output = (char *) malloc(sizeof(char)*16*2 + 1);

	if (hex_output == NULL) {
		warn("secdigest - memory allocation error");
		return(NULL);
	}

	sec_key =(char *) malloc(sizeof(char)*(strlen(key) + strlen(MD5SECRET) + strlen(remote_addr)) + 1);

	if (sec_key == NULL) {
		warn("secdigest - memory allocation error");
		free(hex_output);
		return(NULL);
	}

	sprintf(sec_key, "%s%s%s", MD5SECRET, remote_addr, key);

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *) sec_key, strlen(sec_key));
	md5_finish(&state, digest);

	for (di = 0; di < 16; ++di)
            sprintf(hex_output + di * 2, "%02x", digest[di]);

	free(sec_key);

	return(hex_output);
}
