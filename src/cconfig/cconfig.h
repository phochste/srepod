/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: cconfig.h,v 1.4 2006/12/19 08:33:12 hochstenbach Exp $
 * 
 */
#ifndef __cconfig_h
#define __cconfig_h

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <debug.h>
#include <sys/file.h>
#include <unistd.h>
#include <signal.h>

enum config_mode { READ = 0 , READ_WRITE = 1 };

#define config_load(filename)	config_open(filename, READ)
#define config_free(handle)	config_close(handle)

int	config_open(const char *filename, int config_mode);
char 	*config_param(int handle, const char *token);
int	config_param_add(int handle, const char *token, const char *value);
int	config_param_set(int handle, const char *token, const char *value);
int	config_param_del(int handle, const char *token);
int	config_save(int handle);
void	config_close(int handle);

#endif
