/*
 * This work is licensed under the Creative Commons Attribution-NonCommercial 
 * License. To view a copy of this license, visit 
 *
 * http://creativecommons.org/licenses/by-nc/1.0/ 
 * 
 * or send a letter to Creative Commons, 559 Nathan Abbott Way, Stanford, 
 * California 94305, USA.
 */
/* 
 * GATEWAY:
 *
 * An OAI Static Repository Gateway implementation
 * 
 * Author: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: gateway.c,v 1.7 2006/12/19 08:33:11 hochstenbach Exp $
 * 
 */
#include "gateway.h"
#include <cgi-util.h>
#include <oai-gdbm.h>
#include <debug.h>
#include <template.h>

int     main(int argc, char *argv[]) {
        const char      *valid_arguments[] = { 
                        "verb" , "from" , "until" , "identifier" , 
                        "metadataPrefix" , "resumptionToken" , "set" , NULL
                                     };
        const char    *verb, *initiate, *terminate;
	char	      gwlog[MAX_PATH_LENGTH];
        int    	      ret;
	FILE	      *fp;

	/* Open the log file */
	if (getenv("GWSPOOL") != NULL) {
		(void) sprintf(gwlog, "%s/%s", getenv("GWSPOOL"), GATEWAY_LOG);
		fp = fopen(gwlog,"a");
		set_log_stream(fp);
	}

	/* Do some secutiry checks */
	debug("main - checking CGI environment");
	if (! has_secure_env()) {
		warn("main - CGI environment insecure!");
		(void) cmd_error();
		exit(1);
	}

	/* Parse the CGI query */
	ret = cgi_init();

	if (ret != CGIERR_NONE) {
		warn("main - recieved bad request from %s", getenv("REMOTE_ADDR"));
		/* Be gentle and blame the client */
		errmsg  = "Bad request";
		errcode = ERR_HTTP_404;
		(void) cmd_error();
		exit(1);
	}
	else {
		log("main - requery recieved from %s", getenv("REMOTE_ADDR"));
	}

	/* If we recieve an initiate request, then redirect to the registry */
	if (cgi_getnumentries("initiate") == 1) {
		initiate = cgi_getentrystr("initiate");
		if (getenv("GWCONFIRM") != NULL && strcmp(getenv("GWCONFIRM"),"0") == 0) { 
			ret = printf("Location: %s?url=%s\r\n\r\n", getenv("GWREGISTRY"), initiate); fflush(stdout);
		}
		else {
			ret = printf("Location: %s?init_url=%s\r\n\r\n", getenv("GWREGISTRY"), initiate); fflush(stdout);
		}
        	log("main - [initiate] response %d bytes\n", ret);
		exit(0);
	}

	/* If we recieve an terminate request, the redirect to the terminator */
	if (cgi_getnumentries("terminate") == 1) {
		terminate = cgi_getentrystr("terminate");
		ret = printf("Location: %s?init_url=%s\r\n\r\n", getenv("GWTERMINATOR"), terminate); fflush(stdout);	
		log("main - [terminate] response %d bytes\n", ret);
		exit(0);
	}

	/* Check if the query has valid keys*/
        if (! has_valid_keys(valid_arguments)) {
		/* The errmsg and errcode should be set by the has_ functions */
		warn("main - recieved illegal request from %s", getenv("REMOTE_ADDR"));
                (void) cmd_error();
                exit(1);
        }

	/* Try to open the static repository */
	ret = oai_open_repository();

	if (ret != OPEN_SUCCESS ) {
		switch (ret) {
			case OPEN_GATEWAY_LOCKED:
				errmsg  = "Static repository not up to date, processing the update, come back later";
				errcode = ERR_HTTP_503;
				break;
			case OPEN_INVALID_REPOSITORY:
				errmsg  = "Static repository is not valid";
				errcode = ERR_HTTP_502;
				break;
			case OPEN_NETWORK_ERROR:
				errmsg  = "Network timeout";
				errcode = ERR_HTTP_504;
				break;
			case OPEN_ACCESS_ERROR:
				errmsg  = "Repository is not available";
				errcode = ERR_HTTP_504;
				break;
			case OPEN_NO_SUCH_GATEWAY:
				errmsg  = "Repository is unknown";
				errcode = ERR_HTTP_404;
				break;
			case OPEN_CONFIGURATION_ERROR:
				errmsg  = "Gateway configuration error";
				errcode = ERR_HTTP_502;
				break;
			case OPEN_INTERNAL_ERROR:
				errmsg  = "Internal gateway error";
				errcode = ERR_HTTP_502;
				break;
			default:
				errmsg  = "Unknown error";
				errcode = ERR_HTTP_502;
				break;
		}
		warn("main - failed to open repository : errcode %d", errcode);
		(void) cmd_error();
		exit(1);
	}

        verb = cgi_getentrystr("verb");

	log("main - got the verb '%s'", verb);
        if (strcmp(verb,"Identify") == 0) {
                ret = cmd_identify();
        }
        else if (strcmp(verb,"ListSets") == 0) {
                ret = cmd_listsets();
        }
        else if (strcmp(verb,"ListMetadataFormats") == 0) {
                ret = cmd_listmetadataformats();
        }
        else if (strcmp(verb,"ListIdentifiers") == 0) {
                ret = cmd_list(IDENTIFIERS);
        }
        else if (strcmp(verb,"ListRecords") == 0) {
                ret = cmd_list(RECORDS);
        }
        else if (strcmp(verb,"GetRecord") == 0) {
                ret = cmd_getrecord();
        }

	log("main - closing repository");
	oai_close_repository();

        if (ret == -1) {
		/* The errmsg and errcode should be set by all cmd_ functions */
		warn("main - failed to process request : errcode %d", errcode);
                (void) cmd_error();
                exit(1);
        }
        
        log("main - response %d bytes\n", ret);
	
        return(0);
}

int     cmd_error(void) {
        const char 	*valid_arguments[] = { NULL };
        char 		*err;
	char		**carr;
	char		*error_html;
	char		strerrcode[8];
        int  		n = 0;

	error_html = getenv("GWERRORHTML");
	sprintf(strerrcode, "%d", errcode);

        switch(errcode) {
                case ERR_BADARGUMENT :
                        err = "badArgument";
                        break;
		case ERR_BADRESUMPTIONTOKEN :
			err = "badResumptionToken";
			break;
                case ERR_BADVERB :
                        err = "badVerb";
                        break;
		case ERR_CANNOTDISSEMINATEFORMAT :
			err = "cannotDisseminateFormat";
			break;
		case ERR_IDDOESNOTEXIST :
                        err = "idDoesNotExist";
                        break;
                case ERR_NOMETADATAFORMATS :
			err = "noMetadataFormats";
			break;
		case ERR_NORECORDSMATCH :
			err = "noRecordsMatch";
			break;
		case ERR_NOSETHIERARCHY :       
                        err = "noSetHierarchy";
                        break;
		case ERR_HTTP_404 :
			n += printf("Status: %d - %s\r\n", errcode, errmsg); fflush(stdout);
		        n += printf("Content-type: text/html\r\n\r\n"); fflush(stdout);

			if (error_html != NULL) {
				carr = args_template(2,strerrcode, errmsg);
				n += parse_template(stdout, error_html, carr, PARSE_LOOSE);
				free(carr);
			}
			else {
		        	n += printf("<html><head><title>Server error %d</title></head>" \
				    	    "<body><h1>Server error %d</h1>Sorry - %s." \
				    	    "</body>"  \
				    	    "</html>", errcode, errcode, errmsg); 
			}

			fflush(stdout);
			return(n);
			break;
		case ERR_HTTP_503 :
			n += printf("Status: %d - %s\r\n", errcode, errmsg); fflush(stdout);
			n += printf("Retry-After: %d\r\n", RETRY_AFTER_SECS); fflush(stdout);
		        n += printf("Content-type: text/html\r\n\r\n"); fflush(stdout);

			if (error_html != NULL) {
				carr = args_template(2,strerrcode, errmsg);
				n += parse_template(stdout, error_html, carr, PARSE_LOOSE);
				free(carr);
			}
		        else {
				n += printf("<html><head><title>Server error %d</title></head>" \
				    "<body><h1>Server error %d</h1>Sorry - %s." \
				    "</body>"  \
				    "</html>", errcode, errcode, errmsg); 
			}

			fflush(stdout);
		        return(n);
			break;
		case ERR_HTTP_400 :
		case ERR_HTTP_500 :
		case ERR_HTTP_501 :
		case ERR_HTTP_502 :
		case ERR_HTTP_504 :
		case ERR_HTTP_505 :	
		 	n += printf("Status: %d - %s\r\n", errcode, errmsg); fflush(stdout);
		        n += printf("Content-type: text/html\r\n\r\n"); fflush(stdout);

			if (error_html != NULL) {
				carr = args_template(2,strerrcode, errmsg);
				n += parse_template(stdout, error_html, carr, PARSE_LOOSE);
				free(carr);
			}
			else {
		        	n += printf("<html><head><title>Server error %d</title></head>" \
				    "<body><h1>Server error %d</h1>Sorry - %s." \
				    "</body>"  \
				    "</html>", errcode, errcode, errmsg); 
			}

			fflush(stdout);
		        return(n);
			break;
                default :
			/* Send an internal server error */
			n += printf("Status: 500 - Unknown error\r\n"); fflush(stdout);
			n += printf("Content-type: text/html\r\n\r\n"); fflush(stdout);

			if (error_html != NULL) {
				carr = args_template(2, "500", errmsg);
				n += parse_template(stdout, error_html, carr, PARSE_LOOSE);
				free(carr);
			}
			else {
		        	n += printf("<html><head><title>Server error 500</title></head>" \
				    "<body><h1>Server error 500</h1>Sorry - %s." \
				    "</body>"  \
				    "</html>", errmsg); 
			}

			fflush(stdout);
			return(n);
                        break;
        }

        n += cmd_header(valid_arguments);

        n += printf("<error code=\"%s\">%s</error>" , err, errmsg); fflush(stdout);

        n += cmd_footer();

        return(n);
}


int     cmd_header(const char **valid_arguments) {
        int  		n = 0,i;
        const char 	*val;
	char		*path_info;
        char 		*datestr = iso8601date();

        n += printf(
                "Content-type: text/xml\r\n\r\n"
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<OAI-PMH"
                " xmlns=\"http://www.openarchives.org/OAI/2.0/\""
                " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
                " xsi:schemaLocation=\"http://www.openarchives.org/OAI/2.0/ "
                " http://www.openarchives.org/OAI/2.0/OAI-PMH.xsd\">"
                   ); fflush(stdout);

        n += printf("<responseDate>%s</responseDate>",datestr); fflush(stdout);
        n += printf("<request"); fflush(stdout);

        for (i = 0; valid_arguments[i] != NULL ; i++) {
                val = cgi_getentrystr(valid_arguments[i]);
                
                if (val != NULL) {
                        n += printf(" %s=\"%s\"", valid_arguments[i], val); fflush(stdout);
                } 
        }

	path_info = getenv("PATH_INFO");

	if (path_info == NULL) {
		path_info = "";
	}

        n += printf(">%s%s</request>",oai_progname(), path_info); fflush(stdout);

        free(datestr);

        return(n);
}

int     cmd_footer(void) {
        int n = 0;
        
        n = printf("</OAI-PMH>"); fflush(stdout);

        return(n);
}

int     cmd_identify(void) {
        const	char    *valid_arguments[] = { "verb" , NULL };
        int     	n = 0;

        if (! has_valid_keys(valid_arguments) ) return(-1);
       
        n += cmd_header(valid_arguments); 
        n += oai_identify(); 
        n += cmd_footer();

        return(n);
}

int     cmd_listmetadataformats(void) {
        const char    *valid_arguments[] = { "verb" , "identifier" , NULL };
        const char    *identifier;
	int	      n = 0;

        if (! has_valid_keys(valid_arguments) ) return(-1);

        identifier = cgi_getentrystr("identifier");

        if (identifier != NULL && ! oai_identifier_exists(identifier,NULL) ) {
                errmsg  = "No such identifier";
                errcode = ERR_IDDOESNOTEXIST;
                return(-1);
        }

	if (identifier != NULL && oai_num_metadataformats(identifier) == 0) {
		errmsg  = "No metadataformats available";
		errcode = ERR_NOMETADATAFORMATS;
		return(-1);
	}
      
	n += cmd_header(valid_arguments);
	n += oai_listmetadataformats(identifier);
	n += cmd_footer();
	
        return(n);
}

int     cmd_listsets(void) {
        const	char    *valid_arguments[] = { "verb" , "resumptionToken" , NULL };
        const char      *resumptionToken;
        int     n;

        if (! has_valid_keys(valid_arguments) ) return(-1);

        n = oai_num_of_sets();

        if (n == 0) {
                errmsg  = "No sets exists";
                errcode = ERR_NOSETHIERARCHY;
                return(-1);
        }

	resumptionToken = cgi_getentrystr("resumptionToken");

	if (resumptionToken != NULL && ! oai_valid_token(resumptionToken) ) {
		errmsg  = "Illegal resumptionToken";
		errcode = ERR_BADRESUMPTIONTOKEN;
		return(-1);
	}

	n += cmd_header(valid_arguments);
	n += oai_listsets(resumptionToken);
	n += cmd_footer();

        return(n);
}

int     cmd_list(enum list_type t) {
        /* Valid keys in case of a resumptionToken */
        const	char    *valid_arguments_res[]   = { 
                        "verb" , "resumptionToken" , NULL 
                        };
        /* Valid keys in case of no resumptionToken */
        const	char    *valid_arguments_nores[] = { 
			"verb" , "set" , "from" , "until" , "metadataPrefix" , NULL 
			};
        int     n;

        const char      *resumptionToken = NULL, *metadataPrefix = NULL;
	const char      *from = NULL, *until = NULL, *set = NULL;

        resumptionToken = cgi_getentrystr("resumptionToken");

        if (resumptionToken == NULL) {
		if (! has_valid_keys(valid_arguments_nores) ) return(-1);
	}
        else {
		if (! has_valid_keys(valid_arguments_res) ) return(-1);
	}


	if (resumptionToken == NULL) {
        	/* If we don't have a resumptionToken, then we need a metadataPrefix */
       		metadataPrefix = cgi_getentrystr("metadataPrefix");

        	if (metadataPrefix == NULL) {
                	errmsg  = "Need a metadataPrefix";
                	errcode = ERR_BADARGUMENT;
                	return(-1);
        	}

		if (! oai_valid_format(metadataPrefix) ) {
			errmsg  = "Illegal metadataPrefix";
			errcode = ERR_CANNOTDISSEMINATEFORMAT;
			return(-1);
		}
		
        	/* In case the harvester sends dates, we have to check their syntax
         	 * and granularity
         	 */
        	from  = cgi_getentrystr("from");
        	until = cgi_getentrystr("until");

        	if ( (from != NULL || until != NULL) && ! has_valid_dates(from,until) ) return(-1);


        	/* In case the harvesters sends a set, we check for number of sets 
         	 * available
         	 */
        	set = cgi_getentrystr("set");
        
        	if (set != NULL) {
                	if (oai_num_of_sets() == 0) {
                        	errmsg  = "No sets exist";
                        	errcode = ERR_NOSETHIERARCHY;
                        	return(-1);
                	}
		}
        }
	else if (! oai_valid_token(resumptionToken) ) {
		errmsg  = "Illegal resumptionToken";
		errcode = ERR_BADRESUMPTIONTOKEN;
		return(-1);
	}

	n = oai_list(set,metadataPrefix,from,until,resumptionToken,0);

	if (n == 0) {
		errmsg  = "No records match";
		errcode = ERR_NORECORDSMATCH; 
		return(-1);
	}

	n = 0;

	if (resumptionToken == NULL) {
		n += cmd_header(valid_arguments_nores);
	}
	else {
		n += cmd_header(valid_arguments_res);
	}
	n += oai_list(set,metadataPrefix,from,until,resumptionToken,t);
	n += cmd_footer();

        return(n);
}

int     cmd_getrecord(void) {
        const char    *valid_arguments[] = { "verb" , "identifier" , "metadataPrefix" , NULL };
        const char    *identifier, *metadataPrefix;
	int	      n = 0;

        if (! has_valid_keys(valid_arguments) ) return(-1);

        identifier = cgi_getentrystr("identifier");

        if (identifier == NULL) {
                errmsg  = "Need an identifier";
                errcode = ERR_BADARGUMENT;
                return(-1);
        }

        metadataPrefix = cgi_getentrystr("metadataPrefix");

        if (metadataPrefix == NULL) {
                errmsg  = "Need a metadataPrefix";
                errcode = ERR_BADARGUMENT;
                return(-1);
        }

	if (! oai_valid_format(metadataPrefix)) {
		errmsg  = "Illegal metadataPrefix";
		errcode = ERR_CANNOTDISSEMINATEFORMAT;
		return(-1);
	}

        if (! oai_identifier_exists(identifier,NULL) ) {
                errmsg  = "No such identifier";
                errcode = ERR_IDDOESNOTEXIST;
                return(-1);
        }

	if (! oai_identifier_exists(identifier,metadataPrefix) ) {
		errmsg  = "Cannot disseminate format";
		errcode = ERR_CANNOTDISSEMINATEFORMAT;
		return(-1);
	}

	n += cmd_header(valid_arguments);
	n += oai_get(identifier,metadataPrefix);
	n += cmd_footer();

        return(0);
}

/* This subroutine checks some environment variables for security
 * reasons. Returns 1 if everything is okay, returns 0 on error.
 */
int	has_secure_env(void) {
	char	*p;

	p = getenv("REQUEST_METHOD");

	errmsg  = "Bad request";
	errcode = ERR_HTTP_404;

	if (p == NULL) return 0;

	if (strlen(p) > 4) return 0;
	
	if (strcmp(p,"GET") == 0) {
		p = getenv("QUERY_STRING");

		if (p == NULL) return 0;

		if (strlen(p) > SEC_MAX_CONTENT_LENGTH) return 0;
	}
	else if (strcmp(p,"POST") == 0)	 {
		p = getenv ("CONTENT_LENGTH");

		if (p == NULL) return 0;
		
		if (strlen(p) > SEC_MAX_CONTENT_LENGTH) return 0;
	}
	else {
		return 0;
	}

	p = getenv("PATH_INFO");

	if (p != NULL) {
		if (strlen(p) > SEC_MAX_CONTENT_LENGTH) return 0;

		if (is_uri(p) == -1) return 0;
	}

	return 1;
}
                

/* This subroutine checks if all the keys in the CGI query are
 * valid, but doesn't check the content of the keys except
 * for the 'verb' key which is mandatory.
 * Returns 1 when all keys are valid, return 0 on error.
 */
int     has_valid_keys(const char **valid_keys) {
        enum  constant_type { NUM_OF_ARGUMENTS=7  }; /* The number of arguments defined in OAI-PMH */ 
	char		*val;
        const char    	*valid_verbs[] = { 
			"Identify" , "ListMetadataFormats" , 
			"ListSets" , "ListIdentifiers" , 
			"ListRecords" ,"GetRecord" , NULL 
				};
        long    n;
        int     i,found,key_count;

        /* We need at least one key
         */ 
        if (cgi_num_entries == 0) {
                errmsg  = "Need a verb";
                errcode = ERR_BADVERB;
                return(0);
        }

        /* There are very strict rules one the verb key in OAI-PMH.
         * We need to check that there is one and only one verb.
         */
        n = cgi_getnumentries("verb");

        if (n == 0) {
                errmsg  = "Need a verb";
                errcode = ERR_BADVERB;
                return(0);
        }
        else if (n > 1) {
                errmsg  = "Duplicated verbs";
                errcode = ERR_BADVERB;
                return(0);
        }

        
        /* This verb should have a valid value.
         */
	val   = (char *) cgi_getentrystr("verb");
        found = 0;
        
        for (i = 0 ; valid_verbs[i] != NULL ; i++) {
                if (strcmp(val,valid_verbs[i]) == 0) {
                        found = 1;
                        break;
                }
        }

        if (found == 0) {
                errmsg  = "Illegal verb";
                errcode = ERR_BADVERB;
                return(0);
        }

        /* The number of terms should always be equal or less than
         * the number of valid arguments 
         */
        if (cgi_num_entries > NUM_OF_ARGUMENTS) {
                errmsg  = "Illegal arguments";
                errcode = ERR_BADARGUMENT;
                return(0);
        }

        key_count = 0;

        for (i = 0 ; valid_keys[i] != NULL ; i++) {
                n = cgi_getnumentries(valid_keys[i]);

                if (n == 1) {
                       key_count++;
                }
                else if (n > 1) {
                        errmsg  = "Duplicated argument";
                        errcode = ERR_BADARGUMENT;
                        return(0);
                }
	
		val = (char *) cgi_getentrystr(valid_keys[i]);

		if (is_uri((const char *)val) == -1) {
			errmsg  = "Illegal characters in argument";
			errcode = ERR_BADARGUMENT;
			return(0);
		}
        }

        if (cgi_num_entries != key_count) {
                errmsg  = "Illegal arguments";
                errcode = ERR_BADARGUMENT;
                return(0);
        }

        return(1);
}

/* This subroutine returns 1 if both from and until have a valid
 * syntax, granularity and from is earlier than until if set.
 * The subroutine returns 0 in all other cases. 
 */
int     has_valid_dates(const char *from,const char *until) {
        struct date_struct fm,um;
        int repository_granularity;

        if (from == NULL && until == NULL) return(1);

        /* Return 0 if the from or until ar e set and we
         * are unable to parse them
         */
        if (from != NULL && parse_date(from,&fm) == -1) {
                errmsg  = "Illegal date";
                errcode = ERR_BADARGUMENT;
                return(0);
        }

        if (until != NULL && parse_date(until,&um) == -1) {
                errmsg  = "Illegal date";
                errcode = ERR_BADARGUMENT;
                return(0);
        }

        repository_granularity = oai_granularity();

        if (from != NULL && fm.granularity > repository_granularity) {
                errmsg  = "Illegal date";
                errcode = ERR_BADARGUMENT;
                return(0);
        }

        if (until != NULL && um.granularity > repository_granularity) {
                errmsg  = "Illegal granularity";
                errcode = ERR_BADARGUMENT;
                return(0);
        }

        if (from != NULL && until != NULL && fm.granularity != um.granularity) {
                errmsg  = "Illegal granularity";
                errcode = ERR_BADARGUMENT;
                return(0);
        }
        
        if (from != NULL && until != NULL && fm.tval > um.tval) {
                errmsg = "Illegal date";
                errcode = ERR_BADARGUMENT;
                return(0);
        }

        return(1);
}

int	is_uri(const char *p) {
        char *reserved = ";/?:@&=+$,";
        char *mark     = "-_.!~*'()";
        char *alphanum = "abcdefghijklmnopqrstuvwyxzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        char *hex      = "0123456789ABCDEF";

	if (p == NULL) {
		return(0); 
	}	

        while(*p++) {
                if (
                    index(reserved,*p) != NULL ||
                    index(mark,*p) != NULL ||
                    index(alphanum,*p) != NULL) {
                        /* legal char */
                }
                else if (*p == '%' && index(hex,*(p+1)) != NULL && index(hex,*(p+2)) != NULL) {
                        /* legal escape sequence */
                        p += 2;
                }
                else {
                        /* illegal char or escape sequence */
                        return(-1);
                }
        }

        return(0);
}

