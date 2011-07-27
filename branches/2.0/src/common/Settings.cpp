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

#include "FileLocations.h"
#include "Settings.h"

SettingsLegacy::SettingsLegacy(QObject *parent)
  : QObject(parent)
  , m_autoDefault(false)
{
  init();
}


SettingsLegacy::SettingsLegacy(const QString &group, QObject *parent)
  : QObject(parent)
  , m_autoDefault(true)
  , m_group(group)
{
  init();
}


/*!
 * Установка опции.
 *
 * \return true если опция была изменена.
 */
bool SettingsLegacy::setValue(const QString &key, const QVariant &value, bool notice)
{
  if (!m_keys.contains(key)) {
    if (m_autoDefault)
      setDefault(key, value);
    else
      return false;
  }

  return setValue(m_keys.indexOf(key), value, notice);
}


/*!
 * Установка опции.
 *
 * \return true если опция была изменена.
 */
bool SettingsLegacy::setValue(int key, const QVariant &value, bool notice)
{
  if (!m_data.contains(key) && !m_autoDefault)
    return false;

  if (m_data.value(key) == value)
    return false;

  m_data[key] = value;

  if (!m_changed.contains(key))
    m_changed.append(key);

  if (notice && !m_notify.contains(key))
    m_notify.append(key);

  if (notice)
    notify();

  return true;
}


/*!
 * Установка значения по умолчанию.
 *
 * \param key   Текстовый ключ настройки.
 * \param value Значение настройки.
 *
 * \return Числовой ключ настройки или -1 в случае ошибки.
 */
int SettingsLegacy::setDefault(const QString &key, const QVariant &value)
{
  int index = m_keys.size();
  if (m_default.contains(index))
    return -1;

  m_keys.append(key);
  m_default.insert(index, value);
  return index;
}


QVariant SettingsLegacy::value(int key) const
{
  return m_data.value(key);
}


/*!
 * Чтение настроек.
 */
void SettingsLegacy::read(const QString &file)
{
  Q_UNUSED(file)

  QSettings settings(m_locations->path(FileLocations::ConfigFile), QSettings::IniFormat);
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
 * Запись настроек.
 */
void SettingsLegacy::write()
{
  QSettings settings(m_locations->path(FileLocations::ConfigFile), QSettings::IniFormat);
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


void SettingsLegacy::notify()
{
  if (!m_notify.isEmpty()) {
    emit changed(m_notify);
    m_notify.clear();
  }
}


/*!
 * Определение схемы размещения файлов и стандартных путей.
 */
void SettingsLegacy::init()
{
  m_locations = new FileLocations(this);
}


Settings::Settings(const QString &fileName, QObject *parent)
  : QSettings(fileName, QSettings::IniFormat, parent)
{
  setIniCodec("UTF-8");
}


QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
  m_default[key] = defaultValue;
  return QSettings::value(key, defaultValue);
}


void Settings::setDefault(const QString &key, const QVariant &value)
{
  m_default[key] = value;
}


void Settings::setValue(const QString &key, const QVariant &value)
{
  if (value == QSettings::value(key, m_default.value(key)))
    return;

  if (m_default.contains(key) && m_default.value(key) == value)
    QSettings::remove(key);
  else
    QSettings::setValue(key, value);

  emit changed(key, value);
}
