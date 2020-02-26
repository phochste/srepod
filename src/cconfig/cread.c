/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: cread.c,v 1.2 2006/12/19 08:33:12 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include <cconfig.h>

int	main(int argc, char *argv[]) {
	int	c;

	if (argc != 3) {
		fprintf(stderr, "usage : %s file param\n", argv[0]);
		exit(1);
	}

	c = config_load(argv[1]);

	if (c == -1) {
		fprintf(stderr, "error: failed to open %s\n", argv[1]);
	}

	printf("%s\n", config_param(c, argv[2]));

	return(0);		
}
