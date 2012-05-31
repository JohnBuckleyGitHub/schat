/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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
#include "sglobal.h"
#include "tools/Ver.h"

/*!
 * Загружает плагин с именем \p fileName.
 *
 * В случае успешной загрузки метод isValid() вернёт true.
 */
PluginItem::PluginItem(const QString &fileName)
  : m_valid(false)
  , m_plugin(0)
{
  m_loader.setFileName(fileName);
  if (!m_loader.load())
    return;

  m_plugin = m_loader.instance();
  if (!m_plugin)
    return;

  CoreApi *api = qobject_cast<CoreApi *>(m_plugin);
  if (!api)
    return;

  m_header = api->header();
  if (m_header.value(LS("Id")).toString().isEmpty())
    return;

  if (m_header.value(LS("Name")).toString().isEmpty())
    return;

  m_valid = true;
}


PluginItem::~PluginItem()
{
  if (m_loader.isLoaded())
    m_loader.unload();
}


Plugins::Plugins(QObject *parent)
  : QObject(parent)
{
  m_min = LS("1.99.8");
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

  init();
}


/*!
 * Выполняет проверку интерфейса плагина.
 *
 * \return true если плагин прошёл проверку.
 */
bool Plugins::check(PluginItem *plugin)
{
  if (m_plugins.contains(plugin->id()))
    return false;

  if (m_type != plugin->header().value(LS("Type")).toString())
    return false;

  Ver required = Ver(plugin->header().value(LS("Required")).toString());

  // Отклоняем плагин если он требует более новую версию ядра чем имеется.
  if (Ver::current() < required)
    return false;

  // Отклоняем плагин если он требует версию, ниже разрещённой.
  if (Ver(m_min) > required)
    return false;

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
  filters.append(LS("*.dll"));
# elif defined(Q_WS_MAC)
  filters.append(LS("*.dylib"));
  filters.append(LS("*.so"));
# else
  filters.append(LS("*.so"));
# endif

  QStringList files = dir.entryList(filters, QDir::Files);

  for (int i = 0; i < files.size(); ++i) {
    PluginItem *item = new PluginItem(dir.absoluteFilePath(files.at(i)));
    if (!item->isValid()) {
      delete item;
      continue;
    }

    if (!check(item)) {
      delete item;
      continue;
    }

    m_plugins[item->id()] = item;
    m_sorted.append(item->id());
  }
}
