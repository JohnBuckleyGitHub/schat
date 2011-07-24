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

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVariant>

#include "schat.h"

class FileLocations;

class SCHAT_EXPORT Settings : public QObject
{
  Q_OBJECT

public:
  Settings(QObject *parent = 0);
  Settings(const QString &group, QObject *parent = 0);
  bool setValue(const QString &key, const QVariant &value, bool notice = true);
  bool setValue(int key, const QVariant &value, bool notice = false);
  FileLocations *locations() { return m_locations; }
  inline void setAutoDefault(bool enable) { m_autoDefault = enable; }
  inline void setGroup(const QString &group) { m_group = group; }
  int setDefault(const QString &key, const QVariant &value);
  QVariant value(int key) const;
  virtual void read(const QString &file = "");
  virtual void write();
  void notify();

signals:
  void changed(const QList<int> &keys);

protected:
  bool m_autoDefault;             ///< Автоматически создавать настройку по умолчанию при установке новой опции.
  QHash<int, QVariant> m_data;    ///< Таблица настроек.
  QHash<int, QVariant> m_default; ///< Настройки по умолчанию.
  QList<int> m_changed;           ///< Список изменившихся настроек.
  QList<int> m_notify;            ///< Список изменившихся настроек.
  QString m_group;                ///< Группа настроек.
  QStringList m_keys;             ///< Символьные ключи настроек.

private:
  void init();

  FileLocations *m_locations;     ///< Пути размещения файлов.
};

#endif /* SETTINGS_H_ */
