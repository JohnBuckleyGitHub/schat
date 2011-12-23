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

#ifndef NODEAPI_H_
#define NODEAPI_H_

#include <QObject>

class Core;
class NodePlugin;

class NodeApi
{
public:
  virtual ~NodeApi() {}
  virtual NodePlugin *init(Core *core) = 0;
  inline NodePlugin *plugin() const { return m_plugin; }

protected:
  NodePlugin *m_plugin;
};

Q_DECLARE_INTERFACE(NodeApi, "me.schat.NodeApi/1.0");


#endif /* NODEAPI_H_ */
