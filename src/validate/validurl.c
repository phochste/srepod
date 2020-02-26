/*
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * Read fields from the Identify section of a static repository
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include "expat.h"

#define	BUFF_SIZE 		1024
#define STATIC_REPOSITORY_NS 	"http://www.openarchives.org/OAI/2.0/static-repository"
#define OPEN_ARCHIVES_NS     	"http://www.openarchives.org/OAI/2.0/"

extern int errno;
const char *prog = "validurl";

typedef	struct {
	int	depth;
	int	identify;
	int	read;
	char	*element;
	char	*result;
} parse_struct;

static char	*substr(const char *, int , int );

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


static void startElement(void *userData, const char *name, const char **atts) {
	parse_struct 	*parse_session = userData;

	if (parse_session->depth == 1 && strcmp(name, STATIC_REPOSITORY_NS "|Identify") == 0) {
		parse_session->identify = 1;
	}

	if (parse_session->identify && strcmp(name, parse_session->element) == 0) {
		parse_session->read = 1;
	}

	parse_session->depth++;
}

static void charData(void *userData, const XML_Char *s, int len) {
	parse_struct 	*parse_session = userData;

	if (parse_session->read)  {
		
		if (strlen(parse_session->result) + len > BUFF_SIZE) {
			fprintf(stderr, "%s: buffer overflow\n", prog);
			exit(127);
		}

		strncat(parse_session->result, s, len);
	}
}

static void endElement(void *userData, const char *name) {
	parse_struct 	*parse_session = userData;

	parse_session->depth--;

	if (parse_session->identify && strcmp(name, parse_session->element) == 0) {
		parse_session-> read = 0;
	}

	if (parse_session->depth == 1 && strcmp(name, STATIC_REPOSITORY_NS "|Identify") == 0) {
		parse_session->identify = 0;
	}
}

int	main(int argc, char *argv[]) {
	FILE		*fp;
	struct stat 	sb;
	parse_struct	parse_session;
	char		*filename;
	char		*frag, *baseurl;
	char            buf[16384];
	char		search[BUFF_SIZE];
	char		result[BUFF_SIZE];
	char		match[BUFF_SIZE];
	XML_Parser      parser = XML_ParserCreateNS(NULL,'|');
	int		done;

	if (argc != 3) {
		fprintf(stderr, "usage: %s filename baseurl\n", prog);
		return(1);
	}

	if (getenv("GWBASEURL") == 0) {
		fprintf(stderr, "%s : GWBASEURL not set\n", prog);
		return(2);
	}

	filename = argv[1];
	baseurl  = argv[2];

/**
	if (stat(filename, &sb) == -1) {
		fprintf(stderr, "%s: cannot stat `%s': %s\n", prog, filename, strerror(errno));
		return(2);
	}
**/

	if (strcmp(filename,"-") == 0) {
		fp = stdin;
	}
	else {
		fp = fopen(filename, "r");
	}

	if (fp == NULL) {
		fprintf(stderr, "%s: cannot open `%s': %s\n", prog, filename, strerror(errno));
		return(3);
	}

	sprintf(search, "%s|%s", OPEN_ARCHIVES_NS, "baseURL");

	bzero(result, 1024);

	parse_session.depth	= 0;
	parse_session.identify  = 0;
	parse_session.read	= 0;
	parse_session.element   = (char *)search;
	parse_session.result    = (char *)result;

	XML_SetUserData(parser, &parse_session);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, charData);

	do {
		size_t len = fread(buf, 1, sizeof(buf), fp);
		done = len < sizeof(buf);
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
      			fprintf(stderr, "%s: %s at line %d\n"
					, prog
              				, XML_ErrorString(XML_GetErrorCode(parser))
              				, XML_GetCurrentLineNumber(parser));
      			exit(4);
    		}
  	} while (!done);


	if (parse_session.result[0] == '\0') 
		return(5);

	
	frag = substr(baseurl, strlen("http:/"), strlen(baseurl));

	sprintf(match, "%s%s", getenv("GWBASEURL"), frag);

	if (strcmp(match, parse_session.result) != 0) {
		fprintf(stderr, "%s: file url `%s' doesn't match `%s'\n", prog, parse_session.result, match);
		return(6);
	}

	free(frag);
  
  	XML_ParserFree(parser);

	return(0);
}
