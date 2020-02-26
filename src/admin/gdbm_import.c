/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: gdbm_import.c,v 1.2 2006/12/19 08:33:12 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include <gdbm.h>
#include <unistd.h>

#define BUFSIZE         100 * 1024

char	*fromline(char *str, int n);

int main(int argc, char *argv[]) {
        GDBM_FILE       dbf;
        char            *buffer,*keybuf,*databuf,*p;
        datum           key,data;
        int             line;

	if (argc != 2) {
                fprintf(stderr,"usage : %s database < data.exp\n",argv[0]);
                exit(1);
        }

        unlink(argv[1]);

        dbf = gdbm_open(argv[1], 1024, GDBM_WRCREAT, 0644, 0);

        if (dbf == NULL) {
                fprintf(stderr,"gddm_open error\n");
                exit(1);
        }

        buffer  = (char *) malloc(sizeof(char)*BUFSIZE);
        keybuf  = (char *) malloc(sizeof(char)*BUFSIZE);
        databuf = (char *) malloc(sizeof(char)*BUFSIZE);
        
        if (buffer == NULL || keybuf == NULL || databuf == NULL) {
                fprintf(stderr,"malloc error\n");
                exit(2);
        }

        line = 0;

        while ( fgets(buffer, BUFSIZE,stdin) != NULL ) {
               line++; 
        
	       if (sscanf(buffer, "%s\t%[^\n]\n", keybuf, databuf) == 2) {
			p = fromline(databuf, strlen(databuf));
	       }
	       else if (sscanf(buffer, "%s\t\n", keybuf) == 1) {
		       	p = "";
	       }
	       else {
		        fprintf(stderr, "error on line %d\n", line);
		        unlink(argv[1]);
		        exit(2);
	       }

               key.dptr   = keybuf;
               key.dsize  = strlen(keybuf);

               data.dptr  = p;
               data.dsize = strlen(p);
               
               if (gdbm_store(dbf,key,data,GDBM_INSERT) != 0) {
                       fprintf(stderr,"error on line %d\n",line);
               }
               else {
                       printf("stored %s\n",keybuf);
               }
        }

        gdbm_close(dbf);
}

char	*fromline(char *str, int n) {
	char *p, *q;

	p = (char *) malloc(sizeof(char *)*n + 1);

	if (p == NULL) {
		fprintf(stderr, "memory allocation error\n");
		return(NULL);
	}

	q = p;

	while(*str) {
		if (*str == '\\') {
			switch(*(str + 1)) {
				case 'n':
					*p++ = '\n';
					break;
				case 'r':
					*p++ = '\r';
					break;
				case 't':
					*p++ = '\t';
					break;
				case '\\':
					*p++ = '\\';
					break;
				default:
					*p++ = *(str);
					*p++ = *(str+1);
					break;
			}
			str += 2;
		}
		else {
			*p++ = *str++;
		}
	}

	*p++ = '\0';

	return(q);
}
