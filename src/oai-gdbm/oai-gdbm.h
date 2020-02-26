/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: oai-gdbm.h,v 1.2 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */
#ifndef __oai_gdbm_h
#define __oai_gdbm_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <gdbm.h> 
#include <timefunc.h>
#include <oai-interface.h>
#include <cconfig.h>
#include <session.h>
#include <debug.h>
#include <template.h>

#ifndef GWDBCONF
	#define	GWDBCONF	"./gateway.conf"
#endif

#ifndef	GWDBFILE
	#define	GWDBFILE	"./db/test.dbf"
#endif

#define	MAX_PATH_LENGTH		1024
#define	MAX_URL_LENGTH		1024

enum	open_error_types {
	OPEN_SUCCESS=0,				/* The cache copy is fresh everything is ok */
	OPEN_GATEWAY_LOCKED=-1,			/* The cache copy is not fresh the gateway is busy with the update */
	OPEN_INVALID_REPOSITORY=-2,		/* The static repository is invalid */
	OPEN_NETWORK_ERROR=-3,			/* The network location of the static repo is not accessible */
	OPEN_ACCESS_ERROR=-4,			/* A configuration failure at the network location */
	OPEN_NO_SUCH_GATEWAY=-5,		/* The static repository is unknown to the gateway */
	OPEN_CONFIGURATION_ERROR=-6,		/* The gateway is badly configured */
	OPEN_INTERNAL_ERROR=-7,			/* The gateway had problems processing the open request */
};

extern int errno;

#endif
