#! /usr/bin/env python

from urllib2 import urlopen
from json import load
from os.path import isdir, join
from subprocess import check_call
from sys import argv
if len(argv) != 3:
    print 'USAGE: replicate_github organization destination_directory'
    print
    print 'replicate all repos in github organization; when first seen'
    print 'clone them, on later runs fetch'
    exit(1)

for repo in load(urlopen('https://api.github.com/orgs/'+argv[1]+'/repos?type=all')):
    destd = join(argv[2], repo['name']+'.git')
    if not isdir(destd):
        print 'cloning to', destd
        check_call(['git', 'clone', '--bare', repo['clone_url'], destd])
    else:
        print 'fetching to', destd
        check_call(['git', '--git-dir='+destd, 'fetch'])
