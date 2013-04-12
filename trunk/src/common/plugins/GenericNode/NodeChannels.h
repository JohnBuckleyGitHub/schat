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

#ifndef NODECHANNELS_H_
#define NODECHANNELS_H_

#include "NodeNoticeReader.h"
#include "net/Channels.h"
#include "net/packets/ChannelNotice.h"

class ChannelNotice;
class Core;
class FeedEvent;
class Storage;

class NodeChannels : public QObject, NodeNoticeReader
{
  Q_OBJECT

public:
  NodeChannels(Core *core);

protected:
  bool read(PacketReader *reader);
  void acceptImpl(ChatChannel user, const AuthResult & /*result*/, QList<QByteArray> &packets);
  void addImpl(ChatChannel user);
  void releaseImpl(ChatChannel user, quint64 socket);

private slots:
  void notify(const FeedEvent &event);

private:
  bool info();
  bool join();
  int name();
  bool part();
  bool quit();
  int update();

  bool isForbidden(ChatChannel channel) const;
  ChannelPacket reply(ChatChannel channel, bool forbidden = false, const QString &command = CHANNELS_CHANNEL_CMD) const;
  void reply(int status);

  ChannelNotice *m_packet;
  ChatChannel m_user;
};

#endif /* NODECHANNELS_H_ */
