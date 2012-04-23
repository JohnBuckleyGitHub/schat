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
SCHAT_RELEASE_BASE   = "/var/www/download.schat.me/htdocs/schat2"

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
  ],
  'change_source': [
    SVNPoller(
      svnurl="http://schat.googlecode.com/svn/trunk/",
      split_file=split_file_alwaystrunk
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
    builderNames=["lucid", "lucid64", "win32"]
  ),
]


svn_co = [
  SVN(mode='full', method='clobber', repourl='http://schat.googlecode.com/svn/trunk/'),
]


def MakeDebBuilder():
  f = BuildFactory()
  f.addSteps(svn_co)
  f.addStep(ShellCommand(name="deb", command=["bash", "build.sh"], workdir="build/os/ubuntu"))
  
  dir = SCHAT_RELEASE_BASE + "/" + SCHAT_VERSION
  f.addStep(FileUpload(
    mode=0644,
    slavesrc=WithProperties("os/ubuntu/deb/schat2_" + SCHAT_VERSION + "-1~%(codename)s_%(arch)s.deb"),
    masterdest=WithProperties(dir + "/schat2_" + SCHAT_VERSION + "-1~%(codename)s_%(arch)s.deb")))
  f.addStep(FileUpload(
    mode=0644,
    slavesrc=WithProperties("os/ubuntu/deb/schatd2_" + SCHAT_VERSION + "-1~%(codename)s_%(arch)s.deb"),
    masterdest=WithProperties(dir + "/schatd2_" + SCHAT_VERSION + "-1~%(codename)s_%(arch)s.deb")))
  return f


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
  
  dir = SCHAT_RELEASE_BASE + "/" + SCHAT_VERSION
  f.addStep(FileUpload(
    mode=0644,
    slavesrc="os/win32/out/schat2-" + SCHAT_VERSION + ".exe",
    masterdest=dir + "/schat2-" + SCHAT_VERSION + ".exe"))
  f.addStep(FileUpload(
    mode=0644,
    slavesrc="os/win32/out/schat2-server-" + SCHAT_VERSION + ".exe",
    masterdest=dir + "/schat2-server-" + SCHAT_VERSION + ".exe"))
  return f


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
    factory=MakeWinBuilder()),
]
