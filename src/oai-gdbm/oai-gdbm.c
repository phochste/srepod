/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: oai-gdbm.c,v 1.2 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */
#include "oai-gdbm.h"
#include "http_head.h"

char    *oai_interface_version  = "oai-gdbm v0.1";

static  GDBM_FILE       dbf;

/* This function reads the static repository network
 * location from the database and copies at most
 * sz bytes it in the url. Returns 0 on success or 
 * -1 on failure.
 * The url should be big enough to store sz chars
 * plus the '\0'.
 */
static int get_url(char *url,size_t sz);

/* This function reads the last modification date of the
 * last static repository cache from the database.
 * Returns 0 on success or -1 on failure.
 */
static int get_lmf(char *lmf, size_t sz);

/* This function reads the lock status of the
 * static repository cache.
 * Returns 0 on success -1 on failure.
 */
static int get_status(int *status);	

static int get_url(char *url, size_t sz) {
        datum   key,data; 

        key.dptr  = "frm";
        key.dsize = sizeof(key.dptr) - 1;
        
        data = gdbm_fetch(dbf,key);

        if (data.dptr == NULL) {
		warn("get_url - failed to find frm key");
                return -1;
        }

	memcpy(url,data.dptr,sz > data.dsize ? data.dsize : sz);

	if (data.dsize >= sz) 
		url[sz] = '\0';
	else 
		url[data.dsize] = '\0';

        free(data.dptr);

	return 0;
}

static int get_lmf(char *lmf, size_t sz) {
	char	*databuf;
        datum   key,data; 

        key.dptr  = "lmf";
        key.dsize = sizeof(key.dptr) - 1;
        
        data = gdbm_fetch(dbf,key);

        if (data.dptr == NULL) {
		warn("get_mod - failed to find mod key");
                return -1;
        }

        memcpy(lmf, data.dptr, sz > data.dsize ? data.dsize : sz);

        if (data.dsize >= sz) 
                lmf[sz] = '\0';
        else 
                lmf[data.dsize] = '\0';

        free(data.dptr);
	
	return 0;
}

static int get_status(int *status) {
        datum   key,data; 

        key.dptr  = "lck";
        key.dsize = sizeof(key.dptr) - 1;
        
        data = gdbm_fetch(dbf,key);

        if (data.dptr == NULL) {
		warn("get_status - failed to find lck key");
                return -1;
        }

	if (sscanf(data.dptr, "%1d", status) != 1) {
		 warn("get_status - lck key doesn't contain a single digit");
		 return -1;
	}

        free(data.dptr);

	return 0;
}



int	oai_open_repository(void) {
	char	*path_info = getenv("PATH_INFO");
	char	*gwdbfile  = getenv("GWDBFILE");
	char	*gwdbconf  = getenv("GWDBCONF");
	char	*DATABASE,*CONF;	
	char	dbfile[MAX_PATH_LENGTH];
	char	url[MAX_URL_LENGTH];
	char	lmf[MAX_URL_LENGTH];
	char	*lock;
	int	db_conf = -1;	
	int	db_lock_status;
	session_struct	st;
	HS	*hs;

	/* Read the path to the configuration file from the environment of the
	 * GWDBCONF set at compile time
	 */
	if (gwdbconf != NULL) {
		CONF = gwdbconf;
	}
	else {
		CONF = GWDBCONF; 
	}

	/* Read the database file from the environment or the pathinfo
	 */
	if (path_info == NULL) {
		DATABASE = gwdbfile;
	}
	else if (CONF != NULL && strlen(CONF)) {
		db_conf = config_load(CONF);
		if (db_conf < 0)  {
			warn("failed to open %s",CONF);
			return OPEN_CONFIGURATION_ERROR;
		}
		log("oai_open_repository - trying to load config for '%s'", path_info);
		DATABASE = config_param(db_conf,path_info);
		
		/*
		 * config_free(db_conf);
		 */
	}
	else {
		warn("oai_open_repository - failed to find database file location"); 
		return OPEN_CONFIGURATION_ERROR;
	}

	if (DATABASE == NULL || strlen(DATABASE) == 0) {
		warn("oai_open_repository - no database file location defined");
		return OPEN_NO_SUCH_GATEWAY;
	}
	else {
		log("oai_open_repository - client requested %s", DATABASE);
	}

	/* Make a safe copy of the DATABASE contents.
	 * We need to do this because subsequent calls can change the
	 * contents of the DATABASE pointer.
	 */
	if (strlen(DATABASE) >= MAX_PATH_LENGTH) {
		warn("oai_open_repository - strlen(DATABASE) exceeds %d", MAX_PATH_LENGTH);
		return  OPEN_INTERNAL_ERROR;
	}

	strncpy(dbfile, DATABASE, MAX_PATH_LENGTH);

	/* Test if the database is locked */
	log("oai_open_repository - checking lock file for %s", dbfile);

	if (is_locked(dbfile) == 1) {
		log("oai_open_repository - %s is locked");

		/* Open the lock file to read the status */
		if (open_lock(dbfile, &st) == -1) {
			warn("oai_open_repository - failed to open_lock()");
			return OPEN_INTERNAL_ERROR;
		}

		if (read_lock(&st) == -1) {
			warn("oai_open_repository - failed to read_lock()");
			return OPEN_INTERNAL_ERROR;
		}

		switch(st.status) {
			case SESSION_INVALID :
				close_lock(&st);
				return OPEN_INVALID_REPOSITORY;
			case SESSION_NO_RESOURCES_LEFT :
				close_lock(&st);
				warn("oai_open_repository - !! NO RESOURCES LEFT ON SYSTEM !!");
				return OPEN_INTERNAL_ERROR;
			case SESSION_CONFIG_ERROR :
				close_lock(&st);
				warn("oai_open_repository - !! CONFIG ERROR !!");
				return OPEN_INTERNAL_ERROR;
			default :	
				close_lock(&st);
				return OPEN_GATEWAY_LOCKED;
		}
	}
	else {
		log("oai_open_repository - %s is free", dbfile);
	}

	/* Open the database
	 */
        dbf = gdbm_open((char *)dbfile, 1024, GDBM_READER, 0644, 0);

        if (dbf == NULL) {
		warn("oai_open_repository - failed to open database file '%s'", dbfile);
                return OPEN_CONFIGURATION_ERROR;
	}

	/* Read the lock status of the repository */
	if (get_status(&db_lock_status) != 0) {
		warn("oai_open_repository - failed to check status database file '%s'", dbfile);
        	gdbm_close(dbf);
		return OPEN_CONFIGURATION_ERROR;
	}


	/* Read the static repository network location from the database
	 */
	if (get_url(url, MAX_URL_LENGTH) != 0) {
		warn("oai_open_repository - failed to read static repository URL from databse");
        	gdbm_close(dbf);
		return OPEN_CONFIGURATION_ERROR;
	}

	/* Read the static repository modification time from the database
	 */
	if (get_lmf(lmf, MAX_URL_LENGTH) != 0) {
		warn("oai_open_repository - failed to read last modification time from database");
        	gdbm_close(dbf);
		return OPEN_CONFIGURATION_ERROR;
	}

	/* Fetch now header information for this static repository 
	 */
	hs = init_header();

	if (hs == NULL) {
		warn("oai_open_repository - failed to init_header()");
        	gdbm_close(dbf);
		return OPEN_CONFIGURATION_ERROR;
	}

	log("oai_open_repository - requesting status for %s at %s", dbfile, url);

	if (simple_header(url, lmf, hs) != 0) {
		free_header(hs);
		log("oai_open_repository - failed to get status");
		warn("oai_open_repository - failed to retrieve header information for %s since %s", url, lmf);
        	gdbm_close(dbf);
		return OPEN_NETWORK_ERROR;
	}

	log("oai_open_repository - received status %d", hs->status_code);

	/* If the status code is 200, then the static repository cache is not
	 * up to date. We have to retrieve a fresh one
	 */
	if (hs->status_code == 200) {
		log("oai_open_repository - database not up to date requesting lock file");
		/* Create a lock file asking the daemon for a fresh copy
		 * of the database
		 */
		if (create_lock(dbfile, url, &st) != 0) {
			warn("oai_open_repository - failed to create a lock file (already locked?)");
        		gdbm_close(dbf);
			return OPEN_GATEWAY_LOCKED;
		}

		log("oai_open_repository - lock file %s created", st.lockfile);

        	gdbm_close(dbf);
		close_lock(&st);

		return OPEN_GATEWAY_LOCKED;
	}
	/* Only a 304 should be a sign of success */
	else if (hs->status_code != 304) {
		log("oai_open_repository - database not accessible");
        	gdbm_close(dbf);
		return OPEN_ACCESS_ERROR;
	}
	else {
		log("oai_open_repository - database is up to date");
	}

	/* If the database is up to date, check if it is valid */
	if (db_lock_status > 0) {
		log("oai_open_repository - database not valid");
        	gdbm_close(dbf);
		return OPEN_INVALID_REPOSITORY;
	}

	return OPEN_SUCCESS;
}

int	oai_close_repository(void) {
        gdbm_close(dbf);
	return 0;
}

char    *oai_progname(void) {
	char 	*gwbaseurl = getenv("GWBASEURL");

	if (gwbaseurl == NULL) {
		warn("oai_progname - error no GWBASEURL set");
		return "";
	}

        return gwbaseurl;
}

int     oai_granularity(void) {
        datum   key,data; 
        int     ret;

        key.dptr  = "grn";
        key.dsize = sizeof(key.dptr) - 1;
        
        data = gdbm_fetch(dbf,key);

        if (data.dptr == NULL) {
		warn("oai_granularity - failed to find grn key");
                return -1;
        }

        if (memcmp(data.dptr,"YYYY-MM-DD",data.dsize) == 0) {
                ret = DAY;
        }
        else if (memcmp(data.dptr,"YYYY-MM-DDTHH:MM:SSZ",data.dsize) == 0) {
                ret = SECOND;
        }
        else  {
		warn("oai_granularity - grn value has unknown format");
                ret = -1;
        }

        free(data.dptr);

        return ret;
}

int     oai_num_of_sets(void) {
	/* We don't support sets */
        return 0;
}

int     oai_identifier_exists(const char *identifier, const char *metadataPrefix) {
        datum   key; 
        char    *keybuf;
        int     ret,l;

        if (identifier == NULL) {
		warn("oai_identifier_exists - recieved NULL as identifier");
                return 0;
	}

        /* If we don't have a metadataPrefix, then we can use the 'fmt:<indentifier>'
         * fields to check the existance of an identifier.
         */
        if (metadataPrefix == NULL) {
                /* 4 is the length of 'fmt:' */     
                keybuf = (char *) malloc(sizeof(char) * (strlen(identifier) + 4) + 1); 

                if (keybuf == NULL)  {
			warn("oai_identifier_exists - memory allocation error");
                        return 0;
		}
                
                l = sprintf(keybuf,"fmt:%s",identifier); 

                key.dptr  = keybuf;
                key.dsize = l;

                ret = gdbm_exists(dbf,key);

                free(keybuf);

                return ret;
        }
        /* Now we need to search the 'xml:<metadataPrefix>:<identifier>' field */
        else {
                /* 5 is the length of 'xml:' plus the extra ':' inbetween the format and id */
                keybuf = (char *) malloc(sizeof(char) * (strlen(metadataPrefix) + strlen(identifier) + 5) + 1);

                if (keybuf == NULL)  {
			warn("oai_identifier_exists - memory allocation error");
                        return 0;
		}

                l = sprintf(keybuf, "xml:%s:%s", metadataPrefix, identifier);        
               
                key.dptr  = keybuf;
                key.dsize = l;

                ret = gdbm_exists(dbf,key);

                free(keybuf);

                return ret;
        }
        
        return 0;
}

int	oai_num_metadataformats(const char *identifier) {
        datum   key,data; 
	char	*databuf;
        int     ret;

        key.dptr  = "nfm";
        key.dsize = sizeof(key.dptr) - 1;
        
        data = gdbm_fetch(dbf,key);

        if (data.dptr == NULL) {
		warn("oai_num_metadataformats - failed to find nfm key");
                return 0;
        }


	databuf = (char *) malloc(sizeof(char)*data.dsize + 1);

	if (databuf == NULL) {
		warn("oai_num_metadataformats - memory allocation error");
		free(data.dptr);
		return 0;
	}

	memcpy(databuf ,data.dptr ,data.dsize);
	
	databuf[data.dsize] = '\0';

        if (sscanf(databuf,"%2d",&ret) != 1) {
		warn("oai_num_metadataformats - nfm value has wrong format");
                return 0;
        }

	free(databuf);
        free(data.dptr);

        return ret;
}

int	oai_valid_token(const char *resumptionToken) {
        /* We don't support resumptionTokens */
	return 0;
}

int	oai_valid_format(const char *metadataPrefix) {
        datum   key; 
        char    *keybuf;
        int     ret,l;

        if (metadataPrefix == NULL) {
		warn("oai_valid_format - recieved NULL as metadataPrefix");
                return 0;
	}

        /* We test the 'fmt:<metadataPrefix>' field */

        /* 4 is the length of 'fmt:' */
        keybuf = (char *) malloc(sizeof(char) * (strlen(metadataPrefix) + 4) + 1);

        if (keybuf == NULL)  {
		warn("oai_valid_format - memory allocation error");
                return 0;
	}

        l = sprintf(keybuf, "fmt:%s", metadataPrefix);

        key.dptr  = keybuf;
        key.dsize = l;
        
        ret = gdbm_exists(dbf,key);

        free(keybuf);

	return ret;
}

int     oai_identify(void) {
	FILE	*fp;
	char    **parse_args;
	char	*gwbaseurl   = getenv("GWBASEURL");
	char 	*gwdbfriends = getenv("GWDBFRIENDS");
	char 	*gwdbdesc    = getenv("GWDBDESC");
	char	*gwadmin     = getenv("GWADMIN");
        datum   key,data; 
        int     n = 0;
	int	c;
	char	url[MAX_URL_LENGTH];

        key.dptr  = "idf";
        key.dsize = sizeof(key.dptr) - 1;
        
        data = gdbm_fetch(dbf,key);

        if (data.dptr == NULL) {
		warn("oai_identify - failed to find idf key"); 
                return 0;
        }

        n += write(STDOUT_FILENO , data.dptr , data.dsize);

        free(data.dptr);

	/* If the GWDBFRIENDS environmental variable exists, then
	 * read this file and append it to the Identify section.
	 */
	parse_args = args_template(1,NULL);
	if (gwdbfriends	!= NULL && (c = parse_template(stdout, gwdbfriends, parse_args, PARSE_LOOSE)) != -1) {
		n += c;
	}
	free(parse_args);

	/* If the GWDBDESC environmental variable exists, then
	 * read this file and append it to the Identify section.
	 */
	get_url(url,MAX_URL_LENGTH);
	parse_args = args_template(3, url, gwbaseurl, gwadmin, NULL);
	if (gwdbdesc != NULL && (c = parse_template(stdout, gwdbdesc, parse_args, PARSE_LOOSE)) != -1) {
		n += c;
	}
	free(parse_args);

	n += write(STDOUT_FILENO, "</Identify>", strlen("</Identify>"));

        return n;
}

int	oai_listsets(const char *resumptionTokens) {
	/* We don't support sets */
	return 0;
}

int	oai_listmetadataformats(const char *identifier) {
	int     n=0,l;
        datum   key,data,fmtdata; 
        char    *lh = "<ListMetadataFormats>";
        char    *lf = "</ListMetadataFormats>";
        char    *saved_ptr,*fmt,*keybuf,*databuf;
       
	if (identifier == NULL) {
		keybuf = "fmt";
	}
	else {
		/* 4 is the length of 'fmt:' */
		keybuf = (char *) malloc(sizeof(char)*(strlen(identifier) + 4) + 1);

		if (keybuf == NULL) {
			warn("oai_listmetadataformats - memory allocation error");
			return 0;
		}

		sprintf(keybuf,"fmt:%s",identifier);
	}
        
	key.dptr  = keybuf;
	key.dsize = strlen(keybuf);

        data = gdbm_fetch(dbf,key);

	if (identifier != NULL) {
		free(keybuf);
	}

        if (data.dptr == NULL) {
		warn("oai_listmetadataformats - failed to find fmt key");
                return 0;
        }

	databuf = (char *) malloc(sizeof(char)*data.dsize + 1);

	if (databuf == NULL) {
		warn("oai_listmetadataformats - memory allocation error");
		return 0;
	}

	memcpy(databuf,data.dptr,data.dsize);

	databuf[data.dsize] = '\0';

	free(data.dptr);

        n += write(STDOUT_FILENO, lh, strlen(lh));

        /* Now this is a pretty nasty part, we are going to use the strtok to parse the
         * databuf string. This is a tab delimited list of metadataPrefixes. We keep
         * a copy of the start adress to free memory at the end of this subroutine.
         */
        saved_ptr = databuf;

        fmt = strtok(databuf,"\t");

        do {
                if (fmt == NULL) {
			warn("oai_listmetadataformats - fmt value has wrong format");
                        goto err;
		}

                /* Use the fmt to fetch the key 'fmt:<fmt>' which contains the xml
                 * description for this format.
                 */

                /* 4 is the length of 'fmt:' */ 
                keybuf = (char *) malloc(sizeof(char)*(strlen(fmt) + 4) + 1);

                l = sprintf(keybuf , "fmt:%s", fmt);
               
                key.dptr  = keybuf;
                key.dsize = l;

                fmtdata = gdbm_fetch(dbf, key);

                if (fmtdata.dptr == NULL) {
			warn("oai_listmetadataformats - %s not found", keybuf);
                        goto err;
		}

                n += write(STDOUT_FILENO, fmtdata.dptr, fmtdata.dsize);

                free(fmtdata.dptr);

                free(keybuf);

        } while ((fmt = strtok(NULL,"\t")) != NULL) ;
        
        err:

        n += write(STDOUT_FILENO, lf, strlen(lf));

        free(saved_ptr);

        return n;
}

int     oai_list(const char *set,const char *metadataPrefix, const char *from, const char *until, const char *resumptionToken, int return_type) {
	int 			n=0,l;
	datum			key,nextkey,data,tmkey,tmdata;
	char			*keybuf,*idbuf,*fmtbuf,*tmbuf;
	struct  date_struct	fromstruct,untilstruct,itemstruct;

	if (return_type < 0 || return_type > 2) { 
		warn("oai_list - wrong return_type '%d'" , return_type);
		return 0;
	}

	/* We don't support resumptionTokens */
	if (resumptionToken != NULL) {
		warn("oai_list - recieved not NULL resumptionToken");
		return 0;
	}

	/* We don't support sets */
	if (set != NULL) {
		warn("oai_list - recieved not NULL set");
		return 0;
	}

	if (from  != NULL && parse_date(from,&fromstruct) != 0) {
		warn("oai_list - failed to parse from '%s'" , from);
		return 0;
	}

	if (until != NULL && parse_date(until,&untilstruct) != 0) {
		warn("oai_list - failed to parse until '%s'" , until);
		return 0;
	}

	if (return_type == 1) {
		n += write(STDOUT_FILENO, "<ListIdentifiers>", strlen("<ListIdentifiers>"));
	}
	else if (return_type == 2) {
		n += write(STDOUT_FILENO, "<ListRecords>", strlen("<ListRecords>"));
	}

	/* Just visit every item in the database */
	key = gdbm_firstkey (dbf);

	while (key.dptr) {
		nextkey = gdbm_nextkey(dbf, key);

		/* Loop until we fetch a 'xml:' key */
		if (memcmp(key.dptr,"xml:",4) != 0)  {
			free(key.dptr);
			key = nextkey;
			continue;
		}

		keybuf = (char *) malloc(sizeof(char)*(key.dsize) + 1);
		idbuf  = (char *) malloc(sizeof(char)*(key.dsize) + 1);
		fmtbuf = (char *) malloc(sizeof(char)*(key.dsize) + 1);


		if (keybuf == NULL || idbuf == NULL || fmtbuf == NULL || keybuf == NULL) {
			warn("oai_list - memory allocation error");
			free(idbuf);
			free(fmtbuf);
			free(key.dptr);
			return n;
		}

		memcpy(keybuf,key.dptr,key.dsize);

		keybuf[key.dsize] = '\0';

		if (sscanf(keybuf,"xml:%[^:]:%s",fmtbuf,idbuf) != 2)  {
			warn("oai_list - wrong key format '%s'" , key.dptr);
			free(keybuf);
			free(idbuf);
			free(fmtbuf);
			free(key.dptr);
			key = nextkey;
			continue;
		}

		free(keybuf);

		if (strcmp(metadataPrefix,fmtbuf) != 0)  {
			free(idbuf);
			free(fmtbuf);
			free(key.dptr);
			key = nextkey;
			continue;
		}

		free(fmtbuf);

		/* Fetch the datestamp of this item... */

		/* 4 is the length of 'dat:' */
		tmbuf = (char *) malloc(sizeof(char)*(4 + strlen(metadataPrefix) + 1 + strlen(idbuf)) + 1);

		if (tmbuf == NULL) {
			warn("oai_list - memory allocation error");
			free(idbuf);
			free(key.dptr);
			return n;
		}

		l = sprintf(tmbuf ,"dat:%s:%s" ,metadataPrefix ,idbuf);

		tmkey.dptr  = tmbuf;
		tmkey.dsize = l;

		tmdata = gdbm_fetch(dbf , tmkey);

		if (tmdata.dptr == NULL) {
			warn("oai_list - failed to find %s",tmbuf);
			free(idbuf);
			free(key.dptr);
			free(tmbuf);
			key = nextkey;
			continue;
		}

		free(tmbuf);

		/* Store now the data in the tmbuf */
		tmbuf = (char *) malloc(sizeof(char)*tmdata.dsize + 1);

		if (tmbuf == NULL) {
			warn("oai_list - memory allocation error");
			free(idbuf);
			free(key.dptr);
			return n;
		}

		memcpy(tmbuf,tmdata.dptr,tmdata.dsize);

		tmbuf[tmdata.dsize] = '\0';

		free(tmdata.dptr);

		if (parse_date(tmbuf,&itemstruct) != 0) {
			warn("oai_list - failed to parse %s",tmbuf);
			free(idbuf);
			free(tmbuf);
			free(key.dptr);
			key = nextkey;
			continue;
		}

		if (from != NULL || until != NULL) {
			if (  
			      /* The item has an ealier date than the from */
			      (from != NULL && fromstruct.tval   > itemstruct.tval) 

			      ||

			      /* The item has a later date than the until */

			      (until != NULL && untilstruct.tval <  itemstruct.tval)
			   ) {
				free(idbuf);
				free(tmbuf);
				free(key.dptr);
				key = nextkey;
				continue;
			}

		}

		/* At this point the item passed all checks, we can write the data
		 * to stdout (or count).
		 */
		if (return_type == 0) {
			n += 1;
		}
		else if (return_type == 1) { /* ListIdentifiers */
			n += write(STDOUT_FILENO, "<header>" ,strlen("<header>"));
			n += write(STDOUT_FILENO, "<identifier>" ,strlen("<identifier>"));
			n += write(STDOUT_FILENO, idbuf ,strlen(idbuf));
			n += write(STDOUT_FILENO, "</identifier>" ,strlen("</identifier>"));	
			n += write(STDOUT_FILENO, "<datestamp>" ,strlen("<datestamp>"));
			n += write(STDOUT_FILENO, tmbuf ,strlen(tmbuf));
			n += write(STDOUT_FILENO, "</datestamp>" ,strlen("</datestamp>"));
			n += write(STDOUT_FILENO, "</header>" ,strlen("</header>"));
		}
		else if (return_type == 2) { /* ListRecords */
			data = gdbm_fetch(dbf ,key);

			if (data.dptr != NULL) {
				n += write(STDOUT_FILENO , data.dptr ,data.dsize);
			}

			free(data.dptr);
		}

		free(idbuf);
		free(tmbuf);
		free(key.dptr);

		key = nextkey;
	}

	if (return_type == 1) {
		n += write(STDOUT_FILENO, "</ListIdentifiers>" ,strlen("</ListIdentifiers>"));
	}
	else if (return_type == 2) {
		n += write(STDOUT_FILENO, "</ListRecords>" ,strlen("</ListRecords>"));
	}

	return n;
}

int     oai_get(const char *identifier, const char *metadataPrefix) {
	int 	n=0,l;
	datum	key,data;
	char	*keybuf;

	if (identifier == NULL || metadataPrefix == NULL) {
		warn("oai_get - identifier or metadataPrefix is NULL");
		return 0;
	}

	/* 5 is the length of 'xml:' plus the ':' between the format and the identifier */
	keybuf = (char *) malloc(sizeof(char)*(strlen(metadataPrefix) + strlen(identifier) + 5) + 1);

	if (keybuf == NULL) {
		warn("oai_get - memory allocation error");
		return 0;
	}

	l = sprintf(keybuf, "xml:%s:%s", metadataPrefix, identifier);

	key.dptr  = keybuf;
	key.dsize = l;

	data = gdbm_fetch(dbf,key);

	if (data.dptr == NULL) {
		free(keybuf);
		return 0;
	}

	n += write(STDOUT_FILENO, "<GetRecord>" ,strlen("<GetRecord>"));
	n += write(STDOUT_FILENO, data.dptr, data.dsize);
	n += write(STDOUT_FILENO, "</GetRecord>" ,strlen("</GetRecord>"));

	free(keybuf);
	free(data.dptr);

	return n;
}
