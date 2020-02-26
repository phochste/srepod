/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: http_head.h,v 1.4 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */
#ifndef	__http_head_h
#define	__http_head_h

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <debug.h>

#define HEAD_TIMEOUT_VALUE	60 		/* How long do we wait for a header response  */
#define MAX_HEAD_SIZE		256		/* How much date can each header line contain */
#define	MAX_URL_LENGTH		1024

struct	head_struct {
	int	status_code;
	char	*etag;
	char	*date;
	char 	*content_encoding;
	char	*content_type;
	char	*last_modified;
	char	*server;
	char	*via;
};

typedef	struct	head_struct HS;

/* This function returns a pointer to a struct head_struct or
 * NULL on error.
 */
HS	*init_header(void);

/* This function destroys a struct head_struct.
 */
void	free_header(HS *hs);

/* This functions connects to url and requests a header with if-modified-since
 * equal to the modification_time. The returned header is written into a
 * struct head_struct pointed at by hs. The function return 0 on success
 * and a non-zero value on error
 */
int	simple_header(char *url, char *modification_time, HS *hs);

/* This function should return the size of the header and fills
 * in the struct head_struct pointed at by userp. Returns 
 * number of parsed header bytes on or -1 on error.
 */
size_t	parse_header(void *ptr, size_t size, size_t nmemb, void *userp);


#endif
