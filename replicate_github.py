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
if 0:
    # this should work but seems to miss out many repos due to a github bug
    
    repos = load(urlopen('https://api.github.com/orgs/'+argv[1]+'/repos?type=all'))
else:
    if argv[1] == 'openxt':
        repos = [{'name':name[:-4], 'clone_url':'git://git.xci-test.com/xenclient/'+name} for name in "blktap.git bootage.git dm-agent.git dm-wrapper.git fbtap.git gene3fs.git icbinn.git idl.git input.git installer.git ioemu.git ioemu-pq.git ipxe-pq.git libedid.git libpciemu.git libxcdbus.git libxenbackend.git linux-3.11-pq.git manager.git meta-selinux.git msi-installer.git network.git ocaml.git openxt.git polmod-example.git pv-linux-drivers.git qemu-dm.git qemu-dm-pq.git refpolicy-xt-pq.git resized.git sdk.git seabios.git seabios-pq.git selinux-policy.git surfman.git sync-client.git sync-cli.git sync-database.git sync-server.git sync-ui-helper.git sync-wui.git toolstack-data.git toolstack.git uid.git v4v.git win-tools.git xblanker.git xclibs.git xctools.git xc-vusb-daemon.git xc-vusb.git xc-windows.git xenaccess.git xenaccess-pq.git xenclient-oe-extra.git xenclient-oe.git xen-common-pq.git xenfb2.git xsm-policy.git".split()]
    else:
        print 'org', argv[1], 'not known'

for repo in repos: 
    destd = join(argv[2], repo['name']+'.git')
    if not isdir(destd):
        print 'cloning to', destd
        check_call(['git', 'clone', '--bare', repo['clone_url'], destd])
    else:
        print 'fetching to', destd
        check_call(['git', '--git-dir='+destd, 'fetch', '--all'])
