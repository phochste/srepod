/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: hasout.c,v 1.2 2006/12/19 08:33:19 hochstenbach Exp $
 * 
 */
#include <stdio.h>

int	main(int argc, char *argv[]) {
	
	if (fgetc(stdin) == EOF) {
		return(0);
	}
	else {
		return(1);
	}
}
