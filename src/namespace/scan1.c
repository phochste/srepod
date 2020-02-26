/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: scan1.c,v 1.9 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */

#include "defs.h"

static 	void parseSchemaLocation(const char *schemaLocation);
static 	void parseExpandedName(const char *ename, char* uri, char *lname, char *prefix);

static void startElement(void *userData, const char *name, const char **atts) {
  	int 		i;
	char		prefix[PREFIXSIZE],lname[NAMESIZE],uri[URISIZE];
	parse_struct 	*parse_session = userData;
#ifdef XSI_TYPE_HACK
    char        *p, *key, *val;
#endif

	/* Parse the schemaLocations you find outside the records...
	 */
	if (parse_session->inRecord == 0 && parse_session->inAbout == 0) {
		for (i = 0 ; atts[i] ; i += 2) {
			parseExpandedName(atts[i],uri,lname,prefix);

			if (strcmp(lname,"schemaLocation") == 0) {
				parseSchemaLocation(atts[i+1]);
			}
		}
	}

	parseExpandedName(name,uri,lname,prefix);

	/* Store URI's of namespaces *used* inside records...
	 */
	if (parse_session->inRecord == 1) {
		if (prefix == NULL || prefix[0] == '\0') 
			hash_add(parse_session->inHash,uri,"");			 
		else
			hash_add(parse_session->inHash,uri,prefix);
#ifdef XSI_TYPE_HACK
        /**
        * Inside records we could find xsi:type declarations which
        * could contain declared prefixes...
        */
        for (i = 0 ; atts[i] ; i += 2) {
            parseExpandedName(atts[i],uri,lname,prefix);
            if (strcmp(uri, SCHEMA_NAMESPACE) == 0 && strcmp(lname, "type") == 0) {
               /**
               * Now, try to find the uri for this prefix
               */
               fprintf(stderr, "Found xsi:type %s\n" , atts[i+1]);
               p = strtok(atts[i+1], ":");
               if (p != NULL) {
                    hash_next_reset(outHash);
		            while ((key = hash_next_key(outHash)) != NULL) {
                        val = hash_val(outHash,key);
			            if (strcmp(p, val) == 0) {
                            fprintf(stderr, " %s=\"%s\"\n", val, key);
				            hash_add(parse_session->inHash, key, val);
                        }
                    }
               }
           }
        }
#endif
	}

	/* Store URI's of namespaces *used* inside about containers...
	 */
	if (parse_session->inAbout == 1) {
		if (prefix == NULL || prefix[0] == '\0') 
			hash_add(parse_session->abHash,uri,"");			 
		else
			hash_add(parse_session->abHash,uri,prefix);
	}

	/* If we hit a ListRecords, then we need to know which 
	 * metadataPrefix we are parsing
	 */
	if (  parse_session->depth == LISTRECORDSDEPTH && 
	      strcmp(uri,STATIC_REPOSITORY_NAMESPACE) == 0 && 
	      strcmp(lname,"ListRecords") == 0) {

		 parse_session->inListRecords = 1;

         parse_session->metadataPrefix[0] = 0;

		 /* Search for the current metadataPrefix */
		 for (i = 0 ; atts[i] ; i += 2) {
			parseExpandedName(atts[i],uri,lname,prefix);

			if (strcmp(atts[i], "metadataPrefix") == 0 || strcmp(lname, "metadataPrefix") == 0)  {
				
				/* Drastic bail out if we have buffer overflow probs... */
				if (strlen(atts[i+1]) + 1 > sizeof(char)*PREFIXSIZE) {
					fprintf(stderr, "%s : error - metadataPrefix size to big\n", pname);
					exit(3);
				}
				strcpy(parse_session->metadataPrefix, atts[i+1]);
			}
		 }

         /* Check if we found the metadata prefix */
         if (! parse_session->metadataPrefix[0] ) {
             fprintf(stderr, "%s - ListRecords needs metadataPrefix\n", pname);
             exit(3);
         }

		 parse_session->inHash  = hash_new();
		 parse_session->abHash  = hash_new();
	}
	/* If we hit a oai:metadata, then the next elements should be from the record
	 */
	else if ( parse_session->depth == OAIMETADATADEPTH &&
		 strcmp(uri,OAI_PMH_NAMESPACE) == 0 &&
		 strcmp(lname,"metadata") == 0) {
		 
		 parse_session->inRecord = 1;
	}
	/* If we hit a oai:metadata, then the next elements should be from the record
	 */
	else if ( parse_session->depth == OAIMETADATADEPTH &&
		 strcmp(uri,OAI_PMH_NAMESPACE) == 0 &&
		 strcmp(lname,"about") == 0) {
		 
		 parse_session->inAbout = 1;
	}


	parse_session->depth++;
}

static void charData(void *userData, const XML_Char *s, int len) {
}

static void endElement(void *userData, const char *name) {
	char		prefix[PREFIXSIZE],lname[NAMESIZE],uri[URISIZE];
	char		*key,*val;
	HASH		nshash;
	parse_struct 	*parse_session = userData;

	parse_session->depth--;

	parseExpandedName(name,uri,lname,prefix);

	if ( parse_session->depth == LISTRECORDSDEPTH &&
	     strcmp(uri,STATIC_REPOSITORY_NAMESPACE) == 0 &&
	     strcmp(lname,"ListRecords") == 0) {

		parse_session->inListRecords = 0;

	        /*--------------------------------------------------------------
		 * Here we can figure out which namespace declarations should be
		 * used in the record element
		 */
		nshash = hash_new();

        fprintf(stderr, "namespaces used in '%s'-type records:\n", parse_session->metadataPrefix);

		hash_next_reset(parse_session->inHash);
		while ((key = hash_next_key(parse_session->inHash)) != NULL) {

			val = hash_val(outHash,key);	
			
			if (val != NULL) {
				hash_add(nshash,key,val);

				fprintf(stderr," %s=\"%s\"\n", val, key);
			}
		}

		/*--------------------------------------------------------------
                 */

		hash_free(parse_session->inHash);
		
		/*
		 * Push the results on the prefixStack.
		 */
		push_stack(prefixStack,(int) nshash);

	        /*--------------------------------------------------------------
		 * Here we can figure out which namespace declarations should be
		 * used in the about element
		 */
		nshash = hash_new();

        fprintf(stderr, "namespaces used in '%s'-type abouts:\n", parse_session->metadataPrefix);

		hash_next_reset(parse_session->abHash);
		while ((key = hash_next_key(parse_session->abHash)) != NULL) {

			val = hash_val(outHash,key);	
			
			if (val != NULL) {
				hash_add(nshash,key,val);

				fprintf(stderr," %s=\"%s\"\n", val, key);
			}
		}

		/*--------------------------------------------------------------
                 */

		hash_free(parse_session->abHash);
		
		/*
		 * Push the results on the prefixStack.
		 */
		push_stack(prefixStack,(int) nshash);

	}
	else if ( parse_session->depth == OAIMETADATADEPTH &&
		  strcmp(uri,OAI_PMH_NAMESPACE) == 0 &&
		  strcmp(lname,"metadata") == 0) {

		parse_session->inRecord = 0;
	}
	else if ( parse_session->depth == OAIMETADATADEPTH &&
		  strcmp(uri,OAI_PMH_NAMESPACE) == 0 &&
		  strcmp(lname,"about") == 0) {

		parse_session->inAbout = 0;
	}
}

static	void startNamespaceDecl(void *userData, const XML_Char *prefix, const XML_Char *uri) {
        parse_struct    *parse_session = userData;

	/* Handle empty namespace declaration
	 */
	if (uri == NULL || strlen(uri) == 0) {
		fprintf(stderr, "error: can't handle empty namespace declarations\n");
		exit(2);
	}

	/* Store URI's of namespaces defined outside the records...
	 */
	if (parse_session->inRecord == 0 && parse_session->inAbout == 0) {
		if (prefix == NULL) 
			hash_add(outHash,uri,"");			 
		else
			hash_add(outHash,uri,prefix);
	}
}

static	void endNamespaceDecl(void *userData, const XML_Char *prefix) {
        parse_struct    *parse_session = userData;
}

/*
 * Given a schemaLocation..fill in the schemaHash
 */
static 	void parseSchemaLocation(const char *schemaLocation) {
	char	nsuri[URISIZE],location[LOCATIONSIZE];
	char	*p, *copy;
	int	fl = -1;

	copy = (char *) malloc(sizeof(char)*strlen(schemaLocation) + 1);

	if (copy == NULL) {
		fprintf(stderr,"memory allocation error\n");
		exit(2);
	}

	strcpy(copy,schemaLocation);

	p = strtok(copy, " ");

	while (p != NULL) {
		if (fl == -1) {
			if (strlen(p) == 0) {
				/* In case we found spaces at the beginning */
				p = strtok(NULL," ");
				continue;
			}

			fl = 0;
		}
		
		if (fl == 0) {
			/* Drastic bail out if we have buffer overflow probs... */
			if (strlen(p) + 1 > sizeof(char)*URISIZE) {
				fprintf(stderr,"%s : error - namespace uri size to big\n", pname);		
				exit(3);
			}

			strcpy(nsuri,p);
			fl = 1;
		}	
		else if (fl == 1) {
			/* Drastic bail out if we have buffer overflow probs... */
			if (strlen(p) + 1 > sizeof(char)*LOCATIONSIZE) {
				fprintf(stderr,"%s : error - schema location uri size to big\n", pname);		
				exit(3);
			}

			strcpy(location,p);
			fl = 0;

			hash_add(schemaHash,nsuri,location);
		}

		p = strtok(NULL," ");
	}

	free(copy);
}

/*
 * Given an expanded name..fill in the uri, local name and prefix
 */
static 	void parseExpandedName(const char *ename, char *uri, char *lname , char *prefix) {
	int 	fl = 0;
	char	c, *uri_ptr, *prefix_ptr, *lname_ptr;

	uri_ptr   = uri;
	lname_ptr = lname;

	while(*ename) {
		c = *ename++;

		if (c == SEPARATOR) {
			fl++;
			continue;
		}

		switch(fl) {
			case 0:
				/* Drastic bail out if we have buffer overflow probs... 
			         * The + 2 = current char + \0	
				 */
				if (uri - uri_ptr + 2 > sizeof(char)*URISIZE) {
					fprintf(stderr,"%s : error - namespace uri size to big\n", pname);		
					exit(3);
				}
				*uri++ = c;
				break;
			case 1:
				/* Drastic bail out if we have buffer overflow probs... 
				 * The + 2 = current char + \0
				 */
				if (lname - lname_ptr + 2 > sizeof(char)*NAMESIZE) {
					fprintf(stderr,"%s : error - element name size to big\n", pname);		
					exit(3);
				}
				*lname++ = c;
				break;
			default:
				return;
		}
	}

	*uri++   = '\0';
	*lname++ = '\0';

	strcpy(prefix,"");

	/*
	 * Search the right prefix using the outHash
	 */
	if (uri_ptr[0] != '\0') { 
		prefix_ptr = hash_val(outHash,uri_ptr);

		if (prefix_ptr != NULL) {
			/* Drastic bail out if we have buffer overflow probs... */
			if (strlen(prefix_ptr) + 1 > PREFIXSIZE) {
				fprintf(stderr,"%s : error - namespace prefix size to big\n", pname);
				exit(3);
			}
			strcpy(prefix,prefix_ptr);
		}
	}
}

void	scan1(const char *filename) {
	FILE		*fp;
	parse_struct    parse_session;
	char            buf[BUFSIZ];
	XML_Parser      parser = XML_ParserCreateNS(NULL,SEPARATOR);
	int 		done;

	fp = fopen(filename,"r");
	
	if (fp == NULL) {
                fprintf(stderr,"failed to open %s for reading\n", filename);
                exit(2);
        }

	/* Ignore some parameter definitions at the beginning of the XML file */
	while(fgets(buf,BUFSIZ, fp) != NULL) {
		if (strncmp(buf, "<?xml", 5) == 0) {
			rewind(fp);
			break;
		}
		else if (strncmp(buf , "\r\n", 2) == 0) {
			break;
		}
	}

	/* Initialize user data...
	 */
	parse_session.depth 	     = 0;
	parse_session.inListRecords  = 0;
	parse_session.inRecord 	     = 0;
	parse_session.inAbout        = 0;

	XML_SetUserData(parser,&parse_session);
  	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetNamespaceDeclHandler(parser, startNamespaceDecl,  endNamespaceDecl);

  	do {
    		size_t len = fread(buf, 1, sizeof(buf), fp);
    		done = len < sizeof(buf);
    		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
      			fprintf(stderr, "%s at line %d\n",
              			XML_ErrorString(XML_GetErrorCode(parser)),
              			XML_GetCurrentLineNumber(parser));
      			exit(2);
    		}
  	} while (!done);
  
  	XML_ParserFree(parser);

	fclose(fp);
}
