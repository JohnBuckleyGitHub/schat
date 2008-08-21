/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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

#ifndef DAEMONSETTINGS_H_
#define DAEMONSETTINGS_H_

#include <QObject>
#include <QMap>
#include <QSettings>

class DaemonSettings : public QObject {
  Q_OBJECT
  
public:
  DaemonSettings(QObject *parent = 0);
  inline bool getBool(const QString &key) const                                                 { return m_bool.value(key); }
  inline int getInt(const QString &key) const                                                   { return m_int.value(key); }
  inline QString getString(const QString &key) const                                            { return m_string.value(key); }
  inline void setBool(const QString &key, bool defValue, const QSettings &settings)             { m_bool[key] = settings.value(key, defValue).toBool(); }
  inline void setBool(const QString &key, bool value)                                           { m_bool[key] = value; }
  inline void setInt(const QString &key, int defValue, const QSettings &settings)               { m_int[key] = settings.value(key, defValue).toInt(); }
  inline void setInt(const QString &key, int value)                                             { m_int[key] = value; }
  inline void setString(const QString &key, const QString &defValue, const QSettings &settings) { m_string[key] = settings.value(key, defValue).toString(); }
  inline void setString(const QString &key, const QString &value)                               { m_string[key] = value; }
  void read();

private:
  QMap<QString, bool> m_bool;
  QMap<QString, int> m_int;
  QMap<QString, QString> m_string;
};

#endif /*DAEMONSETTINGS_H_*/
