/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: template.c,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
 * 
 */
#include "template.h"

int	parse_template(FILE *fout, const char* filename, char * const args[], enum parse_mode mode) {
	FILE *fp;
	int  sz = 0;
	int  c,n,d;
	long pos;

	c = 0;

	/* Count the number of arguments */
	while(args[c++] != NULL) sz++;

	fp = fopen(filename, "r");

	if (fp == NULL) {
		warn("parse_template - failed to open %s", filename);
		return(-1);
	}

	n = 0;
	while ( (c = fgetc(fp)) != EOF ) {
		/* If the dollar sign is reached we should try to fill in
		 * the appropiate arg
		 */
		if (c == '$') {
			pos = ftell(fp);
			/* Read the next characters and scan for digits */
			if (fscanf(fp, "%2d", &d) != 1) {
				if (mode == PARSE_STRICT) {
					fputc(c,fout);
					n++;
				}

				/* Roll back */
				fseek(fp, pos, SEEK_SET);

			}
			else if (d >= sz || d < 0) {
				if (mode == PARSE_STRICT)
					n += fprintf(fout, "$%d", d);
			}
			else {
				n += fprintf(fout, "%s", args[d]);
			}
			
		}
		/* If the dollar sign is not reached just keep on trucking..*/
		else {
			fputc(c,fout);
			n++;
		}
	}

	fflush(fout);

	fclose(fp);

	return(n);
}

char 	**args_template(int n,...) {
        va_list arg;
        char **carr;
        char *p;
        int   i = 0;

        carr = (char **) malloc(sizeof(char *) * n);

        if (carr == NULL) {
		warn("args_template - memory allocation error");
                return(NULL);
        }

        va_start(arg,n);
        while(i < n) {
                p = va_arg(arg, char *);

                carr[i++] = p;

                if (p == NULL)
                        break;
        }
        va_end(arg);

        return carr;
}
