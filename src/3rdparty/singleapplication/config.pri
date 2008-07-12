LICENSE		= LGPLv2

# Chose one of the following two lines to configure the build
#SINGLEAPPLICATION_LIBCONFIG = dll
SINGLEAPPLICATION_LIBCONFIG = staticlib

SINGLEAPPLICATION_LIBNAME = singleapplication

CONFIG		+= debug_and_release


CONFIG		*= qt thread warn_on
CONFIG		-= exceptions rtti
QT		*= core gui

DESTDIR		= $$PWD/bin

LIBS		+= -L$$DESTDIR


# Configuration for MacOS X
macx {
	# Using gcc
	QMAKESPEC=macx-g++
	# uncoment this line if you want use xcode
	# QMAKESPEC=macx-xcode
}

BUILDDIR	= .build/
win32:BUILDDIR	= $$join(BUILDDIR,,,win32)
unix:BUILDDIR	= $$join(BUILDDIR,,,unix)
macx:BUILDDIR	= $$join(BUILDDIR,,,macx)

UI_DIR		= $${BUILDDIR}/ui
UIC_DIR		= $${BUILDDIR}/uic
MOC_DIR		= $${BUILDDIR}/moc
RCC_DIR		= $${BUILDDIR}/rcc
OBJECTS_DIR	= $${BUILDDIR}/obj
CONFIG(release, debug|release):OBJECTS_DIR = $$join(OBJECTS_DIR,,,/release)
else:OBJECTS_DIR = $$join(OBJECTS_DIR,,,/debug)

*-g++ {
	QMAKE_CFLAGS_DEBUG	= -O0 -g
	QMAKE_CXXFLAGS_DEBUG	= -O0 -g
	QMAKE_CFLAGS_RELEASE	= -O3 -fomit-frame-pointer
	QMAKE_CXXFLAGS_RELEASE	= -O3 -fomit-frame-pointer
}
