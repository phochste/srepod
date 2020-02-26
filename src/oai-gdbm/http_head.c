/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: http_head.c,v 1.3 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */
#include "http_head.h"


/* This function returns a pointer to a struct head_struct or
 * NULL on error.
 */
HS	*init_header(void) {
	HS	*hs;

	hs = (struct head_struct *) malloc(sizeof(HS));

	if (hs == NULL) {
		warn("init_header -  memory allocation error");
		return NULL;
	}

	hs->etag 	     = NULL;
	hs->date 	     = NULL;
	hs->content_encoding = NULL;
	hs->content_type     = NULL;
	hs->last_modified    = NULL;
	hs->server 	     = NULL;
	hs->via		     = NULL;

	return hs;
}

/* This function destroys a struct head_struct.
 */
void	free_header(HS *hs) {
	if (hs->etag != NULL) 
		free(hs->etag);
	
	if (hs->date != NULL)
		free(hs->date);

	if (hs->content_encoding != NULL)
		free(hs->content_encoding);

	if (hs->content_type != NULL)
		free(hs->content_type);

	if (hs->last_modified != NULL)
		free(hs->last_modified);

	if (hs->server != NULL)
		free(hs->server);

	if (hs->via != NULL)
		free(hs->via);

	free(hs);
}

/* This function should return the size of the header and fills
 * in the struct head_struct pointed at by userp. Returns -1 on
 * error.
 */
size_t 	parse_header(void *ptr, size_t size, size_t nmemb, void *userp) {
	HS 	*hs;

	/* Make sure we don't get more than this ammount of data in our buffers
	 */
	if (size * nmemb > MAX_HEAD_SIZE) {
		warn("parse_header - abnormal header size %d * %d", size ,nmemb);
		return size * nmemb;
	}

	hs = userp;

	debug("parse_header - %s",ptr);

	/* Check if we already parsed the status_code */
	if (hs->status_code == -1) {
		/* Check for 1.0 or 1.0 responses */
		if (! (memcmp((char *)ptr,"HTTP/1.0 ",9) == 0 || memcmp((char *)ptr,"HTTP/1.1 ",9) == 0)) {
			warn("parse_header - protocol nor HTTP/1.0 not HTTP/1.1");
			return -1;
		}

		if (sscanf(ptr,"HTTP/1.%*[01] %3d",&(hs->status_code)) != 1) {
			warn("parse_header - header in unknown format");
			return -1;
		}
	}
	else if (memcmp((char *)ptr,"Etag:",5) == 0) {
		if (hs->etag != NULL)
			free(hs->etag);

		hs->etag = (char *) malloc(sizeof(char)*size*nmemb);

		if (hs->etag == NULL)  {
			warn("parse_header - memory allocation error");
			return -1;
		}

		if (sscanf(ptr,"Etag: \"%[a-z0-9-]\"", hs->etag) != 1) {
			warn("parse_header - etag in unknown format");
			return -1;
		}
	}
	else if (memcmp((char *)ptr,"Date:",5) == 0) {
		if (hs->date != NULL)
			free(hs->date);

		hs->date = (char *) malloc(sizeof(char)*size*nmemb);

		if (hs->date == NULL) {
			warn("parse_header - memory allocation error");
			return -1;
		}

		/* Copy everything from 'Date: ' until the '\r\n' into
		 * the date buffer.
		 */
		memcpy(hs->date, ptr + 6, size*nmemb - 6 - 2);

		hs->date[size*nmemb - 6 - 2] = '\0';
	}
	else if (memcmp((char *)ptr,"Content-Type:",13) == 0) {
		if (hs->content_type != NULL)
			free(hs->content_type);

		hs->content_type = (char *) malloc(sizeof(char)*size*nmemb);

		if (hs->content_type == NULL) {
			warn("parse_header - memory allocation error");
			return -1;
		}

		if (sscanf(ptr,"Content-Type: %[a-z/-]",hs->content_type) != 1) {
			warn("parse_header - content-type in unkown format");
			return -1;
		}
	}
	else if (memcmp((char *)ptr,"Server:",7) == 0) {
		if (hs->server != NULL)
			free(hs->server);

		hs->server = (char *) malloc(sizeof(char)*size*nmemb);

		if (hs->server == NULL) {
			warn("parse_header - memory allocation error");
			return -1;
		}

		/* Copy everything from 'Server: ' until the '\r\n' into
		 * the server buffer.
		 */
		memcpy(hs->server, ptr + 8, size*nmemb - 8 - 2);

		hs->server[size*nmemb - 8 - 2] = '\0';
	}
	else if (memcpy((char *)ptr,"Last-Modified:",14) == 0) {
		if (hs->last_modified != NULL)
			free(hs->last_modified);

		hs->last_modified = (char *) malloc(sizeof(char)*size*nmemb);

		if (hs->last_modified == NULL) {
			warn("parse_header - memory allocation error");
			return -1;
		}

		/* Copy everything from 'Last-Modified: ' until the '\r\n' into
		 * the last_modified buffer.
		 */
		memcpy(hs->last_modified, ptr + 15, size*nmemb - 15 - 2);

		hs->last_modified[size*nmemb - 15 - 2] = '\0';

	}
	else if (memcpy((char *)ptr,"Via:",4) == 0) {
		if (hs->via != NULL)
			free(hs->last_modified);

		hs->via = (char *) malloc(sizeof(char)*size*nmemb);

		if (hs->via == NULL) {
			warn("parse_header - memory allocation error");
			return -1;
		}

		/* Copy everything from 'Via: ' until the '\r\n' into
		 * the via buffer.
		 */
		memcpy(hs->via, ptr + 5, size*nmemb - 5 - 2);

		hs->via[size*nmemb - 5 - 2] = '\0';
	}
	else if (memcpy((char *)ptr,"Content-Encoding:",17) == 0) {
		if (hs->content_encoding != NULL)
			free(hs->content_encoding);

		hs->content_encoding = (char *) malloc(sizeof(char)*size*nmemb);

		if (hs->content_encoding == NULL) {
			warn("parse_header - memory allocation error");
			return -1;
		}

		/* Copy everything from 'Content-Encoding: ' until the '\r\n' into
		 * the content_encoding buffer.
		 */
		memcpy(hs->content_encoding, ptr + 18 , size*nmemb - 18 - 2);

		hs->content_encoding[size*nmemb - 18 - 2] = '\0';
	}

	return size * nmemb;
}

/* This functions connects to url and requests a header with if-modified-since
 * equal to the modification_time. The returned header is written into a
 * struct head_struct pointed at by hs. The function return 0 on success
 * and a non-zero value on error
 */
int	simple_header(char *url, char *modification_time, HS *hs) {
	time_t	 t;
	char	 proxy_url[MAX_URL_LENGTH];
	char	 buf[MAX_URL_LENGTH];
	struct curl_slist *headerlist=NULL;
	CURL     *curl;
	CURLcode res;

	hs->status_code = -1;

	curl = curl_easy_init();

	if (curl == NULL) {
		warn("simple_header - failed tp curl_easy_init()");
		return -1;
	}

	debug("simple_header - requesting header for %s",url);

	/* Request a HEAD */
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST , "HEAD");

	/* From this url */

	/* Create an url that can bypass a proxy server */
	time(&t);
	srand(t);
	sprintf(proxy_url, "%s?%d", url, rand());
	curl_easy_setopt(curl, CURLOPT_URL, proxy_url);

	/* Where the modification time is later then modification */
	sprintf(buf,"If-Modified-Since: %s", modification_time);
	headerlist = curl_slist_append(headerlist, buf);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

	/* Return header information */
	curl_easy_setopt(curl, CURLOPT_HEADER, TRUE);

	/* Don't return the body */
	curl_easy_setopt(curl, CURLOPT_NOBODY , TRUE);

	/* Parse the header with this function */
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_header);
	curl_easy_setopt(curl, CURLOPT_FILE, hs);

	/* This should take us HEAD_TIMEOUT_VALUE seconds */
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, HEAD_TIMEOUT_VALUE);

#ifdef CURLDEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, TRUE);
#endif

	res = curl_easy_perform(curl);

	curl_easy_cleanup(curl);

	curl_slist_free_all(headerlist);

	return res;
}
