/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include "Settings.h"

Settings::Settings(QObject *parent)
  : QObject(parent)
  , m_autoDefault(false)
{
  init();
}


Settings::Settings(const QString &group, QObject *parent)
  : QObject(parent)
  , m_autoDefault(true)
  , m_group(group)
{
  init();
}


/*!
 * Установка значения по умолчанию.
 *
 * \param key   Текстовый ключ настройки.
 * \param value Значение настройки.
 *
 * \return Числовой ключ настройки или -1 в случае ошибки.
 */
int Settings::setDefault(const QString &key, const QVariant &value)
{
  int index = m_keys.size();
  if (m_default.contains(index))
    return -1;

  m_keys.append(key);
  m_default.insert(index, value);
  return index;
}


QVariant Settings::value(int key) const
{
  return m_data.value(key);
}


void Settings::notify()
{
  if (!m_notify.isEmpty()) {
    emit changed(m_notify);
    m_notify.clear();
  }
}


/*!
 * Чтение настроек.
 */
void Settings::read(const QString &file)
{
  Q_UNUSED(file)

  QSettings settings(m_confFile, QSettings::IniFormat);
  settings.setIniCodec("UTF-8");

  if (!m_group.isEmpty())
    settings.beginGroup(m_group);

  for (int i = 0; i < m_keys.size(); ++i) {
    m_data[i] = settings.value(m_keys.at(i), m_default.value(i));
  }

  if (!m_group.isEmpty())
    settings.endGroup();
}


/*!
 * Установка опции.
 */
void Settings::setValue(const QString &key, const QVariant &value, bool notice)
{
  if (!m_keys.contains(key)) {
    if (m_autoDefault)
      setDefault(key, value);
    else
      return;
  }

  setValue(m_keys.indexOf(key), value, notice);
}


/*!
 * Установка опции.
 */
void Settings::setValue(int key, const QVariant &value, bool notice)
{
  if (!m_data.contains(key) && !m_autoDefault)
    return;

  if (m_data.value(key) == value)
    return;

  m_data[key] = value;

  if (!m_changed.contains(key))
    m_changed.append(key);

  if (!m_notify.contains(key))
    m_notify.append(key);

  if (notice)
    notify();
}


/*!
 * Запись настроек.
 */
void Settings::write()
{
  QSettings settings(m_confFile, QSettings::IniFormat);
  settings.setIniCodec("UTF-8");

  if (!m_group.isEmpty())
    settings.beginGroup(m_group);

  for (int i = 0; i < m_keys.size(); ++i) {
    QVariant data = m_data.value(i);
    if (m_changed.contains(i) || data != m_default.value(i))
      settings.setValue(m_keys.at(i), m_data.value(i));
  }

  if (!m_group.isEmpty())
    settings.endGroup();
}


void Settings::init()
{
  m_appDirPath = QCoreApplication::applicationDirPath();
  m_baseName = QFileInfo(QCoreApplication::applicationFilePath()).baseName();

  #if defined(Q_WS_MAC)
  m_scheme = AppBundle;
  #else
  if (m_appDirPath == "/usr/bin" || m_appDirPath == "/usr/sbin")
    m_scheme = UnixStandard;
  else if (QDir(m_appDirPath).dirName() == "bin")
    m_scheme = UnixAdaptive;
  else {
    m_scheme = Portable;
  }
  #endif

  if (m_scheme == UnixStandard || m_scheme == UnixAdaptive)
    m_root = QDir::homePath() + "/.config/" + m_baseName;
  else if (m_scheme == AppBundle)
    m_root = QDir::cleanPath(m_appDirPath + "/../Resources");
  else
    m_root = m_appDirPath;

  m_confFile = m_root + "/" + m_baseName + ".conf";
}
