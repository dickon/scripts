#
#
# Makefile.am:
#
# $Id: Makefile.am,v 1.1 2011/01/13 17:42:27 root Exp $
#
# $Log: Makefile.am,v $
# Revision 1.1  2011/01/13 17:42:27  root
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

SUBDIRS=src test app

EXTRA_DIST= version-major version-minor version-micro version-files version-md5sums

bin_SCRIPTS = libdmx-config

pkgconfigdir=${LIBDIR}/pkgconfig
pkgconfig_DATA= libdmx.pc

libdmx.pc.src:libdmx.pc.src.in
libdmx.pc:libdmx.pc.src
	@SED@ -f version.sed < libdmx.pc.src > libdmx.pc || rm -f libdmx.pc

libdmx-config.src:libdmx-config.src.in
libdmx-config:libdmx-config.src version.sed
	@SED@ -f version.sed < libdmx-config.src > libdmx-config || rm -f libdmx-config
	chmod +x $@
	

VFD=${srcdir}
VF=${shell cat ${VFD}/version-files}
VFS=${VF:%=${VFD}/%}
VCHK=${shell cat ${VFS} | @MD5SUM@ | @AWK@ '{print $$1 }' }
VNUM=${shell @GREP@ ${VCHK} ${VFD}/version-md5sums | @AWK@ '{ print $$2 }'  }
VDEF=${shell echo `cat ${VFD}/version-major`.`cat ${VFD}/version-minor`.`cat ${VFD}/version-micro` }

version.sed: $(VFD)/version-files $(VFD)/version-major \
	$(VFD)/version-minor $(VFD)/version-micro \
	$(VFD)/version-md5sums ${VFS} Makefile
	if [ .${VNUM} = . ]; then \
		echo "s/%VERSION%/${VDEF}-E/g" > version.sed; \
	else \
		echo "s/%VERSION%/${VNUM}/g" > version.sed; \
	fi

