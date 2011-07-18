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
#include "plugins/CoreInterface.h"

Plugins::Plugins(QObject *parent)
  : QObject(parent)
{
}


/*!
 * Загрузка плагинов.
 */
void Plugins::load()
{
  foreach (const QString &path, QCoreApplication::libraryPaths())
    load(path);

  init();
}



/*!
 * Загрузка плагинов с интерфейсом CoreInterface из папки \p path
 * в список плагинов \p m_plugins.
 */
void Plugins::load(const QString &path)
{
  QDir dir(path);

  QStringList files = dir.entryList(QDir::Files | QDir::NoDot | QDir::NoDotDot);

  for (int i = 0; i < files.size(); ++i) {
    QPluginLoader loader(dir.absoluteFilePath(files.at(i)));
    QObject *plugin = loader.instance();

    if (!plugin)
      continue;

    CoreInterface *core = qobject_cast<CoreInterface *> (plugin);
    if (!core) {
      loader.unload();
      continue;
    }

    if (core->id().isEmpty() || core->name().isEmpty()) {
      loader.unload();
      continue;
    }

    QStringList provides = core->provides();
    if (!provides.isEmpty()) {
      bool error = false;

      QString provider;
      for (int i = 0; i < provides.size(); ++i) {
        provider = provides.at(i);
        if (!m_providers.contains(provider)) {
          error = true;
          break;
        }

        if (m_providers.value(provider)) {
          error = true;
          break;
        }

        m_providers[provider] = core;
      }

      if (error) {
        loader.unload();
        continue;
      }
    }

    qDebug() << core->id() << core->name() << core->version();
    m_plugins.append(core);
  }
}
