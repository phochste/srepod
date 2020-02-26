/*
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: unregister.c,v 1.4 2006/12/19 08:33:17 hochstenbach Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <cconfig.h>

#define	MAX_PATH_LENGTH 1024
#define	BUFF_SIZE	1024

static int 	un_dbconf(const char *, const char *);
static int 	un_friends(const char *, const char *);
static int	un_load(const char *);
static char	*substr(const char *, int , int );

typedef void (*sighandler_t)(int);

extern int 	errno;
char		dbfile[MAX_PATH_LENGTH];
const  char 	*prog = "unregister";

int	main(int argc, char *argv[]) {
	int		ret, euid;
	struct stat	sb_conf, sb_friends;
	char	*url;
	char    *gwdbconf    = getenv("GWDBCONF");
	char	*gwdbfriends = getenv("GWDBFRIENDS");

	if (argc != 2) {
		fprintf(stderr, "usage: %s url\n", prog);
		return(1);
	}

	url = argv[1];

	if (gwdbconf == NULL) {
		fprintf(stderr, "%s: GWDBCONF not set\n", prog);
		return(2);
	} 

	if (gwdbfriends == NULL) {
		fprintf(stderr, "%s: GWDBFRIENDS not set\n", prog);
		return(2);
	}

	if (getenv("GWSPOOL") == NULL) {
		fprintf(stderr, "%s: GWSPOOL not set\n", prog);
		return(2);
	}

	if (getenv("GWBASEURL") == NULL) {
		fprintf(stderr, "%s: GWBASEURL not set\n", prog);
		return(2);
	}

	if (stat(gwdbconf, &sb_conf) == -1) {
		fprintf(stderr, "%s: cannot stat `%s': %s\n", prog, gwdbconf, strerror(errno));
		return(3);
	}

	if (stat(gwdbfriends, &sb_friends) == -1) {
		fprintf(stderr, "%s: cannot stat `%s': %s\n", prog, gwdbfriends, strerror(errno));
		return(3);
	}

	/* It is dangerous the change the gwdbconf and gwdbfriends running with root priviledges.
 	 * We could end up with wrong permissions. 
	 */
	euid = geteuid();

	if (euid == 0) {
		fprintf(stderr, "%s: cannot proceed: running the program as root could break up correct permissions\n", prog);
		return(3);
	}

	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	ret = un_dbconf((const char *) gwdbconf, (const char *)url);

	if (ret == -1) {
		return(4);
	}

	ret = un_friends((const char *) gwdbfriends, (const char *)url);

	if (ret == -1) {
		return(5);
	}

	ret = un_load((const char *)url);

	if (ret == -1) {
		return(6);
	}

	fprintf(stdout,"Ok\n");

	return(0);
}

static int 	un_dbconf(const char *gwdbconf, const char *url) {
	char	*frag, *p;
	int	config;

	config = config_open(gwdbconf, READ_WRITE);

	if (config == -1) {
		fprintf(stderr, "%s: cannot open `%s'\n", prog, gwdbconf);
		return(-1);
	}

	frag = substr(url,strlen("http:/"),strlen(url));

	if (frag == NULL) {
		fprintf(stderr, "%s: substr error\n", prog);
		return(-1);
	}

	p = (char *) config_param(config, frag);

	if (strlen(p) == 0) {
		fprintf(stderr, "%s: cannot unregister `%s': not registered\n", prog, url);
		free(frag);
		return(-1);
	}

	dbfile[0] = '\0';	
	strcpy(dbfile, p);

	config_param_del(config, frag);
	config_save(config);
	config_close(config);

	free(frag);

	return(0);
}

static int	un_friends(const char *gwdbfriends, const char *url) {
	struct stat	sb;
	char	buffer[BUFF_SIZE], searchstr[BUFF_SIZE];
	char	tmpfile[MAX_PATH_LENGTH];
	char	*frag;
	int	fd;
	FILE	*in, *out;

	if (stat(gwdbfriends, &sb) == -1) {
		fprintf(stderr, "%s: cannot stat `%s': %s\n", prog, gwdbfriends, strerror(errno));
		return(-1);
	}

	in = fopen(gwdbfriends, "r");

	if (in == NULL) {
		fprintf(stderr, "%s: cannot open `%s': %s", prog, gwdbfriends, strerror(errno));
		return(-1);
	}

	sprintf(tmpfile, "%s/friends.tmp", getenv("GWSPOOL"));

	fd = open(tmpfile, O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if (fd == -1) {
		fprintf(stderr, "%s: cannot open tmp file: %s\n", prog, tmpfile, strerror(errno));
		fclose(in);
		return(-1);
	}

#ifdef __svr4__	
	if (lockf(fd, F_LOCK, 0) == -1) {
#else	
	if (flock(fd, LOCK_EX) == -1) {
#endif		
		fprintf(stderr, "%s: cannot lock `%s': %s\n", prog, tmpfile, strerror(errno));
		close(fd);
		fclose(in);
		unlink(tmpfile);
		return(-1);
	}

	out = fdopen(fd,"w");

	if (out == NULL) {
		fprintf(stderr, "%s: cannot fdopen: %s\n", prog, strerror(errno));
		fclose(out);
		fclose(in);
		unlink(tmpfile);
		return(-1);
	}	

	/* Delete the line that contains <baseURL>url</baseURL>. This is not good 
	 * XML processing. In the future friends could be calculated from the 
	 * list of registered databases.
	 */
	frag = substr(url,strlen("http:/"),strlen(url));

	sprintf(searchstr, "<baseURL>%s%s</baseURL>", getenv("GWBASEURL"), frag);

	while(fgets(buffer, BUFF_SIZE, in) != NULL) {
		if (strstr(buffer, searchstr) == NULL)
			fprintf(out, buffer);
	}

	free(frag);

	if (rename(tmpfile, gwdbfriends) == -1) {
		fprintf(stderr, "%s: cannot rename `%s' to `%s': %s\n", prog, tmpfile, gwdbfriends, strerror(errno));
		fclose(out);
		fclose(in);
		unlink(tmpfile);
		return(-1);
	}

	chmod(gwdbfriends, sb.st_mode);

	fclose(out);
	fclose(in);

	return(0);
}

static int	un_load(const char *url) {
	struct stat	sb;

	if (dbfile == NULL) {
		fprintf(stderr, "%s: cannot delete datafile: unkown\n", prog);
		return(-1);
	}

	if (stat(dbfile, &sb) == -1) {
		fprintf(stderr, "%s: cannot stat `%s': %s\n", prog, dbfile, strerror(errno));
		return(-1);
	}		 

	if (unlink(dbfile) == -1) {
		fprintf(stderr, "%s: cannot remove `%s': %s\n", prog, dbfile, strerror(errno));
	}

	return(0);
}

static char	*substr(const char *str, int start, int l) {
	int  i;
	char *p;

	p = (char *) malloc(sizeof(char *) * l + 1);

	if (p == NULL) {
		return(NULL);
	}

	for (i = start ; i - start < l || str[i] == '\0' ; i++) {
		p[i-start] = str[i];
	}

	p[i-start] = '\0';

	return p;
}

