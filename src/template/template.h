/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: template.h,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
 * 
 */
#ifndef _template_h
#define _template_h

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <debug.h>

enum parse_mode {
	PARSE_STRICT ,
	PARSE_LOOSE
};

/* Parse the filename and replace every $N with the content of
 * args[N] print the output to fd. Args should be a NULL terminated
 * array of strings. Parse_mode tells the function how to parse
 * the temaplate:
 *
 *   parse_mode = PARSE_STRICT : only the $N's defined
 *                               in args will be replaced, all
 *                               not recognized $N's will be
 *                               copied verbatim.
 *                PARSE_LOOSE : all the $N's will be replaced.
 *                              If they aren't defined in the args,
 *                              then they will be replaced by
 *                              an empty string.
 * Returns number of bytes written on success or -1 on error.
 */
int	parse_template(FILE *fout, const char* filename, char * const args[], enum parse_mode mode);

/* Returns a pointer to all strings in the var arg or NULL on error */
char 	**args_template(int size,...); 

#endif
