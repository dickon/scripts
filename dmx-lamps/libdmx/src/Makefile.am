#
#
# Makefile.am:
#
# $Id: Makefile.am,v 1.3 2011/01/19 19:28:12 root Exp $
#
# $Log: Makefile.am,v $
# Revision 1.3  2011/01/19 19:28:12  root
# *** empty log message ***
#
# Revision 1.2  2011/01/13 17:44:27  root
# *** empty log message ***
#
#
#
#

#
# Copyright (c) 2014 Citrix Systems, Inc.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#


INCLUDES =  ${LIBUSB_INC}

SRCS= libdmx.c version.c ftdi.c dmx.c
CPROTO=cproto

DMXSRCS=${SRCS}

noinst_HEADERS= project.h prototypes.h dmx-tail.h ext_prototypes.h

libdmx_a_SOURCES =  ${DMXSRCS}
libdmx_la_SOURCES = ${DMXSRCS}

libdmx_a_CFLAGS = ${AM_CFLAGS}

BUILT_SOURCES = version.h dmx.h

lib_LIBRARIES=libdmx.a
lib_LTLIBRARIES=libdmx.la

include_HEADERS=dmx.h

AM_CFLAGS=-g

libdmx_la_LDFLAGS = \
        -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
        -release $(LT_RELEASE) \
        -export-dynamic 

VFD=${srcdir}/..
VF=${shell cat ${VFD}/version-files}
VFS=${VF:%=${VFD}/%}
VCHK=${shell cat ${VFS} | @MD5SUM@ | @AWK@ '{print $$1 }' }
VNUM=${shell @GREP@ ${VCHK} ${VFD}/version-md5sums | @AWK@ '{ print $$2 }'  }
VDEF=${shell echo `cat ${VFD}/version-major`.`cat ${VFD}/version-minor`.`cat ${VFD}/version-micro` }


dmx.h:dmx-head.h ext_prototypes.h dmx-tail.h
	cat dmx-head.h ext_prototypes.h dmx-tail.h > dmx.h

protos: 
	echo > prototypes.h
	echo > ext_prototypes.h
	cat dmx-head.h ext_prototypes.h dmx-tail.h > dmx.h
	${CPROTO} -v -DINT_PROTOS ${INCLUDES} ${SRCS} > prototypes.tmp
	mv -f prototypes.tmp prototypes.h
	${CPROTO} -v -DEXT_PROTOS ${INCLUDES} ${SRCS} > ext_prototypes.h
	cat dmx-head.h ext_prototypes.h dmx-tail.h > dmx.h

version.h: $(VFD)/version-files $(VFD)/version-major \
	$(VFD)/version-minor $(VFD)/version-micro \
	$(VFD)/version-md5sums ${VFS} Makefile
	if [ .${VNUM} = . ]; then \
		echo "#define FULL_VERSION \"libdmx Version ${VDEF} + Edits\"" > version.h; \
		echo ${VDEF}-E > version-num; \
	else \
		echo "#define FULL_VERSION \"libdmx Version ${VNUM}\"" > version.h; \
		echo ${VNUM} > version-num; \
	fi

