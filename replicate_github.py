#! /usr/bin/env python

from urllib2 import urlopen
from json import load
from argparse import ArgumentParser
from os.path import isdir, join
from subprocess import check_call

parser = ArgumentParser()
parser.add_argument('organization', help='Github organization name')
parser.add_argument('destination', help='Destination directory')
args = parser.parse_args()
for repo in load(urlopen('https://api.github.com/orgs/'+args.organization+'/repos?type=all')):
    destd = join(args.destination, repo['name']+'.git')
    if not isdir(destd):
        print 'cloning to', destd
        check_call(['git', 'clone', '--bare', repo['clone_url'], destd])
    else:
        print 'fetching to', destd
        check_call(['git', '--git-dir='+destd, 'fetch'])
