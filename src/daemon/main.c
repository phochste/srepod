/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: main.c,v 1.3 2006/12/19 08:33:14 hochstenbach Exp $
 * 
 */
#include "defs.h"

/* Prototypes */
static int	daemon_start(void);
static void	loop(void);
static void	print_request(void);
static request_struct	*get_request(session_struct session);
static int	free_request(request_struct *);
static int	spawn_child(request_struct *);

static int	set_status(pid_t pid, enum request_status s);
static int	check_status(void);

static int	is_file(char *filename);

int		write_pid(void);
int		clean_pid(void);

typedef void	Sigfunc(int);
Sigfunc 	*my_signal(int signo, Sigfunc *func);
static	void   	sig_handler(int signo);

/* Globals */
static int		     do_shutdown = 0;
static time_t  		     boot_time;
static char		     *pname;
static request_struct 	     *requests[MAX_REQUESTS];

int	main(int argc, char *argv[]) {

	time(&boot_time);	

	pname = argv[0];

	if ((config = config_load("srepod.conf")) == -1 ) {
		fprintf(stderr,"error - can't load srepod.conf\n");
		exit(1);
	}

	if (daemon_start() != 0) {
		exit(1);
	}

	loop(); /* Never returns */

	return(0);
}

static void loop(void) {
	int	pending;
	request_struct	*r;
	session_struct	session;
	
	debug("loop - entering for loop");
	for ( ; ; ) {
		/* Check status of pending requests */
		pending = check_status();

		if (do_shutdown) {
			if (pending == 0) {
				close_pid();
				log("loop - shutdown requested : honored");
				exit(0);
			}
			else {
				log("loop - shutdown requested : not honored - %d pending requests", pending);
			}
		}

		/* Find the next lock file to process */
		if (next_lock(&session, SESSION_UNPROCESSED_TYPES) == 0) {
			log("loop - found lock %s" , session.lockfile);
		
			/* Try to find a free slot to process this file */
			r = get_request(session);

			if (r == NULL) {
				log("loop - all request slots are occupied going to sleep");
				close_lock(&session);
				sleep(SLEEP_TIME);
				continue;
			}

			/* This function is only active in DEBUG mode */
			print_request();

			if (spawn_child(r) == -1) {
				warn("loop - failed to spawn_child");

				/* Close the connection to the lock file but don't delete it */
				debug("loop - closing the lock file");
				close_lock(&session);

				free_request(r);

				warn("loop - going to sleep");
				sleep(SLEEP_TIME);
				continue;
			}
		}
		else {
			debug("loop - no locks found going to sleep");
			sleep(SLEEP_TIME);
			continue;
		}
	}
}

/* Check all requests and check the status field.
 */
static int check_status(void) {
	int i;
	int pending = 0;

	debug("check_status - checking all pending requests");

	for (i = 0 ; i < MAX_REQUESTS ; i++) {
		if (requests[i] == NULL)
			continue;

		pending++;

		/* If the request finished and exited successfully,
		 * then close the lock and delete the lock file from disk
		 */
		if (requests[i]->status == REQUEST_FINISHED) {
			log("check_status - slot %d has finished successfully", i);
			log("check_status - releasing lock");
			release_lock(requests[i]->session);
			free_request(requests[i]);
			requests[i] = NULL;
			pending--;
			continue;
		}

		/* If the request finished but failed,
		 * then close the lock and delete the lock file from disk
		 */
		if (requests[i]->status == REQUEST_FAILED) {
			log("check_status - slot %d has finished, but failed", i);
			log("check_status - closing lock");
			release_lock(requests[i]->session);
			free_request(requests[i]);
			requests[i] = NULL;
			pending--;
			continue;
		}
	}

	return(pending);
}

/* Set the status of the request with the specified pid to s 
 */
static int set_status(pid_t pid, enum request_status s) {
	int i;

	for (i = 0 ; i < MAX_REQUESTS ; i++) {
		if (requests[i] == NULL)
			continue;

		if (requests[i]->pid == pid) {
			requests[i]->status = s;
			return(0);
		}
	}

	return(-1);
}


/* Fork of a child to process the query. Return 0 on success -1 on failure
 */
static int spawn_child(request_struct  *r) {
	char  *gwspool = getenv("GWSPOOL");
	pid_t childpid;
	uid_t uid;
	gid_t gid;

	debug("spawn_child - trying to fork of a child to process the request");

	if ( (childpid = fork()) == -1) {
		warn("spawn_child - fork failed");
		return(-1);
	}
	else if (childpid == 0) {
		/* child process */
		if (chdir(gwspool) != 0) {
			warn("spawn_child - failed to chdir(%s) : %s", gwspool, strerror(errno));
			_exit(2);
		}
		
		/* Set the user and group id's of the child */
		uid = atoi(config_param(config, "uid"));
		gid = atoi(config_param(config, "gid"));

		if ((int) gid >= 0 && setgid(gid) == -1) {
			warn("spawn_child - failed to setgid(%d) : %s", gid, strerror(errno));
			_exit(2);
		}

		if ((int) uid >= 0 && setuid(uid) == -1) {
			warn("spawn_child - failed to setuid(%d) : %s", uid, strerror(errno));
			_exit(2);
		}

		if (process_request(r) == -1) {

			/* If the database exists we can clear the lock file,
			 * otherwise we keep it on disk.
			 */
			if (is_file(r->session->database) == 0) {
				log("spawn_child - deleting the lock file");
				release_lock(r->session);
			}

			warn("spawn_child - failed to process_request");
			_exit(1);
		}
		_exit(0);
	}
	else {
		/* parent process */
		r->pid = childpid;

		log("spawn_child - spawned child process with pid %d", childpid);

		return(0);
	}

	return(0);
}

/* This function prints the status of all requests to the log file.
 */
static void print_request(void) {
	int i;
	for (i = 0 ; i < MAX_REQUESTS ; i++) {
		if (requests[i] == NULL) 
			continue;
		log("slot[%d] : pid = %d", i, requests[i]->pid);
		log("slot[%d] : status = %d", i, requests[i]->status);
		log("slot[%d] : session->lockfile = %s" , i, requests[i]->session->lockfile);
		log("slot[%d] : session->database = %s" , i, requests[i]->session->database);
		log("slot[%d] : session->url      = %s" , i, requests[i]->session->url);
		log("slot[%d] : session->status   = %d" , i, requests[i]->session->status);
		log("slot[%d] : session->fd       = %d" , i, requests[i]->session->fd);
		log("slot[%d] : session->modified = %ld", i, requests[i]->session->modified);
	}
}

/* This function returns the first free position found in the
 * requests array. Returns a pointer to a request_struct on 
 * success or -1 on error
 */
static request_struct *get_request(session_struct session) {
	int i;

	debug("get_request - trying to allocate a free slot");

	for (i = 0 ; i < MAX_REQUESTS ; i++) {
		if (requests[i] == NULL) {
			requests[i] = (request_struct *) malloc(sizeof(request_struct));

			if (requests[i] == NULL) {
				warn("get_request - memory allocation error");
				return(NULL);
			}

			requests[i]->session = (session_struct *) malloc(sizeof(session_struct));

			if (requests[i]->session == NULL) {
				warn("get_request - memory allocation error");
				free(requests[i]);
				return(NULL);
			}

			log("get_request - allocating request slot %d", i);

			requests[i]->pid    = -1;
			requests[i]->status = REQUEST_PENDING;

			memcpy(requests[i]->session,&session,sizeof(session_struct));

			return requests[i];
		}
	}

	debug("get_request - failed to find a free slot");
	return(NULL);
}

/* This function deletes an entry in the requests array. Returns
 * 0 on success or -1 on failure
 */
static int free_request(request_struct *r) {
	free(r->session);
	free(r);
	return(0);
}

/* This function returns 0 if the filename exists on disk or
 * -1 otherwise.
 */
static int is_file(char *filename) {
	struct stat buf;

	if (stat(filename, &buf) == 0) {
		return(0);
	}
	else {
		return(-1);
	}
}

static int daemon_start(void) {
	FILE	*fh;
	pid_t	pid;
	char	*gwspool = getenv("GWSPOOL");
	char 	*gwhome  = getenv("GWHOME");
	char	logfile[MAX_PATH_LENGTH];
	uid_t	uid;
	gid_t	gid;

	if (gwspool == NULL) {
		fprintf(stderr,"%s : daemon_start failed - no GWSPOOL set\n", pname);
		return(-1);
	}

	/* Needed in the child code... */
	if (gwhome == NULL) {
		fprintf(stderr,"%s : daemon_start failed - no GWHOME set\n", pname);
		return(-1);
	}


	/* Test of the effective user id from the configuration files and 
	 * the current user id are the same, otherwise we need root privileges
	 * to run this daemon
	 */
	uid = atoi(config_param(config,"uid"));	
	gid = atoi(config_param(config,"gid"));

	fprintf(stderr, "%s : requesting uid = %d gid = %d\n", pname, uid, gid);

	if (( (int) uid >= 0 && uid != getuid()) || ( (int) gid >= 0 && gid != getgid())) {
		if (geteuid() != 0) {
			fprintf(stderr,"%s : daemon_start failed - need root privileges\n", pname);
			return(-1);
		}
	}

	if (SERVER_FORK) {
                switch (pid = fork()) {
                	case -1:
                        	fprintf(stderr,"%s : fork failed\n", pname);
                        	return(-1);
                	case 0:
                        	break;                  /* child becomes server */
                	default:
				fprintf(stderr, "repod : %d\n", pid);
                        	sleep(1);
                       	exit(0);                       /* kill parent */
                }

        }


	/* Write the pid to a file */
	if (write_pid() == -1) {
		fprintf(stderr,"%s : can't write to pid file - the srepod daemon is already running?\n", pname);
		exit(1);
	}
	
	/* Close all file desciptors */
        close(0);
        close(1);
        close(2);

	/* Open a log file */
	umask(022);
	sprintf(logfile, "%s/%s", gwspool, config_param(config,"log_file"));

	if (strlen(logfile)) 
		fh = fopen(logfile,"a");
	else
		fh = fopen(LOGFILE,"a");

	if (fh == NULL) {
		fprintf(stderr,"%s : failed to open the log file" , pname);
		return -1;
	}

	/* Send log/warn/debugging messages to this stream */
	set_log_stream(fh);

	/* Become session leader */
        if (setsid() == -1) {
                warn("daemon_start - setsid() failed\n");
                return -1;
        }

	/* Change working directory */
	if (chdir("/") == -1) {
                warn("daemon_start - chdir() failed\n");
		return -1;
	}

	/* Clear the file creation mask */
        umask(0);

        my_signal(SIGALRM, sig_handler);
        my_signal(SIGCHLD, sig_handler);
        my_signal(SIGCONT, SIG_IGN);
	my_signal(SIGHUP,  SIG_IGN);
	/* my_signal(SIGINFO, SIG_IGN); */
        my_signal(SIGINT,  SIG_IGN);
        my_signal(SIGPIPE, sig_handler);
        my_signal(SIGQUIT, SIG_IGN);
	my_signal(SIGSEGV, sig_handler);
        my_signal(SIGTERM, SIG_IGN);
        my_signal(SIGTSTP, SIG_IGN);
        my_signal(SIGTTIN, SIG_IGN);
        my_signal(SIGTTOU, SIG_IGN);
	my_signal(SIGUSR1, sig_handler);

	log("daemon_start - succesfully");

	return 0;
}

int	write_pid(void) {
	int	fd;
	char	pid[8];
	char	pidfile[MAX_PATH_LENGTH];
	char	*gwspool = getenv("GWSPOOL");

	if (gwspool == NULL) {
		return(-1);
	}

	sprintf(pidfile, "%s/%s", gwspool , config_param(config,"pid_file"));

	fd = open(pidfile, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	if (fd == -1) {
		return(-1);
	}

	sprintf(pid,"%d",getpid());

	write(fd, pid,strlen(pid));

	close(fd);

	return(0);
}

int	close_pid(void) {
	char *pidfile;

	pidfile = config_param(config,"pid_file");

	return(unlink(pidfile));
}

Sigfunc *my_signal(int signo, Sigfunc *func) {
        struct  sigaction       act, oact;

        act.sa_handler  = func;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
                act.sa_flags |= SA_INTERRUPT;
#endif
        } else {
#ifdef SA_RESTART
                act.sa_flags |= SA_RESTART;
#endif
        }

        if (sigaction(signo,&act,&oact) < 0)
                return ((void *)SIG_ERR);

        return((void*)oact.sa_handler);
}

static void    sig_handler(int signo) {
        int	status;
	int	exit_status;
	pid_t	childpid;

        switch(signo) {
                case SIGALRM :
                        break;
                case SIGCHLD :
			while ((childpid = waitpid(-1,&status,WNOHANG)) > 0 ) {
				if (WIFEXITED(status)) {
					exit_status = WEXITSTATUS(status);
				}
				else {
					exit_status = -1;
				}

				if (exit_status == 0) {
					set_status(childpid, REQUEST_FINISHED);
				} 
				else {
					set_status(childpid, REQUEST_FAILED);
				}

                        	debug("sig_handler - recieved SIGCHLD %d exited status %d", childpid, exit_status);
			}
                        break;
                case SIGPIPE :
                        log("sig_handler - recieved SIGPIPE");
                        break;
		case SIGSEGV :
				warn("sig_handler - going down on a segmentation fault");
				exit(1);
			break;	
		case SIGUSR1 :
			do_shutdown = 1;
			break;
        }
	return;
}
