#
# Makefile.am:
#
# $Id: Makefile.am,v 1.3 2011/01/19 19:28:03 root Exp $
#
# $Log: Makefile.am,v $
# Revision 1.3  2011/01/19 19:28:03  root
# *** empty log message ***
#
# Revision 1.1  2011/01/13 17:42:27  root
# *** empty log message ***
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


INCLUDES = -I$(srcdir)/../src 

bin_PROGRAMS = dmx dmxfind

dmxfind_SOURCES = dmxfind.c
dmxfind_LDADD = ../src/libdmx.a   -lusb -lX11

dmx_SOURCES = dmx.c
dmx_LDADD = ../src/libdmx.a   -lusb -lX11

AM_CFLAGS=-g 


