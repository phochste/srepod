/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: http_test.c,v 1.2 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include "http_head.h"

int	main(int argc, char *argv[]) {
	HS	*hs;

	if (argc != 3) {
		fprintf(stderr,"usage : %s url time_str\n", argv[0]);
		exit(1);
	}

	hs = init_header();

	if (simple_header(argv[1], argv[2], hs) != 0) {
		fprintf(stderr,"simple_header failed\n");
		exit(2);
	}

	printf("%s : got a %d\n", argv[0], hs->status_code);
	return(0);
}
