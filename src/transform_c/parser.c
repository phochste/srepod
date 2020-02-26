/* 
 * Authors: Henry Jerez <hjerez@lanl.gov>
 *
 * $Id: parser.c,v 1.7 2005/06/07 18:08:17 hochstenbach Exp $
 * 
 */
// program reads an xml file and parses it
#include <stdio.h>
#include <expat.h>
#include "stackli.h"
#include <gdbm.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#define	TMPSIZE		2*1024*1024
#define BUFFSIZE        8192
#define	GDBM_BUFF	8192
#define	ROOT_NAME	"Repository"
#define LAST_MODIFIED "%*[Ll]%*[Aa]%*[Ss]%*[Tt]-%*[Mm]%*[Oo]%*[Dd]%*[Ii]%*[Ff]%*[Ii]%*[Ee]%*[Dd]"

#define PROCESS_NONE		0
#define PROCESS_DATESTAMP	1
#define PROCESS_IDENTIFIER	2
#define PROCESS_METADATAFORMAT	3
#define PROCESS_IDENTIFY	4
#define PROCESS_METADATA	5
#define PROCESS_METADATAPREFIX	33

char dtf[11], dtl[11], *fmt, *frm, *temporary,*temporary2;
char  *DATABASE, *curr_id, *curr_pref;
FILE *tempo;
int Depth;
int in_process = 1;
int carry = 0, manda=0;
int totalfound;
int pass=0;
int insidemetadata=0;
int level = 0;
int carryon=0;
int baseu=0;
int nid,nfm;
char *intermediate;
GDBM_FILE dbf;
Stack S;  /*regular parser stack*/
Stack I;  /*identifier stack*/
Stack P;  /*metadata prefixes stack*/
Stack T;  /*temporary files names stack*/

/* Return an iso gmt time in isotime (should be big enough to store an iso time) */

/* This subroutine returns a date in iso8601 format using "Zulu" time. The
 * caller should take care of freeing the memory taken by the string.
 */
char    *iso8601date(void) {
	char            *datestr;
	time_t          t;
	struct tm       *t_struct;

	datestr = (char *) malloc(sizeof(char) * 21);

	if (datestr == NULL) {
    		return NULL;
   	}

	(void) time(&t);
	t_struct = gmtime(&t);
	(void) strftime((char *)datestr, 21, "%Y-%m-%dT%H:%M:%SZ",t_struct);
	return datestr;
}

char    *escape2(const char *str, int len) {
        int     i=0;
        char    *q,*p,*d;

        q = (char *) malloc(sizeof(char)*len+1);
        p = q;
        d= (char*)str;	
       strcpy(d,str);

        if (q == NULL) {
                return(NULL);
        }

        if (*str == 'o'&& manda==1) {
                         str++; i++;
                         if(*str=='a')
                         { str++; i++;
                           if(*str=='i'){
                                str++; i++;
                              if(*str==':'){
                                str++; i++;
                                strcpy(q,str);
                                //printf("es ahora----------%s----%d-------%s\n",str,len,q);
                                }
                            }
                         }

                }

          if (i<4){
                     strcpy(q,d);
		    // printf("es AHORRRRRAra----------%s----%d-------%s\n",str,len,q);
                }

        

        return(p);
}


char    *escape(const char *str, int len) {
        int     i;
        char    *q,*p;

        q = (char *) malloc(sizeof(char)*len*5+1);

        p = q;
        i = len;

        if (q == NULL) {
                return(NULL);
        }

        while (i-- > 0) {
                if (*str == '&') {
                        *q++ = '&';
                        *q++ = 'a';
                        *q++ = 'm';
                        *q++ = 'p';
                        *q++ = ';';
                }
                else if (*str == '<') {
                        *q++ = '&';
                        *q++ = 'l';
                        *q++ = 't';
                        *q++ = ';';
                }
                else if (*str == '>') {
                        *q++ = '&';
                        *q++ = 'g';
                        *q++ = 't';
                        *q++ = ';';
                }
                else if (*str == '\'') {
                        *q++ = '&';
                        *q++ = 'a';
                        *q++ = 'p';
                        *q++ = 'o';
                        *q++ = 's';
                        *q++ = ';';
                }
                else if (*str == '"') {
                        *q++ = '&';
                        *q++ = 'q';
                        *q++ = 'u';
                        *q++ = 'o';
                        *q++ = 't';
                        *q++ = ';';
                
		}
	
		
                else {
                        *q++ = *str;
                }

                str++;
        }

        *q++ = '\0';

        return(p);
}

static void commentData(void *userData, const XML_Char *s) {
char attrib[1024],*t;

 if (carryon==1) {
strcat(temporary2,"<!--");
t=escape(s,strlen(s));
strcat(temporary2,t);
strcat(temporary2,"-->");
free(t);
//printf("%s",temporary2);

 }
}


static void processingData(void *userData, const XML_Char *target, const XML_Char *data) {
             if (carryon==1) {
		strcat(temporary2,"<?");
                strcat(temporary2,target);
                strcat(temporary2," ");
                strcat(temporary2,data);
                strcat(temporary2,"?>");
	     }
}


//converts date strings to integers
long int numera(char texto[])       
{
      char l1[8];
      long int i,n,valor;

      n = 0;
      
      for (i=0;i<=9 ;i++){
            if (isdigit(texto[i]) != 0) {
            	l1[n++] = texto[i];
            }
      }
      
      valor = atol((char *)l1);

      return(valor);
}

int  	datadestroy(char *str) {
      	datum key;

     	key.dptr = str;
      	key.dsize = strlen(str);
      
      	gdbm_delete(dbf,key);

      	return 0;
}

char 	*dataget (char *str) {
    	char 	*out;
        datum   key,data;

#ifdef DEBUG
	printf("get %s\n", str);
#endif	

        key.dptr  = str;
        key.dsize = strlen(str);

        data = gdbm_fetch(dbf,key);

        if (data.dptr == NULL) {
                return NULL;
        }

        out = (char *) malloc(sizeof(char)*data.dsize+1);

        strncpy(out,data.dptr,data.dsize);

	out[data.dsize] = '\0';
      
	free(data.dptr);

        return out;
}



//writes the database using gdbm
int 	dataput(char *newkey, char *newkeyval)
{
    	datum key, data;

#ifdef DEBUG
	printf("put %s\n", newkey);
#endif	


    	key.dptr  = newkey;
	key.dsize = strlen(newkey);

    	data.dptr  = newkeyval;
	data.dsize = strlen(newkeyval);   

    	if (gdbm_store(dbf, key, data, GDBM_REPLACE) != 0) {
		exit(3);
    	} 

    	return(0);
}


//Deals with start of tags found on system
static void start(void *data, const char *el, const char **attr)
{       
    	int i;
	char *EL;
	char attrib[1024],*t;	
    	manda=1;

	level++;

     	EL =escape2(el,strlen(el));
        //printf("ppppp***********************************************************here is===%s\n",EL);
	
	//if (strcmp(EL, ROOT_NAME) == 0 || in_process == 0) {
        if (strstr(EL,ROOT_NAME) !=NULL || in_process == 0) {
		

		//if (strcmp(EL, ROOT_NAME) == 0) {
		if (strstr(EL,ROOT_NAME) !=NULL) {	
	    		totalfound++;
		}
    		
	//	sscanf(el,"oai:%[^\n]\n", el);
		//printf("************************************************************here is===%s\n",EL);
      		


    
  		/*handles attributes*/
  
    		//if(strcmp(EL, "ListRecords") == 0) {
          	if ((strstr(EL,"ListRecords") !=NULL) &&  insidemetadata==0){
		//printf("foundit\n");
		for (i = 0; attr[i]; i += 2) {
            			Push((char*)attr[i + 1],P);
          		}
     		}
  
  		/* WE HAVE FOUND THE DATESTAMP TAG SO WE LET THE CHARHNDLR NOW*/
		if (strcmp(EL,"datestamp")==0 && insidemetadata==0) {
			pass = PROCESS_DATESTAMP;
		}
  
  		/* WE HAVE FOUND AN IDENTIFIER TAG SO WE LET THE CHARHNDLR NOW*/
		if (strcmp(EL,"identifier")==0 && insidemetadata==0) {
      			pass = PROCESS_IDENTIFIER;
        	}


  		/* WE ARE STILL INSIDE TEH METADATAFORMAT SO WE CONTINUE TO GRAB TEH TEXT HEREIN*/
        	if (pass == PROCESS_METADATAFORMAT) {
          		strcat(temporary,"<");
          		strcat(temporary,EL);
                        
           		for (i = 0; attr[i]; i += 2) {
				t = escape(attr[i + 1], strlen(attr[i + 1]));

            			sprintf(attrib," %s=\"%s\"", attr[i], t);

				strcat(temporary, attrib);

				free(t);
            		}         
          		strcat(temporary,">");
      		}
      	
		/* We are still inside the Identify copy the content */
       		if (pass == PROCESS_IDENTIFY){
          		strcat(temporary,"<");
          		strcat(temporary,EL);

          /*set the process base url flag*/
              if (strcmp(EL,"baseURL")==0)
              {
                baseu=1;
              
                }

          
           		for (i = 0; attr[i]; i += 2) {
				t = escape(attr[i + 1], strlen(attr[i + 1]));

				sprintf(attrib," %s=\"%s\"", attr[i], t);

				strcat(temporary, attrib);

				free(t);
            		}         
          	
			strcat(temporary,">");
      		}

/* THIS IS TEH FIRST TAG AFTER METADATAFORMAT SO THIS IS A METADATA PREFIX AND I MEMORIZE IT
     		if (pass == PROCESS_METADATA){
          		strcat(temporary,EL);
          		strcat(temporary,"\t");
			pass=0;
			Push((char*)EL,P);
    		}
 */

         	/*WE JUST FOUND METADATA AND CAN START NOW TO MEMORIZE IT*/
         	if (strcmp(EL,"metadata") == 0 && insidemetadata==0) {
		    	pass = PROCESS_METADATA;
			insidemetadata=1;
        	}

      		/*WE HAVE JUST FOUND THE METADATAFORMAT SO WE INCLUDE THIS AND START RECORDING THE METADATAFORMAT RESPONSE */    
     		if (strcmp(EL,"metadataFormat") == 0 && insidemetadata==0) {
         		pass = PROCESS_METADATAFORMAT;

         		strcpy(temporary,"<");
         		strcat(temporary,EL);

            		for (i = 0; attr[i]; i += 2) {
				t = escape(attr[i + 1], strlen(attr[i + 1]));

				sprintf(attrib," %s=\"%s\"", attr[i], t);

				strcat(temporary, attrib);

				free(t);
            		}         
         	
			strcat(temporary,">");
      		}
  
       		/* WE HAVE FOUND THE METADATAPREFIX SO WE ARE INSIDE METADATA FORMAT AND LET THE SYSTEM NOW*/
    		if (strcmp(EL,"metadataPrefix")==0 && insidemetadata==0) {   
         		pass = PROCESS_METADATAPREFIX;
          		nfm++;
      		}

       		/* WE HAVE FOUND THE INDENTIFY HANDLER SO WE LET THE CHRHNDLR NOW*/
       		//if (strcmp(EL,"Identify")==0) {
          	 if (strstr(EL,"Identify") !=NULL && insidemetadata==0){
		 	pass= PROCESS_IDENTIFY;
           		strcpy(temporary,"<");
         		//strcat(temporary,EL);
			strcat(temporary,"Identify");

            		for (i = 0; attr[i]; i += 2) {
				t = escape(attr[i + 1], strlen(attr[i + 1]));

				sprintf(attrib," %s=\"%s\"", attr[i], t);

				strcat(temporary, attrib);

				free(t);
            		}  
         	
			strcat(temporary,">");
          	}

		/* If we continue to write XML */
          	if (carryon==1) {
               		strcat(temporary2,"<");
         		strcat(temporary2,EL);

            		for (i = 0; attr[i]; i += 2) {
				t = escape(attr[i + 1], strlen(attr[i + 1]));

				sprintf(attrib," %s=\"%s\"", attr[i], t);

				strcat(temporary2, attrib);

				free(t);
            		}
         	
			strcat(temporary2,">");
            	}
          
		if (strcmp(EL,"record")==0 && insidemetadata==0){
         		carryon = 1;
           
			strcpy(temporary2,"<");
         		strcat(temporary2,EL);

            		for (i = 0; attr[i]; i += 2) {
				t = escape(attr[i + 1], strlen(attr[i + 1]));

                                sprintf(attrib," %s=\"%s\"", attr[i], t);

                                strcat(temporary2, attrib);

                                free(t);
           		}  
         	
			strcat(temporary2,">");
         	}

		Push((char *)EL, S);		//places tag into stack

		in_process = 0;
    	} else {
		Depth++;
    	}
	manda=0;

	free(EL);
}



//Deals with ending tags
static void end(void *data, const char *el)
{
    	char *valor, *temp1;                 
	char *val, *EL;
	int largo;
manda=1;
  EL =escape2(el,strlen(el)); 
 // sscanf(el,"oai:%[^\n]\n", el);
      
    
	/* end of the repository, lets finish and delete the temporary files*/
     	//if (strcmp(EL,"Repository") == 0 ) {
          if(strstr(EL,"Repository") != NULL && insidemetadata==0){
		  while (Top(T) != NULL) {
               		intermediate = (char *)malloc(sizeof(char)*(strlen(Top(T))+strlen("fmt:")+1));
               		sprintf(intermediate, "fmt:%s", Top(T));
            
			val = dataget(Top(T));

               		dataput(intermediate,val);
			datadestroy(Top(T));
               
			free(val);

			Pop(T);
              	}

		level--;
		return;
         }     


    	if (in_process == 0) {
		Depth--;

      		/*wE INCLUDE THIS INSIDE THE METADATA FORMAT */
  		if (pass == PROCESS_METADATAFORMAT) {
      			strcat(temporary,"</");
         		strcat(temporary,EL);
         		strcat(temporary,">");
     		}

      		/* *FMT:<PREFIX> METADATA FORMAT IS OVER SO WE WRITE THE DATA FOR THE PREFIX*/
  		if ((strcmp(EL,"metadataFormat") == 0) &&  insidemetadata==0) {
           		pass = PROCESS_NONE;

			temp1 = (char *)malloc(sizeof(char)*strlen(Top(P))+strlen("fmt:")+1);

			if (temp1 == NULL) {
				exit(3);
			}                             

            		sprintf(temp1, "fmt:%s", Top(P));  
			dataput(temp1, temporary); 
			largo=strlen(fmt);
			fmt[largo]='\0';

		}

      		/* wE INCLUDE THIS INSIDE THE IDENTIFY RESPONSE*/
      		
		if (pass == PROCESS_IDENTIFY)
		{
		//  if (strcmp(EL,"Identify")!= 0) {
			if(strstr(EL,"Identify") != NULL){	  
      		//	strcat(temporary,"</");
         	//	//strcat(temporary,EL);
         	//	strcat(temporary,"Identify");
		//	strcat(temporary,">");
		//	
     		    }
		    else {
		    	strcat(temporary,"</"); 
			strcat(temporary,EL);
			strcat(temporary,">");
		    }
		}


 		 /* anexes xml for XML:<prefix>:<identifier>*/
  		if (carryon == 1) {
			strcat(temporary2,"</");
         		strcat(temporary2,EL);
         		strcat(temporary2,">");
		}

    
    		/* xml:<prefix>:<identifier> */
		if (strcmp(EL,"record") == 0 && insidemetadata==0) {
			carryon = 0;
    
			temp1 = (char *)malloc(sizeof(char)*(strlen(Top(P))+strlen(Top(I))+strlen("xml::")+1));

            		sprintf(temp1, "xml:%s:%s", Top(P),Top(I));  
			dataput(temp1,temporary2);
      
			/* In preparation for som realloc code */
			free(temporary2);
			temporary2 = (char *) malloc(sizeof(char)*TMPSIZE);
		}

  		//if (strcmp(EL,"Identify") == 0) {
		if(strstr(EL,"Identify") != NULL &&  insidemetadata==0){
            		pass = PROCESS_NONE;
      			dataput("idf", temporary);
		}

     	       if (strcmp(EL,"metadata") == 0 && insidemetadata==1 && level == 4) {
                	insidemetadata=0;
		}
		 
      
    		/* Is the end of the ListMetadataFormats so we enable a new stack for the prefixes on each record*/
		// (strcmp(EL,"ListMetadataFormats")==0 ) {
		if(strstr(EL,"ListMetadataFormats") != NULL &&  insidemetadata==0 ) {
			DisposeStack(P);
			P = CreateStack();
		}

    		/* metadata for this record is over */
		//if (strcmp(EL,"Listrecords")==0) {
		if (strstr(EL,"Listrecords") != NULL &&  insidemetadata==0){	
             		Pop(P);
		}
    	
		if (strcmp(Top(S), ROOT_NAME) == 0 && insidemetadata==0) {
	    		in_process = 1;
		}
	
		Pop(S);
    	}
	free(EL);
	manda=0;

	level--;
}


//Deals With characters inside the files
static void charhnld(void *userData, const XML_Char * s, int len)
{
    FILE *out;
    char *linea, *resultadochar, *temp1;
    const char *vale;
    char *tempbu;
    int  i,changeres;
    char *v1,*v2;

    
   
    if (in_process == 0) {
	   /* oai:datestamp */
	   if (pass == PROCESS_DATESTAMP) {
	            linea = (char *) malloc(sizeof(char)*(len) + 1);
                   
		    if (linea==NULL) {
		    	exit(3);
	            }
              
		    strncpy(linea,s, len);
	      	    linea[len] = '\0';

                    if (numera(dtl) < numera(linea)) {
		    	strcpy(dtl,linea);	
		    }

	            if (numera(dtf) > numera(linea)) {
		         strcpy(dtf,linea);			
		    }

                    /*Writes Dat:prefix:identifier*/
                    temp1 = (char *)malloc(sizeof(char)*(strlen(Top(I))+strlen(Top(P))+strlen("dat::")+1));
                    sprintf(temp1, "dat:%s:%s", Top(P),Top(I));
		    dataput(temp1,linea);

                    pass = PROCESS_NONE;
		  
  		    free(linea);
  		    free(temp1);
	    }
      
	    /* oai:identifier */

      	    if (pass == PROCESS_IDENTIFIER) {
		    linea=escape(s,len);		
	            Push(linea,I);
            

                    /*will put into temporary field in database*/
		    v1 = dataget(linea);
                    if (v1 != NULL) {
			    	v2 = Top(P);

               			intermediate = (char *) malloc(strlen(v1) + strlen(v2) + strlen("\t") + 1);
                  		
				if (intermediate == NULL) {
	                 		fprintf(stderr, "memory not allocated\n");
		                	exit(3);
	                    	}
				
                  		strcpy(intermediate,v1);
                  		strcat(intermediate,"\t");
                 		strcat(intermediate,v2);

                  		dataput(linea,intermediate);
                  
                  		free(v1);    
				free(intermediate);
		    } 
               	    else {
			    	nid++;
                 		Push(linea,T);
                 		dataput(linea,Top(P));
                    }

		    pass = PROCESS_NONE;

		    free(linea); 
	     }

	    /* oai:metadataFormat */
      	    if (pass == PROCESS_METADATAFORMAT){
                 	/*linea = (char *) malloc(sizeof(char)*(len) + 1);
              
	         	if (linea == NULL) {
	               		exit(3);
	         	}
            
	    	 	strncpy(linea, s, len);
                                                                  
		 	linea[len] = '\0';*/
		 	linea=escape(s,len);
                 	strcat(temporary,linea);
         
	 		free(linea);
            }
	
	    /* oai:metadataPrefix INSIDE METADATA FORMAT */
      	    if (pass == PROCESS_METADATAPREFIX){
                 /*linea = (char *) malloc(sizeof(char)*(len) + 1);
              
	         if (linea == NULL) {
	                exit(3);
	         }

                 strncpy(linea, s, len);
			
		 linea[len] = '\0';*/
		 linea=escape(s,len);

                 Push(linea ,P);
           
	         strcat(fmt,linea);
                 strncat(temporary,s,len);
                 strcat(fmt,"\t");
          
	         pass = PROCESS_METADATAFORMAT;

		 free(linea);
           }

  	   /* Identify*/
      	   if (pass == PROCESS_IDENTIFY){
                 

           if (baseu != 1)
           {

	   	linea = escape(s, len);

            }
            else //process the base url
            {
              tempbu=getenv("GWBASEURL");
              
                      linea = (char *) malloc(sizeof(char)*( strlen(tempbu) +strlen(frm) + 1 - 7));
                        if (linea == NULL) {
	                        exit(3);
			}
                          sprintf(linea,"%s/%s",tempbu,(frm+7));
			    
			  //linea[len] = '\0';
			  baseu=0;
                                               
             	 

              }

		
                 strcat(temporary,linea);
	 	 free(linea);
           }    

	   /*xml*/
	   if (carryon==1) {  
		 //linea = (char *) malloc(sizeof(char)*(len) + 1);
              
	         //if (linea == NULL) {
	           //    exit(3);
	         //}

           	 //strncpy(linea,s, len);
		 //linea[len] = '\0';
		 linea = escape(s, len);
		 strcat(temporary2,linea);

        	 free(linea);                                
	   }
     }
}


int localiza(char *filename)
{
    FILE *fp;
    char Buff[BUFFSIZE];
    char LMF[BUFFSIZE];
    int len;
    int done = 0;
    XML_Parser p;

    /* Number of tags found */
    totalfound = 0;
    
    /* Tags stack */
    S = CreateStack();

    /* Record identifier stack */ 
    I = CreateStack();

    /* MetadataPrefix stack */
    P = CreateStack();
    T = CreateStack();

    p = XML_ParserCreate(NULL);

    if (!p) {
        fprintf(stderr, "Couldn't allocate memory for parser\n");
        return -1;
    }

    XML_SetElementHandler(p, start, end);
    XML_SetCharacterDataHandler(p, charhnld);
    XML_SetCommentHandler(p, commentData);
    XML_SetProcessingInstructionHandler(p,processingData);
    
    if (strcmp(filename, "-") == 0) {
	 fp = stdin;
    }
        else {
        fp = fopen(filename, "r");
		if (fp == NULL) {
                 fprintf(stderr, "fopen error\n");
                  return -1;
		 }
	     }
	

    

    /* Ignore some parameter definitions at the beginning of the XML file */
    while(fgets(Buff,BUFFSIZE, fp) != NULL) {
	    if (sscanf(Buff, LAST_MODIFIED ": %[^\r\n]\r\n", LMF) == 1) {
      			dataput("lmf",LMF);
      	    }

	    if (strcmp(Buff,"\r\n") == 0) {
		    break;
	    }
      
    }
   
    /* Now read the XML */
    while(fgets(Buff,BUFFSIZE, fp) != NULL) {
            len = strlen(Buff);

	    	if (feof(fp)) done = 1;

    	    if (!XML_Parse(p, Buff, len, done)) {
          	fprintf(stderr, "Parse error at line %d:\n%s\n",XML_GetCurrentLineNumber(p),XML_ErrorString(XML_GetErrorCode(p)));
	        return -2;
	    }
    }
    
    fclose(fp);

    DisposeStack(S);
    DisposeStack(I);
    DisposeStack(P);
    DisposeStack(T);
   
    return 0;
}

//MAIN PROCEDURE 
int main(int argc, char *argv[])
{
    char   *filename;
    int    nst,lck;
    char    *grn, *set, tiempo[32], *crtt;
    time_t  unix_time;

    if (getenv("GWBASEURL") == NULL) {
	    fprintf(stderr,"error - no GWBASEURL set\n");
	    exit(1);
    }
    
    if (argc != 4) {
	    fprintf(stderr,"usage: %s xmlfile gdbmfile url\n", argv[0]);
	    exit(1);
    }

    filename = argv[1];
    DATABASE = argv[2];
    frm      = argv[3];

    unlink(DATABASE);
    dbf = gdbm_open(DATABASE, GDBM_BUFF, GDBM_WRCREAT, 0644, 0);

    if (dbf == NULL) {
	fprintf(stderr, "gddm_open error\n");
	exit(3);
    }
    
    dataput("frm",frm);

    // FoM = 2;
    nid = 0;	
    
    // ISO TIME HERE
    crtt=iso8601date();
   
    dataput("crt",crtt);
    free(crtt);

    time(&unix_time);
    sprintf(tiempo,"%d",unix_time);
    dataput("mod",tiempo);


    grn = "YYYY-MM-DD";
    dataput("grn",grn);

    nst = 0;
    sprintf(tiempo,"%d",nst); 
    dataput ("nst", tiempo);

    set = "";
    dataput("set", set);

    lck=0;
    sprintf(tiempo,"%d",lck);
    dataput("lck",tiempo);
    /* Initialize the last and first dates */
    strcpy(dtf,"3000-00-00"); 
    strcpy(dtl,"0000-00-00");
    
    fmt         = (char *) malloc(sizeof(char)*1024);
    temporary   = (char *) malloc(sizeof(char)*BUFFSIZE);
    temporary2  = (char *) malloc(sizeof(char)*TMPSIZE);

    if (localiza(filename) != 0) {   
	    fprintf(stderr,"%s : localiza failed\n", argv[0]);
	    exit(2);
    }

    dataput("fmt",fmt);
    dataput("dtf",dtf);
    dataput("dtl",dtl);

    sprintf(tiempo, "%d", nid);
    dataput("nid", tiempo);

    sprintf(tiempo, "%d", nfm);
    dataput("nfm", tiempo);

/*
 * printf("TOTAL FOUND dtf es:%s,y dtl es:%s, El nid es:%d, el fmt es:%s, el defnitive es\n",dtf,dtl,nid,fmt);
 */

    return(0);
}
 
