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

from infrastructure.xt.decode_tag import extract_kind
from infrastructure.xt.get_build_info import get_build_info
from infrastructure.xt.decode_tag import extract_build_number, extract_branch
from os.path import join, isfile

def inspect_build(build_dir, tag, typename='build', alias='build'):
    """Work out information about build_dir which contains a build of tag"""
    variants = []
    branch = extract_branch(tag)
    build_info = get_build_info(branch, tag, build_dir=build_dir)
    for kind, dirname in [('plain', 'netboot'), 
                          ('trial', 'netboot-trial'),
                          ('kent', 'netboot-kent')]:
        netboot = build_info.get('netboot' if kind != 'trial' else
                                 'netboot-trial')
        if netboot == dirname:
            pxelinux = join(build_dir, netboot, 'pxelinux.cfg')
            if isfile(pxelinux):
                variants.append({'kind':kind, 'pxelinux':pxelinux,
                                 'netboot':netboot})
    if variants == []:
        return []
    return [{'type':typename, 'build_number': extract_build_number(tag),
             'branch':branch, 'variants': variants, 'alias':alias,
             'kind':extract_kind(tag),
             'tag':tag, 'build_directory':build_dir}]
