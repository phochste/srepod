/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: get.c,v 1.2 2006/12/19 08:33:14 hochstenbach Exp $
 * 
 * get - fetch data from a http address on the Internet
 *
 * usage: get [options] http_address
 *
 * options:
 *	-h 	      (include the head in the output)
 *	-p	      (try to bypass proxy servers)
 * 	-o file
 * 	-s maxsize
 * 	-t timeout
 * 	
 * exit values:
 *
 * 	0 	- success
 * 	1	- invalid command arguments
 * 	2	- output file can't be created
 * 	3	- timeout reached
 * 	4	- maxsize reached
 * 	5	- internal error
 *
 * only exit value 0 will create an output file (except when writing to stdout).	
 *
 */
#include <stdio.h>
#include <curl/curl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define	MAX_PATH 1024
#define MAX_URL	 1024
#define BUF_SIZE 1024

extern char 	*optarg;
extern int 	optind, opterr, optopt;

typedef struct {
	FILE	*fp;
	int	timeout;
	int	maxsize;
	size_t	cursize;
	char	outfile[MAX_PATH];
} opt_struct;

void	usage(void);
size_t 	mywriter(void *ptr, size_t size, size_t nmemb, void *userp);

void	usage(void) {
	fprintf(stderr,"usage: get [options] http_address\n"
		       "\n"
		       "options:\n"
		       "        -h\n"
		       "        -p\n"
		       "        -o file\n"
		       "        -s maxsize\n"
		       "        -t timeout\n\n"
	       );
	exit(1);
}

size_t 	mywriter(void *ptr, size_t size, size_t nmemb, void *userp) {
	size_t 	   n;
	opt_struct *opt_p;

	opt_p = userp;

	/* Check if we don't get more than we requested */
	if (opt_p->maxsize > 0  && opt_p->cursize + size * nmemb > opt_p->maxsize) 
		return(-1);
	
	n = fwrite(ptr, size, nmemb, opt_p->fp);

	opt_p->cursize += n * size;

	return(n * size);
}
		       

int	main(int argc, char *argv[]) {
	int		exit_val;
	int		c;
	int		doheader = 0;
	int		doproxy  = 0;
	time_t		t;
	char		url[MAX_URL];
	CURL		*curl;
	CURLcode	res;
	opt_struct	opt;	

	opt.fp	       =  stdout; /* Default write to stdout */
	opt.timeout    = -1;
	opt.maxsize    = -1;
	opt.cursize    =  0;
	opt.outfile[0] = '\0';

	while( (c = getopt(argc,argv,"ho:ps:t:")) != EOF ) {
		switch (c) {
			case 'h':
				doheader = 1;
				break;
			case 'o':
				strncpy(opt.outfile,optarg,1024);
				/* make sure the filename has a null at the end */
				opt.outfile[1023] = '\0';
				break;
			case 'p':
				doproxy = 1;
				break;
			case 's':
				opt.maxsize = atoi(optarg);
				break;
			case 't':
				opt.timeout = atoi(optarg);
				break;
			default:
				usage();
		}
	}

	if (argc - optind == 1) {
		strncpy(url,argv[optind],1024);
		url[1023] = '\0';
	}
	else {
		usage();
	}

	if (doproxy) {
		time(&t);
		srand(t);
		sprintf(url, "%s?%d", url, rand());
	}

	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);

		curl_easy_setopt(curl, CURLOPT_HEADER, doheader);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mywriter);
		curl_easy_setopt(curl, CURLOPT_FILE, &opt);

		if (opt.timeout > 0) 
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, opt.timeout);

		if (opt.outfile[0] != '\0') {
			umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

			opt.fp = fopen(opt.outfile, "w");

			if (opt.fp == NULL) {
				exit(2);
			}
		}

		res = curl_easy_perform(curl);

		fclose(opt.fp);

		if (res == CURLE_OK) {
			exit_val = 0;
		}	
		else if (res == CURLE_OPERATION_TIMEOUTED) {
			unlink(opt.outfile);
			exit_val = 3;
		}
		else if (res == CURLE_WRITE_ERROR) {
			unlink(opt.outfile);
			exit_val = 4;
		}
		else {
			if (opt.outfile[0] != '\0')
				unlink(opt.outfile);
			exit_val = 5;
		}

		curl_easy_cleanup(curl);
	}
	else {
		exit_val = 5;
	}

	return(exit_val);
}
