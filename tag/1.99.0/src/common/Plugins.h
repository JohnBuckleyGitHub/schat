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

#ifndef PLUGINS_H_
#define PLUGINS_H_

#include <QHash>
#include <QObject>
#include <QPluginLoader>
#include <QStringList>

#include "schat.h"

class CoreApi;

/*!
 * Загружает плагин и хранит о нём информацию.
 */
class SCHAT_EXPORT PluginItem
{
public:
  PluginItem(const QString &fileName);
  ~PluginItem();
  inline bool isValid() const { return m_valid; }
  inline CoreApi *api() { return m_api; }
  inline QObject *plugin() { return m_plugin; }
  QString id() const;

private:
  bool m_valid;
  CoreApi *m_api;
  QObject *m_plugin;
  QPluginLoader m_loader;
};


/*!
 * Базовый класс, обеспечивающий загрузку плагинов.
 */
class SCHAT_EXPORT Plugins : public QObject
{
  Q_OBJECT

public:
  Plugins(QObject *parent = 0);
  bool addProvider(const QString &name);
  void load();

protected:
  virtual void init() {}

  QHash<QString, PluginItem *> m_plugins;   ///< Таблица плагинов.
  QHash<QString, PluginItem *> m_providers; ///< Поддерживаемые провайдеры.
  QStringList m_sorted;                     ///< Сортированный список плагинов, этот список определяет очерёдность загрузки наследниками этого класса.

private:
  bool checkPlugin(PluginItem *plugin);
  void load(const QString &path);
  void sort();

  QStringList m_providersList;
};

#endif /* PLUGINS_H_ */
