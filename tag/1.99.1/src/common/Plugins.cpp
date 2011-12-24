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

/*!
 * Загружает плагин с именем \p fileName.
 *
 * В случае успешной загрузки метод isValid() вернёт true.
 */
PluginItem::PluginItem(const QString &fileName)
  : m_valid(false)
  , m_api(0)
  , m_plugin(0)
{
  m_loader.setFileName(fileName);
  if (!m_loader.load())
    return;

  m_plugin = m_loader.instance();
  if (!m_plugin)
    return;

  m_api = qobject_cast<CoreApi *>(m_plugin);
  if (!m_api)
    return;

  if (m_api->id().isEmpty() || m_api->name().isEmpty())
    return;

  qDebug() << "ITEM CREATED" << m_api->id() << m_api->version();
  m_valid = true;
}


PluginItem::~PluginItem()
{
  if (m_loader.isLoaded())
    m_loader.unload();
}


QString PluginItem::id() const
{
  if (isValid())
    return m_api->id();

  return QString();
}


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
  QStringList paths = QCoreApplication::libraryPaths();
  paths.removeAll(QCoreApplication::applicationDirPath());

  foreach (const QString &path, paths) {
    load(path);
  }

  sort();
  init();
}


/*!
 * Выполняет проверку интерфейса плагина.
 *
 * \return true если плагин прошёл проверку.
 */
bool Plugins::checkPlugin(PluginItem *plugin)
{
  if (m_plugins.contains(plugin->id()))
    return false;

  QStringList provides = plugin->api()->provides();
  if (provides.isEmpty())
    return true;

  QString provider;
  for (int i = 0; i < provides.size(); ++i) {
    provider = provides.at(i);
    if (!m_providers.contains(provider))
      return false;

    if (m_providers.value(provider))
      return false;

    m_providers[provider] = plugin;
  }

  return true;
}



/*!
 * Загрузка плагинов с интерфейсом CoreApi из папки \p path
 * в список плагинов \p m_plugins.
 */
void Plugins::load(const QString &path)
{
  QDir dir(path);
  QStringList filters;

# if defined(Q_WS_WIN) || defined(Q_OS_SYMBIAN)
  filters.append(QLatin1String("*.dll"));
# elif defined(Q_WS_MAC)
  filters.append(QLatin1String("*.dylib"));
  filters.append(QLatin1String("*.so"));
# else
  filters.append(QLatin1String("*.so"));
# endif

  QStringList files = dir.entryList(filters, QDir::Files);

  for (int i = 0; i < files.size(); ++i) {
    PluginItem *item = new PluginItem(dir.absoluteFilePath(files.at(i)));
    if (!item->isValid()) {
      delete item;
      continue;
    }

    if (!checkPlugin(item)) {
      delete item;
      continue;
    }

    m_plugins[item->id()] = item;
    m_sorted.append(item->id());
  }
}


/*!
 * Выполняет сортировку списка плагинов.
 */
void Plugins::sort()
{
  if (m_providersList.isEmpty())
    return;

  PluginItem *item = 0;

  for (int i = m_providersList.size() - 1; i >= 0; --i) {
    item = m_providers.value(m_providersList.at(i));
    if (!item)
      continue;

    int index = m_sorted.indexOf(item->id());
    if (index == -1)
      continue;

    m_sorted.swap(0, index);
  }

  m_providersList.clear();
  qDebug() << m_sorted;
}
