/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: terminator.c,v 1.3 2006/12/19 08:33:17 hochstenbach Exp $
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

#define TERMINATOR_PROG		"terminator.cgi"
#define TERMINATOR_LOG		"terminator.log"
#define MD5SECRET		"dsf8fs8fs8f9sfs435435****1121212Kjg@3!!!';fksdf937xn8y1872"
#define MAX_PATH_LENGTH		1024
#define MAX_URL_LENGTH		1024
#define SEC_MAX_CONTENT_LENGTH 	1024

extern int 	errno;
const char  	*errmsg;

char	*getdbfile(const char *url);

char	*lockstatus(const char *key);
int	valid_url(const char *url);
int  	valid_env(void);
int	cmd_terminate(const char *url);
int	cmd_success(const char *fmt, ...);
int 	cmd_error(void);
int	process_request(void);
char	*substr(const char *str, int start, int length);
char	*secdigest(const char *key);

int	main(void) {
	char	filename[MAX_PATH_LENGTH];
	int 	ret;
	FILE 	*fp;

	/* Open the log file */
	if (getenv("GWSPOOL") != NULL) {
		sprintf(filename, "%s/%s", getenv("GWSPOOL"), TERMINATOR_LOG);
        	fp = fopen(filename,"a");
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
	const char *md5;
	const char *url;
	const char *init_url;
	const char *md5_test;
	char	   *dbfile;
	char	   *p;
	session_struct st;
	int	   domd5 = 1;

	init_url   = cgi_getentrystr("init_url");
	url        = cgi_getentrystr("url");
	md5        = cgi_getentrystr("md5");

	if (init_url == NULL) init_url = "";
	if (url == NULL) url = "";
	if (md5 == NULL) md5 = "";

	log("process_request - recieved url(%s) md5(%s)", url, md5);	

	/* If we only get an URL, then display the registry page */
	if (strlen(url) == 0) {
		cmd_terminate(init_url);
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
			cmd_success("Request registered, please click on <a href=\"%s?url=%s&md5=%s\">this</a> link to proceed.", TERMINATOR_PROG, url, md5_test);
			exit(0);
		}
		else if (strlen(md5) != 16*2 || strncmp(md5, md5_test, 16*2) != 0) {
			errmsg = "checksum not valid";
			return(-1);
		}
	}

	/* Find the associated database file */
	dbfile = getdbfile(url);

	if (dbfile == NULL) {
		errmsg = "static repository is not registered!";
		warn("process_request - tried to terminate a non existing url %s", url);
		return(-1);
	}

	/* Here is the TERMINATION hack. We append to the url the fragment '#TERMINATE'
	 * to indicate that we do something special.
	 */
	p = (char *) malloc(strlen(url) + strlen("#TERMINATE") + 1);

	if (p == NULL) {
		errmsg = "internal error";
		warn("process_request - malloc error");
		return(-1);
	}
						
	if (create_lock(dbfile, p,&st) != 0) {
		errmsg = "url is locked";
		warn("process_request - %s is locked", url);
		return(-1);
	}
	else {
		log("process_request - locked %s as %s", url, st.lockfile);
	}

	sprintf(p, "%s%s", url, "#TERMINATE");
	strcpy(st.url, p);

	write_lock(&st);

	close_lock(&st);				

	cmd_success("Request succeeded<p>Your termination request will be processed.");

	return(0);
}

/* This function checks if a database file exists for the
 * given url. Returns dbfile if a database file exists 
 * or NULL on error.
 */
char	*getdbfile(const char *url) {
	char       *gwdbconf  = getenv("GWDBCONF");
	char 	   *frag;
	char	   *database;
	int	   db_conf;
	char	   *p;

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
		return(NULL);
	}

	if ((db_conf = config_load(gwdbconf)) < 0) {
		warn("process_request - failed to open %s", gwdbconf);
		free(frag);
		return(NULL);
	}

	database = config_param(db_conf, frag);

	if (strlen(database) > 0) {
		/* Database registered in configuration file */

		p = (char *) malloc(strlen(database) + 1);

		if (p != NULL) 
			strcpy(p, database);
	}
	else {
		p = NULL;
	}

	config_free(db_conf);

	free(frag);

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

int	cmd_terminate(const char *url) {
	char	**parse_args;
	int 	n = 0;

	n += printf("Status: 200 - OK\r\n"); fflush(stdout);
        n += printf("Content-type: text/html\r\n\r\n"); fflush(stdout);

	parse_args = args_template(3, url, NULL);

	if (parse_args == NULL) {
		errmsg = "internal error";
		warn("process_request - failed to parse_args()");
		return(-1);
	}
	
	n += parse_template(stdout, getenv("GWTERMINATORHTML"), parse_args, PARSE_LOOSE);

	free(parse_args);

	return(n);
}

int	cmd_success(const char *fmt, ...) {
	va_list arg;
	time_t  tm;
	char    *p;
	char * const parse_args[] = { NULL };	
	int 	n = 0;

	time(&tm);
	p = ctime(&tm);
		
	n += printf("Status: 200 - OK\r\n"); fflush(stdout);
	n += printf("Content-type: text/html\r\n\r\n"); fflush(stdout);
	n += parse_template(stdout, getenv("GWHEADERHTML"), parse_args, PARSE_LOOSE);
	n += printf("<h1>Processing request</h1><p>[%s] ", p);

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
