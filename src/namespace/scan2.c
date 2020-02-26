/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: scan2.c,v 1.6 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */

#include "defs.h"

int		bytecount = 0;

static int	escprintf(const char *str, int len);
static int  hasAttrib(const char **atts, const char *name); 

/* Print XML escape string to the stdout. If len >= 0, then we
 * print only the first len characters, othewise we print till
 * eol.
 */
static int	escprintf(const char *str, int len) {
        int     i,n;

	n = 0;
        i = len;

        while ( (i < 0) ? *str : i-- > 0 ) {
		switch(*str) {
			case '&'  :
				n += printf("&amp;");
				break;
			case '<'  :	
				n += printf("&lt;");
				break;
			case '>'  :
				n += printf("&gt;");
				break;
			case '\'' :
				n += printf("&apos;");
				break;
			case '"'  :
				n += printf("&quot;");
				break;
			default :
				n += printf("%c",*str);
				break;
                }

                str++;
        }

	return(n);
}

static int hasAttrib(const char **atts, const char *name) {
    int     i;

    for (i = 0 ; atts[i] ; i += 2) {
        if (strcmp(atts[i], name) == 0) return 1;
    }

    return 0;
}


static void startElement(void *userData, const char *name, const char **atts) {
	parse_struct 	*parse_session = userData;
	char		    *nsuri, *prefix, *schema, schemaLocation[SCHEMALOCATIONSIZE], buffer[SCHEMALOCATIONSIZE];
	int             i,n;

    schemaLocation[0] = '\0';
    buffer[0] = '\0';

	bytecount += printf("<"); bytecount += escprintf(name,-1);

#ifdef CLEAN_REPOSITORY_ELEMENT
	/* If we hit the Repository, then we write the correct namespace
	 * declarations and schemaLocation.
	 */
	if (parse_session->depth == REPOSITORYDEPTH && 
	      strstr(name,"Repository") != NULL ) {
	      prefix = hash_val(outHash, STATIC_REPOSITORY_NAMESPACE);
	      schema = hash_val(schemaHash, STATIC_REPOSITORY_NAMESPACE);

	      if (schema != NULL && strlen(schema)) {
	      	n = strlen(STATIC_REPOSITORY_NAMESPACE) + strlen(schema) + 2;

	      	/* Drastic bail out if we have buffer overflow probs... */
			if (n + 1 > sizeof(char)*SCHEMALOCATIONSIZE || 
				n + strlen(schemaLocation) + 1 > sizeof(char)*SCHEMALOCATIONSIZE) {
				fprintf(stderr,"%s : error - namespace/schemaLocation uri size to big\n", pname);
				exit(3);
			}

	      	sprintf(buffer,"%s %s ", STATIC_REPOSITORY_NAMESPACE, schema);
			strcat(schemaLocation, buffer);
	      }

	      if (strlen(prefix)) {
	      	bytecount += printf(" xmlns:%s=\"%s\"", prefix, STATIC_REPOSITORY_NAMESPACE);
	      } else {
			bytecount += printf(" xmlns=\"%s\"", STATIC_REPOSITORY_NAMESPACE);
	      }

          prefix = hash_val(outHash, OAI_PMH_NAMESPACE);

          if (strlen(prefix)) {
            bytecount += printf(" xmlns:%s=\"%s\"", prefix, OAI_PMH_NAMESPACE);
          } else {
			bytecount += printf(" xmlns=\"%s\"", OAI_PMH_NAMESPACE);
          }

	      if (strlen(schemaLocation)) {
			bytecount += printf(" xmlns:xsi=\"%s\"", SCHEMA_NAMESPACE);
			bytecount += printf(" xsi:schemaLocation=\"%s\"", schemaLocation);
			schemaLocation[0] = '\0';
	      }

	      bytecount += printf(">");

	      parse_session->depth++;

	      return;
	}
#endif 

	/* Fill in the right declarations in the root record element */
	if (parse_session->inRecord == 1) {
		hash_next_reset(parse_session->inHash);
	
		while ((nsuri = hash_next_key(parse_session->inHash)) != NULL) {

			prefix = hash_val(outHash, nsuri);
			schema = hash_val(schemaHash, nsuri); 

			if (schema != NULL && strlen(schema)) {
				n = strlen(nsuri) + strlen(schema) + 2;
				
		      	/* Drastic bail out if we have buffer overflow probs... */
				if (n + 1 > sizeof(char)*SCHEMALOCATIONSIZE || 
					n + strlen(schemaLocation) + 1 > sizeof(char)*SCHEMALOCATIONSIZE) {
					fprintf(stderr,"%s : error - namespace/schemaLocation uri size to big\n", pname);
					exit(3);
				}
				
				sprintf(buffer,"%s %s ", nsuri, schema);
				strcat(schemaLocation, buffer);
			}

			if (strlen(prefix)) {
                sprintf(buffer, "xmlns:%s", prefix);
			} else {
                sprintf(buffer, "xmlns");
			}

            if ( ! hasAttrib(atts, buffer) ) 
               bytecount += printf(" %s=\"%s\"", buffer,nsuri);
            
		}		

		if (strlen(schemaLocation)) {
            if ( ! hasAttrib(atts, "xmlns:xsi"))
			    bytecount += printf(" xmlns:xsi=\"%s\"", SCHEMA_NAMESPACE);
            if ( ! hasAttrib(atts, "xsi:schemaLocation"))
    			bytecount += printf(" xsi:schemaLocation=\"%s\"", schemaLocation);
			schemaLocation[0] = '\0';
		}

		parse_session->inRecord = 0;
	}

	/* Fill in the right declarations in the root about element */
	if (parse_session->inAbout == 1) {
		hash_next_reset(parse_session->abHash);
	
		while ((nsuri = hash_next_key(parse_session->abHash)) != NULL) {

			prefix = hash_val(outHash, nsuri);
			schema = hash_val(schemaHash, nsuri); 

			if (schema != NULL && strlen(schema)) {
				n = strlen(nsuri) + strlen(schema) + 2;
				
		      		/* Drastic bail out if we have buffer overflow probs... */
				if (n + 1 > sizeof(char)*SCHEMALOCATIONSIZE || 
					n + strlen(schemaLocation) + 1 > sizeof(char)*SCHEMALOCATIONSIZE) {
					fprintf(stderr,"%s : error - namespace/schemaLocation uri size to big\n", pname);
					exit(3);
				}

				sprintf(buffer,"%s %s ", nsuri, schema);
				strcat(schemaLocation, buffer);
			}

			if (strlen(prefix)) {
                sprintf(buffer, "xmlns:%s", prefix);
			} else {
                sprintf(buffer, "xmlns");
			}

            if ( ! hasAttrib(atts, buffer)) 
    			bytecount += printf(" %s=\"%s\"", buffer, nsuri); 
		}		

		if (strlen(schemaLocation)) {
            if ( ! hasAttrib(atts, "xmlns:xsi"))
			    bytecount += printf(" xmlns:xsi=\"%s\"", SCHEMA_NAMESPACE);
            if ( ! hasAttrib(atts, "xsi:schemaLocation"))
			    bytecount += printf(" xsi:schemaLocation=\"%s\"", schemaLocation);
			schemaLocation[0] = '\0';
		}

		parse_session->inAbout = 0;
	}

    /* Copy all attributes found */
	for (i = 0 ; atts[i] ; i += 2) {
		bytecount += printf(" "); 
		bytecount += escprintf(atts[i],-1); 
		bytecount += printf("=\""); 
		bytecount += escprintf(atts[i+1],-1); 
		bytecount += printf("\""); 
	}

	bytecount += printf(">");

	/* If we hit a ListMetadataFormats, then we need to know which 
	 * metadataPrefix we are parsing
	 */
	if (  parse_session->depth == LISTRECORDSDEPTH && 
	      strstr(name,"ListRecords") != NULL) {

		 parse_session->inListRecords = 1;

		 /* Search for the current metadataPrefix */
		 for (i = 0 ; atts[i] ; i += 2) {
			if (strcmp(atts[i], "metadataPrefix") == 0) 
				strcpy(parse_session->metadataPrefix,atts[i+1]);
		 }

		/* Pop one item from the prefixStack, this will be a hash of
	     * record namespaces we have to process...
		 */	
		parse_session->inHash = (HASH) pop_bos_stack(prefixStack);

	 	/* Pop one item from the prefixStack, this will be a hash of
	     * about namespaces we have to process...
		 */	
		parse_session->abHash = (HASH) pop_bos_stack(prefixStack);

	}
	/* If we hit a oai:metadata, the the next elements should be from the record
	 */
	else if ( parse_session->depth == OAIMETADATADEPTH &&
		 strstr(name,"metadata") != NULL) {
		 
		 parse_session->inRecord = 1;
	}
	/* If we hit a oai:about, the the next elements should be from the about record
	 */
	else if ( parse_session->depth == OAIMETADATADEPTH &&
		 strstr(name,"about") != NULL) {
		 
		 parse_session->inAbout = 1;
	}

	parse_session->depth++;

}

static void charData(void *userData, const XML_Char *s, int len) {
	bytecount += escprintf(s,len);
}

static void commentData(void *userData, const XML_Char *s) {
	bytecount += printf("<!--"); 
	bytecount += escprintf(s,-1); 
	bytecount += printf("-->");
}

static void processingData(void *userData, const XML_Char *target, const XML_Char *data) {
	bytecount += printf("<?%s %s?>", target, data);
}

static void endElement(void *userData, const char *name) {
	parse_struct 	*parse_session = userData;

	parse_session->depth--;

	bytecount += printf("</"); 
	bytecount += escprintf(name,-1); 
	bytecount += printf(">");

	if ( parse_session->depth == LISTRECORDSDEPTH &&
	     strstr(name,"ListRecords") != NULL) {

		parse_session->inListRecords = 0;

	}
	else if ( parse_session->depth == OAIMETADATADEPTH &&
		  strstr(name,"metadata") != NULL) {

		parse_session->inRecord = 0;
	}
	else if ( parse_session->depth == OAIMETADATADEPTH &&
		  strstr(name,"about") != NULL) {

		parse_session->inAbout = 0;
	}
}

void	scan2(const char *filename) {
	FILE		*fp;
	parse_struct    parse_session;
	char            buf[BUFSIZ];
	XML_Parser      parser = XML_ParserCreate(NULL);
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
			printf(buf);
			break;
		}
		else {
			printf(buf);
		}
	}

	/* Initialize user data...
	 */
	parse_session.depth 	     = 0;
	parse_session.inListRecords  = 0;
	parse_session.inRecord 	     = 0;
	parse_session.inAbout	     = 0;

	XML_SetUserData(parser,&parse_session);
  	XML_SetElementHandler(parser, startElement, endElement);
  	XML_SetCharacterDataHandler(parser, charData);
	XML_SetCommentHandler(parser, commentData);
	XML_SetProcessingInstructionHandler(parser,processingData);

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

	fprintf(stderr,"%d bytes written\n", bytecount);
	fclose(fp);
}
