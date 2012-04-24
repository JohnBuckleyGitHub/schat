# -*- python -*-
# ex: set syntax=python:

from buildbot.buildslave import BuildSlave
from buildbot.changes.svnpoller import SVNPoller, split_file_alwaystrunk
from buildbot.config import BuilderConfig
from buildbot.process.factory import BuildFactory
from buildbot.process.properties import WithProperties
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
    mode = 0644,
    slavesrc = WithProperties("os/ubuntu/deb/schat2_" + SCHAT_VERSION + "-1~%(codename)s_%(arch)s.deb"),
    masterdest=DebMasterFileName("schat2"),
  ))
  f.addStep(FileUpload(
    mode=0644,
    slavesrc=WithProperties("os/ubuntu/deb/schatd2_" + SCHAT_VERSION + "-1~%(codename)s_%(arch)s.deb"),
    masterdest=DebMasterFileName("schatd2")
  ))
  return f


def DebMasterFileName(base):
  if SCHAT_RELEASE:
    return WithProperties(SCHAT_UPLOAD_BASE + "/" + base + "_" + SCHAT_VERSION + "-1~%(codename)s_%(arch)s.deb")
  else:
    return WithProperties(SCHAT_UPLOAD_BASE + "/" + base + "_" + SCHAT_VERSION + ".%(got_revision)s-1~%(codename)s~dev_%(arch)s.deb")


def MakeWinBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(name="qmake", command=["qmake", "-r"]))
  f.addStep(Compile(command=["nmake"]))
  env = {
    'SCHAT_SIGN_FILE':     schat_passwords.SIGN_FILE,
    'SCHAT_SIGN_PASSWORD': schat_passwords.SIGN_PASSWORD,
    'SCHAT_VERSION':       SCHAT_VERSION,
  }
  f.addStep(ShellCommand(name='sign', command=["cmd", "/c", "sign.cmd"], workdir="build/os/win32", env=env))
  f.addStep(ShellCommand(name='prepare', command=["cmd", "/c", "prepare.cmd"], workdir="build/os/win32"))
  f.addStep(ShellCommand(name='nsis', command=["makensis", "setup.nsi"], workdir="build/os/win32"))
  f.addStep(ShellCommand(name='nsis', command=["makensis", "server.nsi"], workdir="build/os/win32"))
  f.addStep(ShellCommand(name='sign dist', command=["cmd", "/c", "sign_dist.cmd"], workdir="build/os/win32", env=env))
  
  f.addStep(FileUpload(
    mode=0644,
    slavesrc="os/win32/out/schat2-" + SCHAT_VERSION + ".exe",
    masterdest=WinMasterFileName("schat2")))
  f.addStep(FileUpload(
    mode=0644,
    slavesrc="os/win32/out/schat2-server-" + SCHAT_VERSION + ".exe",
    masterdest=WinMasterFileName("schat2-server")))
  return f


def WinMasterFileName(base):
  if SCHAT_RELEASE:
    return SCHAT_UPLOAD_BASE + "/" + base + "-" + SCHAT_VERSION + ".exe"
  else:
    return WithProperties(SCHAT_UPLOAD_BASE + "/" + base + "-" + SCHAT_VERSION + ".%(got_revision)s-dev.exe")


def MakeMacBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(name="dmg", command=["bash", "deploy.sh"], workdir="build/os/macosx"))
  f.addStep(FileUpload(
    mode=0644,
    slavesrc = "os/macosx/dmg/SimpleChat2-" + SCHAT_VERSION + ".dmg",
    masterdest = MacMasterFileName()))
  return f;


def MacMasterFileName():
  if SCHAT_RELEASE:
    return SCHAT_UPLOAD_BASE + "/SimpleChat2-" + SCHAT_VERSION + ".dmg"
  else:
    return WithProperties(SCHAT_UPLOAD_BASE + "/SimpleChat2-" + SCHAT_VERSION + ".%(got_revision)s-dev.dmg")
  

def MakeSrcBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(name="tarball", command=["bash", "os/source/tarball.sh"], env = {'SCHAT_SOURCE': SrcFileName()}))
  f.addStep(FileUpload(
    mode=0644,
    slavesrc=SrcFileName("", ".tar.bz2"),
    masterdest=SrcFileName(SCHAT_UPLOAD_BASE + "/", ".tar.bz2")))
  return f;

def SrcFileName(prefix = "", suffix = ""):
  if SCHAT_RELEASE:
    return prefix + "schat2-src-" + SCHAT_VERSION + suffix
  else:
    return WithProperties(prefix + "schat2-src-" + SCHAT_VERSION + ".%(got_revision)s-dev" + suffix)

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
