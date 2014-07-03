#! /usr/bin/env python
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

import os, sys
from pprint import pprint
from time import asctime, localtime, time


class SubprocessFailure(Exception):
    pass

def check_output(args):
    """Nasty check_output implementation; do not wish to assume Python 2.6+"""
    cmd = ' '.join(args)
    fobj = os.popen(cmd, 'r')
    output = fobj.read()
    code = fobj.close()
    if code != None:
        raise SubprocessFailure(cmd, code)
    return output

ROOTS = [] # add list of directories to clean here

def make_agelist(root, depth):
    """Return a dictionary where keys are build creation times
    and values are lists of build directories, for each build
    we can find"""
    agelist = {}
    for directory, dirnames, filenames in os.walk(root):
        spl = directory[len(root):].split('/')
        if len(spl) != depth or os.path.split(directory)[1].startswith('.'):
            continue
        dstat = os.stat(directory)
        agelist.setdefault(dstat.st_mtime, list())
        agelist[dstat.st_mtime].append(directory)
        del dirnames[:]
    return agelist

def make_branches(agelist):
    """Given an agelist (see make_agelist's result),
    return (number of builds, branches dict) where
    branches dict is a dictionary mapping branch directories
    to a a dictionary mapping creation times to list of builds"""
    
    branches = {}
    for age in sorted(agelist.keys()):
        for directory in agelist[age]:
            base, item = os.path.split(directory)
            branches.setdefault(base, dict())
            branches[base][age] = item
    return branches

def get_free(root):
    return float(check_output(['df', root]).split()[-3]) / 1e6


def get_bname(branch):
    return branch.split('/')[-1]

def run(*args):
    print '+' + ' '.join(args)
    if '-a' in sys.argv:
        check_output(args)

def main():
    """Figure out what builds to delete"""
    byage = {}
    latest = {}
    for root, depth, offload, space in ROOTS:
        if not os.path.isdir(root):
            continue

        if get_free(root) > space:
            continue
        
        branches = make_branches(make_agelist(root, depth))
        for branch in branches:
            bname = get_bname(branch)
            
            ages = sorted(branches[branch])
            for age in ages:
                byage.setdefault(age, list())
                byage[age].append(( branch+'/'+branches[branch][age], 
                                    root, depth, offload, space))
            if ages:
                latest.setdefault(bname, (0, None))
                if latest[bname][0] < ages[-1]:
                    latest[bname] = (ages[-1], 
                                      branch+'/'+branches[branch][ages[-1]])
    killed = 0
    for age in sorted(byage.keys()):
        for dname, root, depth, offload, space in byage[age]:
            if get_free(root) > space:
                continue
            segs = dname.split('/')
            branch = get_bname(dname)
            print '# build',asctime(localtime(age)), dname
            if branch in latest:
                _, last = latest[branch]
                if last == dname:
                    print '# keep', last, 'since it the latest', branch, 'build'
                    continue
                else:
                    print '# newer', last, 'so can kill', dname

            if killed > 5000:
                print '# killed plenty for now'
            else:
                print '# only', int(get_free(dname)), 'GB free on', dname
                if offload:
                    segs = dname.split('/')
                    name = segs[-2]
                    dest = offload+'/'+name
                    run('rsync', '-rap', dname, dest)
                run('rm', '-rf', dname)
                killed += 1
    # remove empty branch directories
    for root, depth, _, _ in ROOTS:
        if not os.path.isdir(root):
            continue
        for directory, dirnames, filenames in os.walk(root):
            here = len(directory[len(root):].split('/'))
            if here == depth -1 and len(dirnames) == 0 and len(filenames) == 0:
                run('rmdir', directory)

            if here >= depth -1:
                del dirnames[:]
main()


