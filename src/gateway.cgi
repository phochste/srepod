#!/bin/bash
#
# This work is licensed under the Creative Commons Attribution-NonCommercial 
# License. To view a copy of this license, visit 
#
# http://creativecommons.org/licenses/by-nc/1.0/ 
# 
# or send a letter to Creative Commons, 559 Nathan Abbott Way, Stanford, 
# California 94305, USA.
#
# $Id: gateway.cgi,v 1.2 2006/12/19 08:33:11 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#

source ./environ

LD_LIBRARY_PATH=${GWHOME}/lib ./gateway
