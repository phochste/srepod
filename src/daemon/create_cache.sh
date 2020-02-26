#!/bin/bash
#
# $Id: create_cache.sh,v 1.7 2006/12/19 08:33:14 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
source ../cgi-bin/environ

# Set the maximum size of core files created
ulimit -c 0

# Set the maximum amount of cpu time in seconds
#ulimit -t 300

# Set the amount of virtual memory available to the shell
#ulimit -v 102400

TMPXMLFILE="${GWSPOOL}/xml.$$"
TMPCACHEFILE="${GWSPOOL}/dbf.$$"
LOGFILE="${GWSPOOL}/create_cache.log"

function verbose {
	DATE=`date`

	if [ ! -r ${LOGFILE} ]; then
		touch ${LOGFILE}
		chmod 644 ${LOGFILE}
	fi

	echo "$DATE $0[$$] : $1" >> ${LOGFILE}
}

function syserror {
	${GWHOME}/sbin/setsession $CACHEFILE SESSION_CONFIG_ERROR > /dev/null 2>&1
	echo "$DATE $0[$$] : $1"
	echo "$DATE $0[$$] : $1" >> ${LOGFILE}
	exit 2
}

if [ -z "${GWHOME}" ]; then
	syserror "no GWHOME set"
fi	

if [ -z "${GWSPOOL}" ]; then
	syserror "no GWSPOOL set"
fi

if [ -z "${GWMAXCACHESIZE}" ];  then
	syserror "no GWMAXCACHESIZE set"
fi

if [ -z "${GWDBPATH}" ]; then
	syserror "no GWDBPATH set"
fi

if [ -z "${GWMAXNETWORKSIZE}" ]; then
	syserror "no GWMAXNETWORKSIZE set"
fi

if [ -z "${GWMAXNETWORKTIME}" ]; then
	syserror "no GWMAXNETWORKTIME set"
fi

if [ $# != 2 ]; then
	echo "usage : $0 cachefile url"
	exit 1
fi	

CACHEFILE=$1
STATICREPOURL=$2

#
# Step 0. Temporary hack: if the STATICREPOURL end with '#TERMINATE', then we
# should start the termination procedure.
#
FRAGMENT=`echo ${STATICREPOURL} | sed -e 's/^[^#][^#]*#//'`
TERMINATE=0

if [ "${FRAGMENT}" == "TERMINATE" ]; then
	TERMINATE=1
	STATICREPOURL=`echo $STATICREPOURL | sed -e 's/#.*//'`
	verbose "Termination requested for $STATICREPOURL..."
fi

#
# Step 1. Check if there is enough space left for the cache file...
#
verbose "Checking space left on cache device..."

SIZE=`du -sk ${GWDBPATH} | awk '{print $1}'`

if [ $SIZE -gt $GWMAXCACHESIZE ]; then
	verbose "No space left - current size $SIZE > $MAXSIZE"

	${GWHOME}/sbin/setsession $CACHEFILE SESSION_NO_RESOURCES_LEFT > /dev/null 2>&1

	exit 1
fi

#
# Step 2. Set the cache file status to 'dirty'...
#
verbose "Setting status of $CACHEFILE to 'dirty'..."

${GWHOME}/bin/dblock $CACHEFILE > /dev/null 2>&1

if [ $? != 0 ]; then
	verbose "Failed to set status of '$CACHEFILE'..."

	${GWHOME}/sbin/setsession $CACHEFILE SESSION_CONFIG_ERROR  > /dev/null 2>&1

	exit 1
fi

#
# Step 3. Get a fresh copy of the static repository from the network...
#
verbose "Connecting to '$STATICREPOURL'..."

${GWHOME}/sbin/setsession $CACHEFILE SESSION_CONTACTING > /dev/null 2>&1

${GWHOME}/bin/get -h -p -s $GWMAXNETWORKSIZE -t $GWMAXNETWORKTIME -o $TMPXMLFILE $STATICREPOURL

EXITSTAT=$?
if [ $EXITSTAT != 0 ]; then
	REASON=""

	case $EXITSTAT in
		1)
		    REASON="invalid command line arguments"
		    ;;
		2)
		    REASON="output file can't be created"
		    ;;
		3)
		    REASON="timeout value reached"
		    ;;
		4)
		    REASON="maxsize reached"
		    ;;
		5)
		    REASON="internal error"
		    ;;
	esac	    

	verbose "Failed to get '$STATICREPOURL' - $REASON"

	${GWHOME}/sbin/setsession $CACHEFILE SESSION_INVALID > /dev/null 2>&1

	rm -f $TMPXMLFILE

	exit 1
fi

#
# Step 4. If we got a fresh copy, then we need to update the last_mod field in the
# cache file...
#
verbose "Searching for 'Last-Modified'..."

LAST_MOD=`egrep "^[Ll][Aa][Ss][Tt]-[Mm][Oo][Dd][Ii][Ff][Ii][Ee][Dd]: " $TMPXMLFILE | sed -e 's/[^A-Za-z0-9:, -]//g' | sed -e 's/[Ll][Aa][Ss][Tt]-[Mm][Oo][Dd][Ii][Ff][Ii][Ee][Dd]: //'`
 
if [ -n "$LAST_MOD" ]; then
	verbose "Updating cache file with last modified date of static repository file..."	

	${GWHOME}/bin/dbupdate $CACHEFILE "$LAST_MOD"

	if [ $? != 0 ]; then
		verbose "Update cache file failed..."

		${GWHOME}/sbin/setsession $CACHEFILE SESSION_CONFIG_ERROR  > /dev/null 2>&1

		rm -f $TMPXMLFILE

		exit 1
	fi	
fi

#
# Step 5. Validate the XML file to check if we have good XML...
#
verbose "Validating the static repository file..."

${GWHOME}/sbin/setsession $CACHEFILE SESSION_VALIDATING > /dev/null 2>&1

cat $TMPXMLFILE | ${GWHOME}/bin/hstrip | ${GWHOME}/bin/xmlwf | ${GWHOME}/bin/hasout

if [ $? != 0 ]; then
	verbose "Validation failed, this is no well formed XML..."

	${GWHOME}/sbin/setsession $CACHEFILE SESSION_INVALID > /dev/null 2>&1

	rm -f $TMPXMLFILE

	exit 1
fi

if [ $GWVALIDBASEURL != 0 ]; then
	cat $TMPXMLFILE | ${GWHOME}/bin/hstrip | ${GWHOME}/bin/validurl - $STATICREPOURL

	if [ $? != 0 ]; then
		verbose "Validation failed, the baseURL is not correctly set in the XML..."
	
		${GWHOME}/sbin/setsession $CACHEFILE SESSION_INVALID > /dev/null 2>&1

		rm -f $TMPXMLFILE

		if [ ${GWDOTERMINATE} == 1 -a ${TERMINATE} == 1 ]; then
			verbose "Terminating $STATICREPOURL..."
		
			${GWHOME}/bin/unregister.sh $STATICREPOURL

			if [ $? != 0 ]; then
				verbose "Termination failed"
			else
				verbose "Termination succeeded"
			fi	
		fi

		exit 1
	fi
fi

#
# Step 6. Transform the XML file into a cache file ...
#
verbose "Transforming the XML into a gdbm cache file..."

${GWHOME}/sbin/setsession $CACHEFILE SESSION_WRITING > /dev/null 2>&1

# The namespace program puts namespaces defined in the root
# element into the record elements...
${GWHOME}/bin/namespace $TMPXMLFILE | ${GWHOME}/bin/transform - $TMPCACHEFILE $STATICREPOURL 2>> $LOGFILE

if [ $? != 0 ]; then
	verbose "Transformation failed..."

	${GWHOME}/sbin/setsession $CACHEFILE SESSION_INVALID > /dev/null 2>&1

	rm -f $TMPXMLFILE
	rm -f $TMPCACHEFILE

	exit 1
fi

rm -f $TMPXMLFILE

#
# Step 7. Installing the cache file to its final destination...
#
verbose "Installing the cache file..."

${GWHOME}/bin/dbinstall $TMPCACHEFILE $CACHEFILE

if [ $? != 0 ]; then
	verbose "Installation failed..."

	${GWHOME}/sbin/setsession $CACHEFILE SESSION_CONFIG_ERROR > /dev/null 2>&1

	rm -f $TMPCACHEFILE

	exit 1
fi	

#
# Step 8. Setting the status of the cache file back to clean...
#
verbose "Setting status of $CACHEFILE to 'clean'..."

${GWHOME}/bin/dbunlock $CACHEFILE > /dev/null 2>&1

if [ $? != 0 ]; then
	verbose "Failed to set status of '$CACHEFILE'..."

	${GWHOME}/sbin/setsession $CACHEFILE SESSION_CONFIG_ERROR > /dev/null 2>&1

	exit 1
fi	

#
# Exit gracefully...
#
exit 0
