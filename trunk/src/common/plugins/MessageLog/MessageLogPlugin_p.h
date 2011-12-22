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
class Settings;
class UserHook;

class MessageLog : public NodePlugin
{
  Q_OBJECT

public:
  MessageLog(Core *core);
  HookResult hook(const NodeHook &data);
  QList<NodeHook::Type> hooks() const;
  void reload();

private:
  void cleanup(const QByteArray &destId);
  void open();

  bool m_isOpen;        ///< true если база открыта.
  bool m_offlineLog;    ///< true если необходима поддержка доставки офлайновых сообщений.
  bool m_privateLog;    ///< true если необходимо вести журнал приватных разговоров.
  bool m_publicLog;     ///< true если необходимо вести журнал публичных сообщений в каналах.
  QString m_id;         ///< Идентификатор сооединения с базой, это строка всегда равна "messages".
  Settings *m_settings; ///< Настройки сервера.
};


#endif /* MESSAGELOGPLUGIN_P_H_ */
