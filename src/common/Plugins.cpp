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

#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>

#include "Plugins.h"
#include "plugins/CoreApi.h"

Plugins::Plugins(QObject *parent)
  : QObject(parent)
{
}


/*!
 * Добавляет провайдера в список поддерживаемых,
 * эта функция должна вызываться до загрузки плагинов.
 */
bool Plugins::addProvider(const QString &name)
{
  if (m_providers.contains(name) || m_providersList.contains(name))
    return false;

  m_providers.insert(name, 0);
  m_providersList.append(name);
  return true;
}


/*!
 * Загрузка плагинов.
 */
void Plugins::load()
{
  foreach (const QString &path, QCoreApplication::libraryPaths())
    load(path);

  m_ids.clear();

  sort();
  init();
}


/*!
 * Выполняет проверку интерфейса плагина.
 *
 * \return true если плагин прошёл проверку.
 */
CoreApi* Plugins::checkPlugin(QObject *plugin)
{
  if (!plugin)
    return 0;

  CoreApi *core = qobject_cast<CoreApi *>(plugin);
  if (!core)
    return 0;

  if (core->id().isEmpty() || core->name().isEmpty())
    return 0;

  if (m_ids.contains(core->id()))
    return 0;

  QStringList provides = core->provides();
  if (provides.isEmpty()) {
    return core;
  }

  QString provider;
  for (int i = 0; i < provides.size(); ++i) {
    provider = provides.at(i);
    if (!m_providers.contains(provider))
      return 0;

    if (m_providers.value(provider))
      return 0;

    m_providers[provider] = plugin;
  }

  return core;
}



/*!
 * Загрузка плагинов с интерфейсом CoreApi из папки \p path
 * в список плагинов \p m_plugins.
 */
void Plugins::load(const QString &path)
{
  QDir dir(path);

  QStringList files = dir.entryList(QDir::Files);

  for (int i = 0; i < files.size(); ++i) {
    QPluginLoader loader(dir.absoluteFilePath(files.at(i)));

    QObject *plugin = loader.instance();
    CoreApi *core = checkPlugin(plugin);
    if (!core) {
      loader.unload();
      continue;
    }

    m_ids.append(core->id());
    m_plugins.append(plugin);
  }
}


/*!
 * Выполняет сортировку списка плагинов.
 */
void Plugins::sort()
{
  if (m_providersList.isEmpty())
    return;

  QObject *core = 0;

  for (int i = m_providersList.size() - 1; i >= 0; --i) {
    core = m_providers.value(m_providersList.at(i));
    if (!core)
      continue;

    int index = m_plugins.indexOf(core);
    if (index == -1)
      continue;

    m_plugins.swap(0, index);
  }

  m_providersList.clear();
}
