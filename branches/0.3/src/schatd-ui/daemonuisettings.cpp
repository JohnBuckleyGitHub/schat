/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QtGui>

#include "daemonuisettings.h"

/*!
 * \brief Конструктор класса DaemonUiSettings.
 */
DaemonUiSettings::DaemonUiSettings(const QString &filename, QObject *parent)
  : DaemonSettings(filename, parent)
{
}


/*!
 * Чтение настроек.
 */
void DaemonUiSettings::read()
{
  DaemonSettings::read();

  QSettings s(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);
  qApp->setStyle(s.value("Style", "Plastique").toString());
}


/*!
 * Запись настроек.
 */
void DaemonUiSettings::write()
{
  if (!m_string.isEmpty()) {
    QMapIterator<QString, QString> i(m_string);
    while (i.hasNext()) {
      i.next();
      m_settings->setValue(i.key(), i.value());
    }
  }

  if (!m_bool.isEmpty()) {
    QMapIterator<QString, bool> i(m_bool);
    while (i.hasNext()) {
      i.next();
      m_settings->setValue(i.key(), i.value());
    }
  }

  if (!m_int.isEmpty()) {
    QMapIterator<QString, int> i(m_int);
    while (i.hasNext()) {
      i.next();
      m_settings->setValue(i.key(), i.value());
    }
  }
}