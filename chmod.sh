#!/bin/bash
#
# $Id: chmod.sh,v 1.3 2006/12/19 08:33:10 hochstenbach Exp $
#
# Run this command as root:
#
# sh chmod.sh
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
# Dec 2002
#

function priv_access {
	USERID=`cat /etc/passwd | egrep "^$1:" | awk 'BEGIN {FS=":"}{print $3}'`
	GROUPID=`cat /etc/group | egrep "^$2:" | awk 'BEGIN {FS=":"}{print $3}'`

	sed "s/^uid.*/uid	$USERID/" bin/srepod.conf > /tmp/env$$
	mv /tmp/env$$ bin/srepod.conf

	sed "s/^gid.*/gid	$GROUPID/" bin/srepod.conf > /tmp/env$$
	mv /tmp/env$$ bin/srepod.conf

	
	sed "s/GWWORLDACCESS=\".\"/GWWORLDACCESS=\"0\"/"  cgi-bin/environ > /tmp/env$$
	mv /tmp/env$$ cgi-bin/environ

	chown -R ${MYUSER}:${MYGROUP} spool
	chown -R ${MYUSER}:${MYGROUP} db
	chown -R ${MYUSER}:${MYGROUP} cgi-bin
	chown -R ${MYUSER}:${MYGROUP} bin/create_cache.sh
	chown -R ${MYUSER}:${MYGROUP} bin/srepod.conf

	chmod 755 .
	chmod 711 cgi-bin/gateway
	chmod 711 cgi-bin/registry
	chmod 555 cgi-bin/registry.cgi
	chmod 555 cgi-bin/gateway.cgi
	chmod 755 cgi-bin/environ
	chmod 644 cgi-bin/*.conf
	chmod 755 cgi-bin
	chmod 755 sbin
	chmod 755 sbin/setsession
	chmod 755 bin
	chmod 755 bin/*.sh
	chmod 755 bin/daemonctl
	chmod 755 htdocs
	chmod 755 htdocs/*
	chmod 755 lib
	chmod 644 lib/*
	chmod 755 db
	chmod 755 spool
}

function world_access {
	sed  "s/GWWORLDACCESS=\".\"/GWWORLDACCESS=\"1\"/"  cgi-bin/environ > /tmp/env$$
	mv /tmp/env$$ cgi-bin/environ

	chmod 755 cgi-bin/environ

	sed "s/^uid.*/uid	-1/" bin/srepod.conf > /tmp/env$$
	mv /tmp/env$$ bin/srepod.conf

	sed "s/^gid.*/gid	-1/" bin/srepod.conf > /tmp/env$$
	mv /tmp/env$$ bin/srepod.conf

	chmod 755 .
	chmod 711 cgi-bin/gateway
	chmod 711 cgi-bin/registry
	chmod 555 cgi-bin/registry.cgi
	chmod 555 cgi-bin/gateway.cgi
	chmod 666 cgi-bin/*.conf
	chmod 777 cgi-bin
	chmod 755 sbin
	chmod 755 sbin/setsession
	chmod 755 bin
	chmod 755 bin/*.sh
	chmod 755 bin/daemonctl
	chmod 755 htdocs
	chmod 755 htdocs/*
	chmod 755 lib
	chmod 644 lib/*
	chmod 1777 db
	chmod 777 spool

	touch spool/gateway.log
	touch spool/registry.log

	chmod 666 spool/gateway.log
	chmod 666 spool/registry.log

	for f in 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10\
	          11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20; do
		  if [ ! -d db/$f ]; then
		  	mkdir db/$f
		  fi
		  chmod 777 db/$f
		  chmod 1777 db/$f
	done		  

}

OSTYPE=`uname`

if [ ${OSTYPE} == "Linux" ]; then
	ECHO="echo -n"
else
	ECHO="echo"
fi	

cat <<EOF

Copyright (C) 2003 Los Alamos National Laboratoy.  All rights reserved.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

Please answer these few questions to finish your installation:

EOF

${ECHO} "Enter the user name or id of the process running cgi scripts: [nobody] "

read ans

if [ ! -z "$ans" ]; then
	MYUSER=$ans
else
	MYUSER="nobody"
fi


${ECHO} "Enter the group name or id of the process running cgi scripts: [nobody] "

read ans

if [ ! -z "$ans" ]; then
	MYGROUP=$ans
else
	MYGROUP="nobody"
fi


if [ "$USER" != "$MYUSER" ] || [ "`groups | grep $MYGROUP`" = "0" ]; then 
  if [ "$EUID" -ne "0" ]; then
	${ECHO} "You need root privileges to run this command"
	exit 1
  fi
  
  priv_access $MYUSER $MYGROUP
  
else

  world_access

fi 



