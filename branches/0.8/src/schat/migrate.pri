# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2013 IMPOMEZIA <schat@impomezia.com>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, see <http://www.gnu.org/licenses/>.

HEADERS += \
    migrate/JSON.h \
    migrate/k8json/k8json.h \
    migrate/MigrateIntro.h \
    migrate/MigrateWizard.h \

SOURCES += \
    migrate/JSON.cpp \
    migrate/k8json/k8json.cpp \
    migrate/MigrateIntro.cpp \
    migrate/MigrateWizard.cpp \

win32 {
  HEADERS += \
    migrate/Migrate.h \
    migrate/MigratePrepare.h \
    migrate/QProgressIndicator/QProgressIndicator.h \
    migrate/Spinner.h \

  SOURCES += \ 
    migrate/Migrate.cpp \
    migrate/MigratePrepare.cpp \
    migrate/QProgressIndicator/QProgressIndicator.cpp \
    migrate/Spinner.cpp \

}
else {
  DEFINES += SCHAT_NO_AUTO_MIGRATE
  HEADERS += migrate/MigrateManual.h
  SOURCES += migrate/MigrateManual.cpp
}
