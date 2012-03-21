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

#ifndef NODEFEEDS_H_
#define NODEFEEDS_H_

#include "NodeNoticeReader.h"

class FeedNotice;
class ServerChannel;

class NodeFeeds : public NodeNoticeReader
{
public:
  NodeFeeds(Core *core);

protected:
  bool read(PacketReader *reader);

private:
  int add();
  int clear();
  int get();
  int headers();
  int query();
  int remove();
  int revert();
  int update();

  int check(int acl);
  void broadcast(FeedPtr feed, bool echo = true);
  void reply(int status);

  FeedNotice *m_packet;
  ChatChannel m_channel;
  ChatChannel m_user;
};

#endif /* NODEFEEDS_H_ */
