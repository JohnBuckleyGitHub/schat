/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#ifndef NODEMESSAGES_H_
#define NODEMESSAGES_H_

#include "NodeNoticeReader.h"

class FeedEvent;
class MessageNotice;

class NodeMessages : public NodeNoticeReader
{
public:
  NodeMessages(Core *core);

protected:
  bool read(PacketReader *reader);

private:
  FeedEvent *createEvent();
  void reject(int status);

  MessageNotice *m_packet; ///< Прочитанный пакет.
  ChatChannel m_dest;      ///< Канал получателя.
  ChatChannel m_sender;    ///< Канал отправителя.
};

#endif /* NODEMESSAGES_H_ */
