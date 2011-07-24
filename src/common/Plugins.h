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
#include <QStringList>

#include "schat.h"

class CoreInterface;

class SCHAT_EXPORT Plugins : public QObject
{
  Q_OBJECT

public:
  Plugins(QObject *parent = 0);
  bool addProvider(const QString &name);
  void load();

protected:
  virtual void init() {}

  QHash<QString, QObject *> m_providers;
  QList<QObject *> m_plugins; ///< Все загруженные плагины.

private:
  CoreInterface* checkPlugin(QObject *plugin);
  void load(const QString &path);
  void sort();

  QStringList m_ids;
  QStringList m_providersList;
};

#endif /* PLUGINS_H_ */
