/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: test.c,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include <template.h>

#define TEST_FILE  "test.tpl"
#define TEST_FILE2 "test2.tpl"

char * const testargs[]  = { "foo" , "bar" , "los alamos" , "charly parker" ,"dizzy gillespie" , NULL};
char * const testargs2[] = { "foo" , "$1\n$2" , NULL};

int 	main(int argc, char *argv[]) {

	char **args;

	if (parse_template(stdout, TEST_FILE, testargs, PARSE_STRICT) == -1) {
		fprintf(stderr, "parse_template() failed\n");
		exit(1);
	}

	if (parse_template(stdout, TEST_FILE2, testargs2, PARSE_LOOSE) == -1) {
		fprintf(stderr, "parse_template() failed\n");
		exit(1);
	}

	args = args_template(0);

	return(0);
}
