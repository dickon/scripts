#! /usr/bin/env python

from urllib2 import urlopen, URLError
from json import load
from os import rename
from os.path import isdir, join
from subprocess import check_call
from sys import argv
if len(argv) not in [3,4]:
    print 'USAGE: replicate_github organization destination_directory [USER_OVERRIDE]'
    print
    print 'replicate all repos in github organization; when first seen'
    print 'clone them, on later runs fetch'
    print 
    print 'if USER_OVERRIDE is specified, overlay any matching repos from that user'
    exit(1)
repos = load(urlopen('https://api.github.com/orgs/'+argv[1]+'/repos?type=all&per_page=1000'))
for repo in repos: 
    destd = join(argv[2], repo['name']+'.git')
    if not isdir(destd):
        print 'cloning to', destd
        check_call(['git', 'clone', '--bare', repo['clone_url'], destd])
    else:
        print 'fetching to', destd
        check_call(['git', 'fetch', '--all'], cwd=destd)
if len(argv) == 4:
    for repo in repos:
        url = 'https://github.com/'+argv[3]+'/'+repo['name']+'.git'
        try:
            urlopen(url)
        except URLError:
            continue
        destd = join(argv[2], repo['name']+'.git')
        upstreamd = destd+'.upstream'
        if not isdir(upstreamd):
            rename(destd, upstreamd)
            check_call(['mv', destd, upstreamd])
            check_call(['git', 'clone', '--bare', url, destd])
