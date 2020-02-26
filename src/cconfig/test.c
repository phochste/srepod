/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: test.c,v 1.3 2006/12/19 08:33:12 hochstenbach Exp $
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "cconfig.h"


int main(int argc, char *argv[]) {
	int c1;
	int i,j;
	
	c1 = config_load("test1.dat");
	
	if (c1 < 0) {
		fprintf(stderr,"failed to open test1.dat\n");
		exit(1);
	}

	printf("c1 = %d\n",c1);

	printf("%s\n",config_param(c1,"hoi"));

	printf("%s\n",config_param(c1,"hoi"));

	config_param_add(c1, "food", "fiber");

	printf("%s\n",config_param(c1,"food"));

	config_param_set(c1, "food", "vegetable");

	printf("%s\n",config_param(c1,"food"));

	config_param_del(c1, "food");

	printf("%s\n",config_param(c1,"food"));

	config_free(c1);

	c1 = config_open("test1.dat",READ_WRITE);
	
	if (c1 < 0) {
		fprintf(stderr,"failed to open test1.dat\n");
		exit(1);
	}

	printf("c1 = %d\n",c1);

	printf("%s\n",config_param(c1,"hoi"));

	printf("%s\n",config_param(c1,"hoi"));

	config_param_add(c1, "food", "fiber");

	printf("%s\n",config_param(c1,"food"));

	config_param_set(c1, "food", "vegetable");

	printf("%s\n",config_param(c1,"food"));

	config_param_del(c1, "food");

	printf("%s\n",config_param(c1,"food"));

	printf("sleeping 10 seconds\n");
	sleep(10);
	printf("wake up, closing hashes\n");

	config_free(c1);


	return(0);
}
