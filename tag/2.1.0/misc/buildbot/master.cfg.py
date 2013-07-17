# -*- python -*-
# ex: set syntax=python:

from buildbot.buildslave import BuildSlave
from buildbot.changes.svnpoller import SVNPoller, split_file_branches 
from buildbot.config import BuilderConfig
from buildbot.process.factory import BuildFactory
from buildbot.process.properties import WithProperties, Property
from buildbot.schedulers.forcesched import ForceScheduler
from buildbot.schedulers.basic import SingleBranchScheduler
from buildbot.changes.filter import ChangeFilter
from buildbot.status import html, web
from buildbot.steps.shell import Compile, ShellCommand
from buildbot.steps.source.svn import SVN
from buildbot.steps.transfer import FileUpload
from buildbot.steps.master import MasterShellCommand

import schat_passwords
import os

SCHAT_VERSION        = "2.1.0"
SCHAT_VERSION_LEGACY = "0.8.6"

SCHAT_UPLOAD_BASE        = "/var/www/download.schat.me/htdocs/schat2/snapshots/" + SCHAT_VERSION
SCHAT_UPLOAD_BASE_LEGACY = "/var/www/download.schat.me/htdocs/schat/snapshots/" + SCHAT_VERSION_LEGACY

authz = web.authz.Authz(
  auth=web.auth.BasicAuth(schat_passwords.WEB_USERS),
  gracefulShutdown   = 'auth',
  forceBuild         = 'auth',
  forceAllBuilds     = 'auth',
  pingBuilder        = 'auth',
  stopBuild          = 'auth',
  stopAllBuilds      = 'auth',
  cancelPendingBuild = 'auth',
)


c = BuildmasterConfig = {
  'title':        "Simple Chat 2",
  'titleURL':     "https://schat.me",
  'buildbotURL':  "http://buildbot.schat.me/",
  'slavePortnum': 9989,
  'slaves': [
    BuildSlave("lucid",     schat_passwords.LUCID),
    BuildSlave("lucid64",   schat_passwords.LUCID64),
    BuildSlave("win32",     schat_passwords.WIN32),
    BuildSlave("macosx",    schat_passwords.MACOSX),
    BuildSlave("master",    schat_passwords.MASTER),
    BuildSlave("precise",   schat_passwords.PRECISE),
    BuildSlave("precise64", schat_passwords.PRECISE64),
  ],
  'change_source': [
    SVNPoller(
      svnurl="https://schat.googlecode.com/svn",
      split_file=split_file_branches,
      pollinterval = 60,
    ),
  ],
  'status': [
    html.WebStatus(
      http_port="tcp:8010:interface=127.0.0.1",
      authz=authz,
      revlink='http://code.google.com/p/schat/source/detail?r=%s'
    ),
  ],
  'db': {
    'db_url': "sqlite:///state.sqlite",
  }
}


c['properties'] = {
  'version': SCHAT_VERSION,
  'version_legacy': SCHAT_VERSION_LEGACY,
  'suffix':  '-dev',
}


c['schedulers'] = [
  ForceScheduler(
    name='force',
    builderNames=['lucid', 'lucid64', 'win32', 'win32-legacy', 'macosx', 'macosx-legacy', 'source', 'ppa', 'ppa-dev', 'obs', 'obs-dev', 'precise', 'precise64', 'release']
  ),
  SingleBranchScheduler(
    name='trunk',
    change_filter=ChangeFilter(branch=None),
    treeStableTimer=60,
    builderNames=['lucid', 'lucid64', 'win32', 'source', 'precise', 'precise64']
  )
]


svn_co = [
  SVN(mode='full', method='clobber', repourl='https://schat.googlecode.com/svn/trunk/'),
]

svn_co_legacy = [
  SVN(mode='full', method='clobber', repourl='https://schat.googlecode.com/svn/branches/0.8/'),
]


def MakeDebBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(
    name          = 'revision',
    command       = ['bash', 'revision.sh'],
    workdir       = 'build/os/ubuntu',
    haltOnFailure = True,
    env           = { 'SCHAT_REVISION': Property('got_revision') },
  ))
  f.addStep(ShellCommand(
    name          = 'deb',
    command       = ['bash', 'build.sh'],
    workdir       = 'build/os/ubuntu',
    env           = { 'SCHAT_VERSION': SCHAT_VERSION },
    haltOnFailure = True,
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/ubuntu/deb/schat2_%(version)s-1~%(codename)s_%(arch)s.deb'),
    masterdest = UploadFileName('schat2_%(version)s-1~%(codename)s%(suffix)s_%(arch)s.deb'),
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/ubuntu/deb/schatd2_%(version)s-1~%(codename)s_%(arch)s.deb'),
    masterdest = UploadFileName('schatd2_%(version)s-1~%(codename)s%(suffix)s_%(arch)s.deb'),
  ))
  return f


def MakePpaBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(
    name          = 'revision',
    command       = ['bash', 'revision.sh'],
    workdir       = 'build/os/ubuntu',
    haltOnFailure = True,
    env           = { 'SCHAT_REVISION': Property('got_revision') },
  ))
  f.addStep(ShellCommand(
    name          = 'ppa',
    command       = ['bash', 'ppa.sh'],
    workdir       = 'build/os/ubuntu',
    env           = {
      'SCHAT_VERSION':   SCHAT_VERSION,
      'SCHAT_REVISION':  Property('got_revision'),
      'SCHAT_PPA':       Property('ppa'),
    },
    haltOnFailure = True,
  ))
  return f


def MakeWinBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(
    name          = 'revision',
    command       = ['cmd', '/c', 'revision.cmd'],
    workdir       = 'build/os/win32',
    env           = { 'SCHAT_REVISION': Property('got_revision') },
    haltOnFailure = True,
  ))
  f.addStep(ShellCommand(
    name          = 'qmake',
    command       = ['qmake', '-r'],
    haltOnFailure = True,
  ))
  f.addStep(Compile(
    command       = ['jom', '-j3', '/NOLOGO'],
    haltOnFailure = True,
  ))
  f.addStep(ShellCommand(
    name          = 'nsis',
    command       = ['cmd', '/c', 'nsis.cmd'],
    workdir       = 'build/os/win32',
    env           = { 'SCHAT_SIGN_FILE': schat_passwords.SIGN_FILE, 'SCHAT_SIGN_PASSWORD': schat_passwords.SIGN_PASSWORD, 'SCHAT_VERSION': SCHAT_VERSION, 'SCHAT_REVISION': Property('got_revision') },
    haltOnFailure = True,
    logEnviron    = False,
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/win32/out/schat2-%(version)s.exe'),
    masterdest = UploadFileName('schat2-%(version)s%(suffix)s.exe'),
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/win32/out/schat2-server-%(version)s.exe'),
    masterdest = UploadFileName('schat2-server-%(version)s%(suffix)s.exe'),
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/win32/out/schat2-%(version)s.msi'),
    masterdest = UploadFileName('schat2-%(version)s%(suffix)s.msi'),
  ))
  return f


def MakeWinLegacyBuilder():
  f = BuildFactory()
  f.addSteps(svn_co_legacy)
  f.addStep(ShellCommand(
    name          = 'revision',
    command       = ['cmd', '/c', 'revision.cmd'],
    workdir       = 'build/os/win32',
    env           = { 'SCHAT_VERSION': SCHAT_VERSION_LEGACY, 'SCHAT_REVISION': Property('got_revision') },
    haltOnFailure = True,
  ))
  f.addStep(ShellCommand(
    name          = 'qmake',
    command       = ['qmake', '-r'],
    haltOnFailure = True,
  ))
  f.addStep(Compile(
    command       = ['jom', '-j3', '/NOLOGO'],
    haltOnFailure = True,
  ))
  f.addStep(ShellCommand(
    name          = 'nsis',
    command       = ['cmd', '/c', 'nsis.cmd'],
    workdir       = 'build/os/win32',
    env           = { 'SCHAT_SIGN_FILE': schat_passwords.SIGN_FILE, 'SCHAT_SIGN_PASSWORD': schat_passwords.SIGN_PASSWORD, 'SCHAT_VERSION': SCHAT_VERSION_LEGACY, 'SCHAT_REVISION': Property('got_revision') },
    haltOnFailure = True,
    logEnviron    = False,
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/win32/out/schat-%(version_legacy)s.%(got_revision)s.exe'),
    masterdest = UploadFileNameLegacy('schat-%(version_legacy)s.%(got_revision)s.exe'),
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/win32/out/schat-core-%(version_legacy)s.%(got_revision)s.exe'),
    masterdest = UploadFileNameLegacy('schat-core-%(version_legacy)s.%(got_revision)s.exe'),
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/win32/out/schat-customize-%(version_legacy)s.%(got_revision)s.exe'),
    masterdest = UploadFileNameLegacy('schat-customize-%(version_legacy)s.%(got_revision)s.exe'),
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/win32/out/schat-runtime-%(version_legacy)s.%(got_revision)s.exe'),
    masterdest = UploadFileNameLegacy('schat-runtime-%(version_legacy)s.%(got_revision)s.exe'),
  ))
  return f


def MakeMacBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(
    name          = 'revision',
    command       = ['bash', 'revision.sh'],
    workdir       = 'build/os/macosx',
    haltOnFailure = True,
    env           = { 'SCHAT_REVISION': Property('got_revision') },
  ))
  f.addStep(ShellCommand(
    name          = 'dmg',
    command       = ['bash', 'deploy.sh'],
    workdir       = 'build/os/macosx',
    haltOnFailure = True,
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/macosx/dmg/SimpleChat2-%(version)s.dmg'),
    masterdest = UploadFileName('SimpleChat2-%(version)s%(suffix)s.dmg'),
  ))
  return f


def MakeMacLegacyBuilder():
  f = BuildFactory()
  f.addSteps(svn_co_legacy)
  f.addStep(ShellCommand(
    name          = 'dmg',
    command       = ['bash', 'deploy.sh'],
    workdir       = 'build/os/macosx',
    haltOnFailure = True,
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/macosx/dmg/SimpleChat-%(version_legacy)s.dmg'),
    masterdest = UploadFileNameLegacy('SimpleChat-%(version_legacy)s.dmg'),
  ))
  return f
  

def MakeSrcBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(
    name          = 'tarball',
    command       = ['bash', 'os/source/tarball.sh'],
    env           = {'SCHAT_SOURCE': WithProperties('schat2-src-%(version)s%(suffix)s')},
    haltOnFailure = True,
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('schat2-src-%(version)s%(suffix)s.tar.bz2'),
    masterdest = UploadFileName('schat2-src-%(version)s%(suffix)s.tar.bz2'),
  ))
  return f


def MakeObsBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(
    name          = 'upload',
    command       = ['bash', 'os/obs/obs-upload.sh', Property('project'), WithProperties('%(version)s.%(got_revision)s')],
    haltOnFailure = True,
    logEnviron    = False,
  ))
  return f


def MakeDevelBuilder():
  f = BuildFactory()
  f.addStep(MasterShellCommand(
    name    = 'Create Update Channel',
    command = [
      'php',
      'update.php',
      '--channel',  Property('channel', default = 'devel'),
      '--version',  Property('version'),
      '--revision', Property('revision'),
      '--os',       Property('os', default = 'win32'),
    ]
  ))
  return f


def MakeReleaseBuilder():
  f = BuildFactory()
  f.addStep(MasterShellCommand(
    name    = 'Upload',
    command = [
      'bash',
      'upload.sh',
      SCHAT_VERSION,
      Property('revision'),
    ],
  ))
  f.addStep(MasterShellCommand(
    name    = 'Create Update Channel',
    command = [
      'php',
      'update.php',
      '--channel',  Property('channel', default = 'stable'),
      '--version',  SCHAT_VERSION,
      '--revision', Property('revision'),
      '--os',       Property('os', default = 'win32,osx,ubuntu'),
    ]
  ))
  f.addStep(MasterShellCommand(
    name    = 'Update Site',
    command = [
      'php',
      'site.php',
      '--version',  SCHAT_VERSION,
    ]
  ))
  return f


def UploadFileName(file):
  return WithProperties(SCHAT_UPLOAD_BASE + '/r%(got_revision)s/' + file)


def UploadFileNameLegacy(file):
  return WithProperties(SCHAT_UPLOAD_BASE_LEGACY + '/r%(got_revision)s/' + file)


c['builders'] = [
  BuilderConfig(name = 'lucid',
    slavenames = ['lucid'],
    factory    = MakeDebBuilder(),
    properties = {
      'codename': 'lucid',
      'arch': 'i386',
    },
  ),
  BuilderConfig(name = 'lucid64',
    slavenames = ['lucid64'],
    factory    = MakeDebBuilder(),
    properties = {
      'codename': 'lucid',
      'arch': 'amd64',
    },
  ),
  BuilderConfig(name = 'precise',
    slavenames = ['precise'],
    factory    = MakeDebBuilder(),
    properties = {
      'codename': 'precise',
      'arch': 'i386',
    },
  ),
  BuilderConfig(name = 'precise64',
    slavenames = ['precise64'],
    factory    = MakeDebBuilder(),
    properties = {
      'codename': 'precise',
      'arch': 'amd64',
    },
  ),
  BuilderConfig(name = 'win32',
    slavenames = ['win32'],
    factory    = MakeWinBuilder(),
  ),
  BuilderConfig(name = 'win32-legacy',
    slavenames = ['win32'],
    factory    = MakeWinLegacyBuilder(),
  ),
  BuilderConfig(name = 'macosx',
    slavenames = ['macosx'],
    factory    = MakeMacBuilder(),
  ),
  BuilderConfig(name = 'macosx-legacy',
    slavenames = ['macosx'],
    factory    = MakeMacLegacyBuilder(),
  ),
  BuilderConfig(name = 'source',
    slavenames = ['master'],
    factory    = MakeSrcBuilder(),
  ),
  BuilderConfig(name = 'release',
    slavenames = ['master'],
    factory    = MakeReleaseBuilder(),
  ),
  BuilderConfig(name = 'ppa',
    slavenames = ['master'],
    factory    = MakePpaBuilder(),
    properties = {
      'ppa': 'stable'
    },
  ),
  BuilderConfig(name = 'ppa-dev',
    slavenames = ['master'],
    factory    = MakePpaBuilder(),
    properties = {
      'ppa': 'development'
    },
  ),
  BuilderConfig(name = 'obs',
    slavenames = ['master'],
    factory    = MakeObsBuilder(),
    properties = {
      'project': 'home:impomezia'
    },
  ),
  BuilderConfig(name = 'obs-dev',
    slavenames = ['master'],
    factory    = MakeObsBuilder(),
    properties = {
      'project': 'home:impomezia:devel'
    },
  ),
]
