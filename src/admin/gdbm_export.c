/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: gdbm_export.c,v 1.2 2006/12/19 08:33:11 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <gdbm.h>
#include <unistd.h>

char	*toline(char *str,int n);

int main(int argc, char *argv[]) {
        GDBM_FILE       dbf;
        datum           key,nextkey,data;
	char		*p;

	if (argc != 2) {
		fprintf(stderr,"usage : %s database\n",argv[0]);
		exit(1);
	}

        dbf = gdbm_open(argv[1], 1024, GDBM_READER, 0644, 0);

        if (dbf == NULL) {
                fprintf(stderr,"gddm_open error\n");
                exit(1);
        }

        key = gdbm_firstkey(dbf);

        while (key.dptr) {
                data = gdbm_fetch(dbf,key);

                nextkey = gdbm_nextkey(dbf,key);
	
		p = toline(data.dptr, data.dsize);

                write(1,key.dptr,key.dsize);
                write(1,"\t",1);
                write(1,p,strlen(p));
                write(1,"\n",1);

                free(key.dptr);
                free(data.dptr);
		free(p);

                key = nextkey;
        }

        gdbm_close(dbf);

	return(0);
}

char	*toline(char *str, int n) {
	char 	*p,*q;

	p = (char *) malloc(sizeof(char *)*(n*2) + 1);

	if (p == NULL) {
		fprintf(stderr, "memory allocation error\n");
		return(NULL);
	}

	q = p;

	while(n--) {
		switch(*str) {
			case '\n':
				*p++ = '\\';
				*p++ = 'n';
				break;
			case '\r':
				*p++ = '\\';
				*p++ = 'r';
				break;
			case '\t':
				*p++ = '\\';
				*p++ = 't';
				break;
			case '\\':
				*p++ = '\\';
				*p++ = '\\';
				break;	
			default:	
				*p++ = *str;
		}
		str++;
	}

	*p++ = '\0';

	return q;
}
