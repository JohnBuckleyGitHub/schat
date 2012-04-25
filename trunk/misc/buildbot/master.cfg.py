# -*- python -*-
# ex: set syntax=python:

from buildbot.buildslave import BuildSlave
from buildbot.changes.svnpoller import SVNPoller, split_file_alwaystrunk
from buildbot.config import BuilderConfig
from buildbot.process.factory import BuildFactory
from buildbot.process.properties import WithProperties, Property
from buildbot.schedulers.forcesched import ForceScheduler
from buildbot.status import html, web
from buildbot.steps.shell import Compile, ShellCommand
from buildbot.steps.source.svn import SVN
from buildbot.steps.transfer import FileUpload

import schat_passwords
import os

SCHAT_VERSION        = "1.99.25"
SCHAT_RELEASE        = False
SCHAT_UPLOAD_BASE    = "/var/www/download.schat.me/htdocs/schat2/"

if SCHAT_RELEASE:
  SCHAT_UPLOAD_BASE = SCHAT_UPLOAD_BASE + SCHAT_VERSION
else:
  SCHAT_UPLOAD_BASE = SCHAT_UPLOAD_BASE + "snapshots/" + SCHAT_VERSION

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
  'titleURL':     "http://schat.me",
  'buildbotURL':  "http://buildbot.schat.me/",
  'slavePortnum': 9989,
  'slaves': [
    BuildSlave("lucid",   schat_passwords.LUCID),
    BuildSlave("lucid64", schat_passwords.LUCID64),
    BuildSlave("win32",   schat_passwords.WIN32),
    BuildSlave("macosx",  schat_passwords.MACOSX),
    BuildSlave("master",  schat_passwords.MASTER),
  ],
  'change_source': [
    SVNPoller(
      svnurl="http://schat.googlecode.com/svn/trunk/",
      split_file=split_file_alwaystrunk,
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
  'suffix':  ''
}


if not SCHAT_RELEASE:
  c['properties']['suffix'] = '-dev'


c['schedulers'] = [
  ForceScheduler(
    name="force",
    builderNames=["lucid", "lucid64", "win32", "macosx", "source"]
  ),
]


svn_co = [
  SVN(mode='full', method='clobber', repourl='http://schat.googlecode.com/svn/trunk/'),
]


def MakeDebBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(name="deb", command=["bash", "build.sh"], workdir="build/os/ubuntu"))
  
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


def MakeWinBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(
    name          = 'revision',
    command       = ['cmd', '/c', 'revision.cmd'],
    workdir       = "build/os/win32",
    env           = { 'SCHAT_REVISION': Property('got_revision') },
    haltOnFailure = True,
  ))
  f.addStep(ShellCommand(
    name          = 'qmake',
    command       = ['qmake', '-r'],
    haltOnFailure = True,
  ))
  f.addStep(Compile(
    command       = ['nmake'],
    haltOnFailure = True,
  ))
  env = {
    'SCHAT_SIGN_FILE':     schat_passwords.SIGN_FILE,
    'SCHAT_SIGN_PASSWORD': schat_passwords.SIGN_PASSWORD,
    'SCHAT_VERSION':       SCHAT_VERSION,
  }
  f.addStep(ShellCommand(
    name          = 'sign',
    command       = ['cmd', '/c', 'sign.cmd'],
    workdir       = 'build/os/win32',
    env           = env,
    haltOnFailure = True,
  ))
  f.addStep(ShellCommand(
    name          = 'prepare',
    command       = ['cmd', '/c', 'prepare.cmd'],
    workdir       = 'build/os/win32',
    haltOnFailure = True,
  ))
  f.addStep(ShellCommand(
    name          = 'nsis',
    command       = ['makensis', 'setup.nsi'],
    workdir       = 'build/os/win32',
    haltOnFailure = True,
  ))
  f.addStep(ShellCommand(
    name          = 'nsis',
    command       = ['makensis', 'server.nsi'],
    workdir       = 'build/os/win32',
    haltOnFailure = True,
  ))
  f.addStep(ShellCommand(
    name          = 'sign dist',
    command       = ['cmd', '/c', 'sign_dist.cmd'],
    workdir       = 'build/os/win32',
    env           = env,
    haltOnFailure = True,
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
  return f


def MakeMacBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(name='dmg', command=['bash', 'deploy.sh'], workdir='build/os/macosx'))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('os/macosx/dmg/SimpleChat2-%(version)s.dmg'),
    masterdest = UploadFileName('SimpleChat2-%(version)s%(suffix)s.dmg'),
  ))
  return f;
  

def MakeSrcBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(
    name    = 'tarball',
    command = ['bash', 'os/source/tarball.sh'],
    env     = {'SCHAT_SOURCE': WithProperties('schat2-src-%(version)s%(suffix)s')},
  ))
  f.addStep(FileUpload(
    mode       = 0644,
    slavesrc   = WithProperties('schat2-src-%(version)s%(suffix)s.tar.bz2'),
    masterdest = UploadFileName('schat2-src-%(version)s%(suffix)s.tar.bz2'),
  ))
  return f;


def UploadFileName(file):
  if SCHAT_RELEASE:
    return WithProperties(SCHAT_UPLOAD_BASE + '/' + file)
  else:
    return WithProperties(SCHAT_UPLOAD_BASE + '/r%(got_revision)s/' + file)


c['builders'] = [
  BuilderConfig(name="lucid",
    slavenames=["lucid"],
    factory=MakeDebBuilder(),
    properties={
      'codename': "lucid",
      'arch': "i386",
  }),
  BuilderConfig(name="lucid64",
    slavenames=["lucid64"],
    factory=MakeDebBuilder(),
    properties={
      'codename': "lucid",
      'arch': "amd64",
  }),
  BuilderConfig(name="win32",
    slavenames=["win32"],
    factory=MakeWinBuilder()
  ),
  BuilderConfig(name="macosx",
    slavenames=["macosx"],
    factory=MakeMacBuilder()
  ),
  BuilderConfig(name="source",
    slavenames=["master"],
    factory=MakeSrcBuilder()
  ),
]
