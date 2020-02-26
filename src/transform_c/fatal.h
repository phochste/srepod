/* 
 * Authors: Henry Jerez <hjerez@lanl.gov>
 *
 * $Id: fatal.h,v 1.1.1.1 2003/06/25 17:57:05 hochstenbach Exp $
 * 
 */
#ifndef _fatal_h
#define	_fatal_h

#include <stdio.h>
#include <stdlib.h>

#define Error( Str )        FatalError( Str )
#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )

#endif
