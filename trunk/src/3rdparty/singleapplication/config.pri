LICENSE      = LGPLv2

SINGLEAPPLICATION_LIBNAME = singleapplication

# Uncomment the following line if you want to build a shared library
#SINGLEAPPLICATION_LIBCONFIG = dll

CONFIG       += debug_and_release
CONFIG       *= qt thread warn_on
CONFIG       -= exceptions rtti

DESTDIR      = $$PWD/bin

LIBS         += -L$$DESTDIR


# Configuration for MacOS X
macx {
    # Using gcc
    QMAKESPEC = macx-g++
    # uncoment the following line if you want to use xcode
    # QMAKESPEC = macx-xcode
}

BUILDDIR       = .build/
win32:BUILDDIR = $$join(BUILDDIR,,,win32)
unix:BUILDDIR  = $$join(BUILDDIR,,,unix)
macx:BUILDDIR  = $$join(BUILDDIR,,,macx)

UI_DIR         = $${BUILDDIR}/ui
UIC_DIR        = $${BUILDDIR}/uic
MOC_DIR        = $${BUILDDIR}/moc
RCC_DIR        = $${BUILDDIR}/rcc
OBJECTS_DIR    = $${BUILDDIR}/obj
CONFIG(release, debug|release):OBJECTS_DIR = $$join(OBJECTS_DIR,,,/release)
else:OBJECTS_DIR = $$join(OBJECTS_DIR,,,/debug)

*-g++ {
    QMAKE_CFLAGS_DEBUG     = -O0 -g
    QMAKE_CXXFLAGS_DEBUG   = -O0 -g
    QMAKE_CFLAGS_RELEASE   = -O3 -fomit-frame-pointer
    QMAKE_CXXFLAGS_RELEASE = -O3 -fomit-frame-pointer
}
