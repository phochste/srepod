#!/bin/bash
#
# $Id: transform.sh,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
#
# Transform a static repository into a gdbm file.
#
# Usage:
#
# transform.sh xml_file db_file http_address 
#
# E.g:
#
# transform.sh /tmp/test.xml /tmp/test.dbf http://libtest.lanl.gov/vb1.xml
#
# Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#

source /lanl/srepod/cgi-bin/environ

LD_LIBRARY_PATH=${GWHOME}/lib ${GWHOME}/bin/transform $1 $2 $3 
