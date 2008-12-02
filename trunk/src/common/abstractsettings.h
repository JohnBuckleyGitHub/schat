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
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ABSTRACTSETTINGS_H_
#define ABSTRACTSETTINGS_H_

#include <QMap>
#include <QSettings>
#include <QStringList>

/*!
 * \brief Абстрактный базовый класс для чтения и записи настроек.
 *
 * Предоставляет методы для получения настроек, типов \a bool, \a int и \a QString.
 */
class AbstractSettings : public QObject {
  Q_OBJECT

public:
  AbstractSettings(const QString &filename, QObject *parent = 0);
  inline bool getBool(const QString &key) const                       { return m_bool.value(key); }
  inline int getInt(const QString &key) const                         { return m_int.value(key); }
  inline QString getString(const QString &key) const                  { return m_string.value(key); }
  inline QStringList getList(const QString &key) const                { return m_list.value(key); }
  inline void setBool(const QString &key, bool value)                 { m_bool[key] = value; }
  inline void setInt(const QString &key, int value)                   { m_int[key] = value; }
  inline void setList(const QString &key, const QStringList &value)   { m_list[key] = value; }
  inline void setString(const QString &key, const QString &value)     { m_string[key] = value; }
  static AbstractSettings *instance()                                 { return m_self; }
  void read()                                                         { read(m_settings); }

  #ifndef SCHAT_NO_WRITE_SETTINGS
    void write() { write(m_settings); }
  #endif

protected:
  void read(const QSettings *s);

  #ifndef SCHAT_NO_WRITE_SETTINGS
    void write(QSettings *s);
  #endif

  QMap<QString, bool> m_bool;
  QMap<QString, int> m_int;
  QMap<QString, QString> m_string;
  QMap<QString, QStringList> m_list;
  QSettings *m_settings;

  #ifndef SCHAT_NO_WRITE_SETTINGS
    QStringList m_ro;
  #endif

private:
  static AbstractSettings *m_self;
};

#endif /*ABSTRACTSETTINGS_H_*/
