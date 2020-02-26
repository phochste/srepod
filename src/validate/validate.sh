#!/bin/bash
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
# Validate an XML file 
#
# Usage:
#
# validate.sh xmlfile [gdbm_file]
#
# E.g:
#
# validate.sh /tmp/vb1.xml
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#

source /lanl/srepod/cgi-bin/environ

# If we receive the name of a gdbm_file on the command line, then update
# the gdbm file with the last-modified date read from the xml file...
if [ -n "$2" ] ; then
	LAST_MOD=`egrep "^[Ll][Aa][Ss][Tt]-[Mm][Oo][Dd][Ii][Ff][Ii][Ee][Dd]: " $1 | sed -e 's/[Ll][Aa][Ss][Tt]-[Mm][Oo][Dd][Ii][Ff][Ii][Ee][Dd]: //'`

	if [ -n "$LAST_MOD" ] ; then
		LD_LIBRARY_PATH=${GWHOME}/lib ${GWHOME}/bin/dbupdate $2 "$LAST_MOD"
	fi	
fi	
	

cat $1 | ${GWHOME}/bin/hstrip | ${GWHOME}/bin/xmlwf | ${GWHOME}/bin/hasout
