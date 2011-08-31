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

#ifndef MESSAGELOGPLUGIN_P_H_
#define MESSAGELOGPLUGIN_P_H_

#include "plugins/NodePlugin.h"

class MessageHook;

class MessageLog : public NodePlugin
{
  Q_OBJECT

public:
  MessageLog(Core *core);
  HookResult hook(const NodeHook &data);
  QList<NodeHook::Type> hooks() const;

private:
  void add(const MessageHook &data);
  void open();

  bool m_isOpen;     ///< true если база открыта.
  bool m_logPrivate;
  bool m_logPublic;
  QString m_id;
};


#endif /* MESSAGELOGPLUGIN_P_H_ */
