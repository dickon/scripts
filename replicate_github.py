#! /usr/bin/env python

from urllib2 import urlopen, URLError
from json import load
from os import rename
from os.path import isdir, join
from subprocess import check_call
from optparse import OptionParser

parser = OptionParser()
parser.add_option('--user', help='Override git repos with any of matching name from USER',
                  metavar='USER')
parser.add_option('--git-binary', metavar='PATH', default='git',
                  help='Assume git binary is at PATH')
options, args = parser.parse_args()
if len(args)!=2:
    print 'USAGE: replicate_github organization destination_directory'
    print
    print 'replicate all repos in github organization; when first seen'
    print 'clone them, on later runs fetch'
    print 
    print 'if USER_OVERRIDE is specified, overlay any matching repos from that user'
    exit(1)
repos = load(urlopen('https://api.github.com/orgs/'+args[0]+'/repos?type=all&per_page=1000'))
for repo in repos: 
    destd = join(args[1], repo['name']+'.git')
    if not isdir(destd):
        print 'cloning to', destd
        check_call([options.git_binary, 'clone', '--bare', repo['clone_url'], destd])
    else:
        print 'fetching to', destd
        check_call([options.git_binary, 'fetch', '--all'], cwd=destd)
if options.user:
    for repo in repos:
        url = 'https://github.com/'+options.user+'/'+repo['name']+'.git'
        try:
            urlopen(url)
        except URLError:
            continue
        destd = join(args[1], repo['name']+'.git')
        upstreamd = destd+'.upstream'
        if not isdir(upstreamd):
            rename(destd, upstreamd)
            check_call([options.git_binary, 'clone', '--bare', url, destd])
