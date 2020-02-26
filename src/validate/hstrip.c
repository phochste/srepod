/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: hstrip.c,v 1.3 2006/12/19 08:33:19 hochstenbach Exp $
 * 
 */
#include <stdio.h>

#define	BUFF_SIZE 2048

int	main(int argc, char *argv[]) {
	char	buffer[BUFF_SIZE];
	int	out = 0;

	while( fgets(buffer, BUFF_SIZE, stdin) != NULL) {
		if (buffer[0] == 0x0d && out == 0) {
			out = 1;
		}
		else if (out) {
			fprintf(stdout, "%s", buffer);
		}
	}
	return(0);
}
