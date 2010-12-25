/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>

#include "abstractsettings.h"

AbstractSettings *AbstractSettings::m_self = 0;

/*!
 * \brief Конструктор класса AbstractSettings.
 */
AbstractSettings::AbstractSettings(const QString &fileName, QObject *parent)
  : QObject(parent)
{
  Q_ASSERT_X(!m_self, "AbstractSettings", "there should be only one settings object");
  m_self = this;

  m_settings = new QSettings(fileName, QSettings::IniFormat, this);
  #if QT_VERSION >= 0x040500
    m_settings->setIniCodec("UTF-8");
  #endif
}


/*!
 * Проверка на новый год.
 */
bool AbstractSettings::isNewYear()
{
  static bool init;
  static bool result;

  if (!init) {
    init = true;
    QDate date = QDate::currentDate();
    if (date.month() == 12 && date.day() > 19)
      result = true;
    else if (date.month() == 1 && date.day() < 15)
      result = true;
    else
      result = false;
  }

  return result;
}


/*!
 * Проверка пути, если название вышестоящего каталога равно bin, то функция возвращает true.
 */
bool AbstractSettings::isUnixLike()
{
  #if defined(Q_OS_MAC)
  return true;
  #else
  if (QDir(QCoreApplication::applicationDirPath()).dirName() == "bin")
    return true;
  #endif

  return false;
}


/*!
 * Чтение настроек.
 */
void AbstractSettings::read(const QSettings *s)
{
  if (!m_bool.isEmpty()) {
    QMapIterator<QString, bool> i(m_bool);
    while (i.hasNext()) {
      i.next();
      m_bool[i.key()] = s->value(i.key(), i.value()).toBool();
    }
  }

  if (!m_int.isEmpty()) {
    QMapIterator<QString, int> i(m_int);
    while (i.hasNext()) {
      i.next();
      m_int[i.key()] = s->value(i.key(), i.value()).toInt();
    }
  }

  if (!m_string.isEmpty()) {
    QMapIterator<QString, QString> i(m_string);
    while (i.hasNext()) {
      i.next();
      m_string[i.key()] = s->value(i.key(), i.value()).toString();
    }
  }

  if (!m_list.isEmpty()) {
    QMapIterator<QString, QStringList> i(m_list);
    while (i.hasNext()) {
      i.next();
      QStringList list = s->value(i.key(), i.value()).toStringList();
      if (list.size() == 1)
        if (list.at(0) == "empty")
          list.clear();

      m_list[i.key()] = list;
    }
  }
}


/*!
 * Запись настроек.
 */
#ifndef SCHAT_NO_WRITE_SETTINGS
void AbstractSettings::write(QSettings *s, bool sync)
{
  if (!m_bool.isEmpty()) {
    QMapIterator<QString, bool> i(m_bool);
    while (i.hasNext()) {
      i.next();
      if (!m_ro.contains(i.key()))
        s->setValue(i.key(), i.value());
    }
  }

  if (!m_int.isEmpty()) {
    QMapIterator<QString, int> i(m_int);
    while (i.hasNext()) {
      i.next();
      if (!m_ro.contains(i.key()))
        s->setValue(i.key(), i.value());
    }
  }

  if (!m_string.isEmpty()) {
    QMapIterator<QString, QString> i(m_string);
    while (i.hasNext()) {
      i.next();
      if (!m_ro.contains(i.key()))
        s->setValue(i.key(), i.value());
    }
  }

  if (!m_list.isEmpty()) {
    QMapIterator<QString, QStringList> i(m_list);
    while (i.hasNext()) {
      i.next();
      if (!m_ro.contains(i.key())) {
        QStringList list = i.value();
        if (list.isEmpty())
          list << "empty";

        s->setValue(i.key(), list);
      }
    }
  }

  foreach (QString remove, m_remove)
    m_settings->remove(remove);

  if (sync)
    m_settings->sync();
}
#endif
