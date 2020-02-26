#!/bin/bash
#
# $Id: unregister.sh,v 1.2 2006/12/19 08:33:17 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
source ../cgi-bin/environ

${GWHOME}/sbin/unregister $*
