/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: defs.h,v 1.3 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */

#ifndef	__defs_h
#define	__defs_h

#include <stdio.h>
#include <string.h>
#include "stack.h"
#include "hash.h"
#include "expat.h"

#define URISIZE                     1024
#define LOCATIONSIZE                1024
#define NAMESIZE                    1024
#define PREFIXSIZE                  1024
#define SCHEMALOCATIONSIZE	        4048 
#define	REPOSITORYDEPTH		        0
#define LISTRECORDSDEPTH            1
#define OAIMETADATADEPTH            3
#define STATIC_REPOSITORY_NAMESPACE "http://www.openarchives.org/OAI/2.0/static-repository"
#define OAI_PMH_NAMESPACE           "http://www.openarchives.org/OAI/2.0/"
#define SCHEMA_NAMESPACE	        "http://www.w3.org/2001/XMLSchema-instance"

#define SEPARATOR                   '|'

typedef struct  {
    char    metadataPrefix[PREFIXSIZE];
    int     depth;
    int     inListRecords;
    int     inRecord;
	int	    inAbout;
    HASH    inHash;  /* Hash of all namespace/prefix used inside records */
	HASH	abHash;  /* Hash of all namespace/prefix used inside about containers */
} parse_struct;

HASH    outHash;        /* Hash of all namespace/prefix defined outside records */
HASH    schemaHash;     /* Hash of all namespace/schema defined outside records */
STACK	*prefixStack;	/* For each prefix this stack contains the namespaces to add in scan2 */

extern	char *pname;

void	scan1(const char *filename);
void	scan2(const char *filename);

#endif
