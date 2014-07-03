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

BUILD_PATH = 'setme/%s/%s' # add path to build directory here
OLD_BUILD_PATH = 'setme/%s/%s' # add path to archived build directory here
from os.path import isdir

class BuildNotFound(Exception):
    """Unable to locate build"""

def find_build(branch, tag):
    """Return a directory for tag on branch or raise BuildNotFound"""
    for directory_format in [BUILD_PATH, OLD_BUILD_PATH]:
        loc = directory_format % (branch, tag)
        if isdir(loc):
            return loc
    raise BuildNotFound(branch, tag)

