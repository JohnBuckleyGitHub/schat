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

#ifndef FEEDPACKET_H_
#define FEEDPACKET_H_

#include "Channel.h"
#include "net/packets/Notice.h"

class SCHAT_EXPORT FeedPacket : public Notice
{
public:
  FeedPacket();
  FeedPacket(const QByteArray &sender, const QByteArray &dest, const QString &command, const QByteArray &id = QByteArray());
  FeedPacket(quint16 type, PacketReader *reader);

  static QByteArray clear(const QByteArray &user, const QByteArray &channel, const QString &name, QDataStream *stream);
  static QByteArray cleared(const QByteArray &channel, const QByteArray &user, const QString &name, int status, QDataStream *stream);
  static QByteArray feed(ClientChannel channel, ClientChannel user, const QString &name, QDataStream *stream);
  static QByteArray get(const QByteArray &user, const QByteArray &channel, const QString &name, QDataStream *stream);
  static QByteArray headers(ClientChannel channel, ClientChannel user, QDataStream *stream);
  static QByteArray headers(const QByteArray &user, const QByteArray &channel, QDataStream *stream);
  static QByteArray query(const QByteArray &user, const QByteArray &channel, const QString &name, const QVariantMap &json, QDataStream *stream);
  static QByteArray update(const QByteArray &user, const QByteArray &channel, const QString &name, const QVariantMap &json, QDataStream *stream);
  static QByteArray updated(const QByteArray &channel, const QByteArray &user, const QString &name, int status, QDataStream *stream);
};

#endif /* FEEDPACKET_H_ */
