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

"""Release XenClient XT"""

from optparse import OptionParser
from time import strftime, localtime, time
from os.path import isdir, join, isfile, split, exists, basename
from os import mkdir, listdir, umask
from subprocess import check_call
from re import match
from infrastructure.xt.get_build_info import get_build_info

RELEASE_AREA = 'setme'
CUSTOMER_SERVER = 'setme'

TRANSIT_ROOT = 'setme'
DOCS_PATTERN = 'setme/%s/setme'
DOCS_LIST = ['XTEngineAdministratorGuide',
             'XTEngineDeveloperGuide', 
             'XTArchitectureGuide',
             'XTReleaseNotes', 
             'XTSynchronizerAdministratorGuide']
BUILD_AREA = 'setme'
INTERNAL = 'NOT_FOR_DISTRIBUTION'
SIGNED = 'production-signed'

# these variables are pervasive enough, and are 
# immutable once defined, so use globals
options = None
args = None
build_directory = None
archive_directory = None
release_directory = None
signed_directory = None
build_info = None

def trace_call(args, *l, **d):
    """run a command with tracing"""
    print '+' + ' '.join(args)
    return check_call(args, *l, **d)

def eval_options():
    """Get options and arguments, updating module level global options and args."""
    global options, args

    parser = OptionParser()
    parser.add_option('-c', '--customer', metavar='CUSTOMER', action='append',
                      default=[],
                      help='Release to customer CUSTOMER (optional)')
    parser.add_option('-w', '--work', action='store_true',
                      help='Do work. Without this option just sit there')
    options,args = parser.parse_args()



def operation(fn, desc, file_test=None):
    """Do or describe doing a release operation

    Args:

        fn: the function to run if we are doing actual work
        desc: a description of the work
        file_test: if not None, only do work if file is absent
    """
    if file_test and isfile(file_test):
        print '(already have %s)' % (split(file_test)[1])
        return

    if not options.work:
        print 'would do', desc, 'if -w was specified'
        return

    print 'doing', desc
    fn()
    print 'done', desc

def get_build_directory(branch, tag):
    """Return build directory

    Raises:
        SystemExit: if the build direcotry does not exist
    """
    bdir = join(BUILD_AREA, branch, tag)
    if not isdir(bdir):
        print 'ERROR: could not find build directory', bdir
        exit(4)
    return bdir

def verify_release_name(release):
    """Verify release name is acceptable

    Args:
        release: the release name

    Raises:
        SystemExit: if the release name is unacceptable
    """
    dashspl = release.split('-')
    if len(dashspl) == 1:
        print 'ERROR: release name should have at least one - in it'
        exit(2)
    for elem in dashspl[:2]:
        if elem[0] != elem[0].upper():
            print 'ERROR: release element', elem, 
            print 'should start with a capital letter'
            exit(3)

def create_release_directory(release):
    """Work out release directory, creating it if necessary, and return it"""
    today = strftime('%Y-%m-%d-', localtime(time()))
    datepre = '' if match(r'[0-9]{4}\-[0-9]{2}\-[0-9]{2}', release) else today
    if datepre:
        print 'ERROR: Please include date prefix e.g.',datepre
        print 'Do not use multiple dates for the same release'
        exit(5)
    release_directory = '%s/%s%s' % (RELEASE_AREA, datepre, release)
    if not isdir(release_directory):
        operation(lambda: mkdir(release_directory), 'create release directory')
    return release_directory

def publish(source_name, target_name, signed=False, write_hash=True,
            source_directory=None, target_directory=None):
    """Publish a single file. If signed is True and the signing output
    directory exists, copy the file from the signing output directory
    instead of source_directory."""
    if not source_directory:
        source_directory = archive_directory
    if not target_directory:
        target_directory = release_directory

    source = join(source_directory, source_name)
    target = join(target_directory, target_name)

    if signed and isdir(signed_directory):
        source = join(signed_directory, source_name)

    if not isfile(source):
        print 'ERROR: missing', source
        exit(6)

    operation(lambda: trace_call(['rsync', '-tp', '--chmod', 'a+rX,ug+w',
                                  '--progress', source, target]),
              'rsync ' + source + ' ' + target)

    if write_hash:
        do_hashes(target)

def publish_docs(branch):
    """Publish documentation"""
    docs_directory = DOCS_PATTERN % branch
    # TODO: internationalisation of docs
    for doc in DOCS_LIST:
        name = doc + '.pdf'
        publish(name, name, write_hash=False, source_directory=docs_directory)
            
def do_hash(filename, tool, extension):
    """Write one hash for filename"""
    dirname, filebase = split(filename)
    with open(filename + extension, 'w') as f:
        trace_call([tool, filebase], cwd=dirname, stdout=f)

def do_hashes(filename):
    """Write hashes for filename"""
    for tool, extension in [('sha256sum', '.sha256'), ('md5sum', '.md5')]:
        operation(lambda: do_hash(filename, tool, extension),
                  tool + ' ' + filename)

def publish_isos():
    """Publish ISOs"""
    publish(build_info['installer'], 'installer.iso', signed=True)
    publish(build_info['installer-trial'], 'installer-trial.iso', signed=True)

    for name in build_info['sources'].split():
        publish(name, basename(name))

def publish_xctools_debian():
    """Publish XC tools debian repository"""
    # TODO: Add this to the build info.
    publish('xctools-debian-repo/xctools-debian-repo.tar.gz',
            'xctools-debian-repo.tar.gz')

def publish_sdk():
    """Publish SDK"""
    # TODO: Add 'sdk' to the build info.
    publish('sdk/xenclient-sdk.tar.gz', 'xenclient-sdk.tar.gz')

def publish_sync():
    """Publish syncXT RPMs and syncXT UI"""
    # TODO: Add 'sync' to the build info.
    sync_directory = join(archive_directory, 'sync')
    for name in listdir(sync_directory):
        if name.endswith('.rpm') or name.endswith('.tar.gz'):
            publish(name, name, source_directory=sync_directory)

def archive_build_output():
    """Archive build output"""
    # first an XC-9529 workaround
    elevated = 'really' if exists('/usr/local/bin/really') else 'sudo'
    operation(lambda: trace_call([elevated, 'chmod', '-R', 'a+rX', 
                                  build_directory]),
                                 'chmod -R a+rX build directory') 
    command = ['rsync', '-rtp', '--chmod', 'a+rX,ug+w', '--progress',
               build_directory, split(archive_directory)[0]]
    print '+' + (' '.join(command))
    operation(lambda: trace_call(command), 'rsyncing build output')

def publish_ota_update():
    """Publish OTA update tar file"""
    publish(build_info['ota-update'], 'ota-update.tar', signed=True)

def main():
    """Do a release"""
    eval_options()
    if len(args) != 3:
        print 'ERROR: specify BRANCH, BUILD and RELEASE_NAME as arguments'
        exit(1)
    umask(002)
    branch, build, release = args 
    tag = 'cam-oeprod-%s-%s' % (build, branch)
    global build_directory
    build_directory = get_build_directory(branch, tag)
    print 'INFO: build_directory=', build_directory
    verify_release_name(release)
    global release_directory
    release_directory = create_release_directory(release)
    print 'INFO: release_directory=', release_directory
    global archive_directory
    archive_directory = join(release_directory, INTERNAL, tag)
    print 'INFO: archive_directory=', archive_directory
    global signed_directory
    signed_directory = join(release_directory, INTERNAL, SIGNED)
    print 'INFO: signed_directory=', signed_directory
    print 'INFO: signed directory', 
    if not isdir(signed_directory):
        print 'MISSING; will produce non production signed release'
    else:
        print 'EXISTS'
    global build_info
    build_info = get_build_info(branch, tag)
    archive_build_output()
    publish_docs(branch)
    publish_isos()
    publish_ota_update()
    publish_xctools_debian()
    publish_sdk()
    publish_sync()
    upload()

def upload():
    """Publish files to customer server"""
    if not options.customer:
        return
    release_name = split(release_directory)[1]
    tdest = TRANSIT_ROOT + '/common/'+release_name
    operation(lambda: trace_call(['ssh', CUSTOMER_SERVER, 'mkdir', '-p',
                                  tdest]), 'mkdir to customer server')
    operation(lambda: trace_call(['rsync', '-rt', '--progress',
                                  '--exclude', INTERNAL,
                                  release_directory+'/', 
                                  CUSTOMER_SERVER+':'+tdest]),
              'upload to customer server')
    for customer in options.customer:
        croot = TRANSIT_ROOT + '/'+customer+'/'
        operation(lambda: trace_call(
                ['ssh', CUSTOMER_SERVER, 'mkdir', '-p', croot]),
                  'make customer root for '+customer)
        operation(lambda: trace_call(
                ['ssh', CUSTOMER_SERVER, 'ln', '-sf',  
                 '../common/'+release_name+'/', croot]), 
                  'symlink for customer '+customer)

if __name__ == '__main__':
    main()
