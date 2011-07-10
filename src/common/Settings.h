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

class Settings : public QObject
{
  Q_OBJECT

public:
  /// Схема размещения файлов.
  enum FileScheme {
    Portable,     ///< Стандартное размещение файлов, когда все файлы находятся в папке с исполняемым файлом чата, под Windows схема обеспечивает переносимость без установки.
    UnixAdaptive, ///< Размещение файлов в unix стиле, если исполняемый файл находится в папке bin.
    UnixStandard, ///< Размещение с использованием абсолютных путей если исполняемый файл размещается в /usr/bin или /usr/sbin.
    AppBundle     ///< Размещение специфичное для Mac OS X.
  };

  Settings(QObject *parent = 0);
  Settings(const QString &group, QObject *parent = 0);
  bool setValue(const QString &key, const QVariant &value, bool notice = true);
  bool setValue(int key, const QVariant &value, bool notice = false);
  inline FileScheme fileScheme() const { return m_scheme; }
  inline FileScheme scheme() const { return m_scheme; }
  inline QString baseName() const { return m_baseName; }
  inline QString confFile() const { return m_confFile; }
  inline QString root() const { return m_root; }
  inline QString share() const { return m_share; }
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
  QString m_appDirPath;           ///< Папка с исполняемым файлом чата.
  QString m_baseName;             ///< Имя исполняемого файла без расширения.
  QString m_confFile;             ///< Основной конфигурационный файл.
  QString m_group;                ///< Группа настроек.
  QString m_root;                 ///< Корневая директория настроек.
  QString m_share;                ///< Общие файлы.
  QStringList m_keys;             ///< Символьные ключи настроек.

private:
  void init();

  FileScheme m_scheme; ///< Схема размещения файлов.
};

#endif /* SETTINGS_H_ */
