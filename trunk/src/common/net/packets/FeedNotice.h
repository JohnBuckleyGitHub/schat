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

#ifndef FEEDNOTICE_H_
#define FEEDNOTICE_H_

#include "net/packets/Notice.h"

class FeedQueryReply;

class SCHAT_EXPORT FeedNotice : public Notice
{
public:
  FeedNotice();
  FeedNotice(const QByteArray &sender, const QByteArray &dest, const QString &command, const QByteArray &id = QByteArray());
  FeedNotice(quint16 type, PacketReader *reader);

  static QByteArray headers(const QByteArray &user, const QByteArray &channel, QDataStream *stream);
  static QByteArray reply(const FeedNotice &source, const FeedQueryReply &reply, QDataStream *stream);
  static QByteArray reply(const FeedNotice &source, const QVariantMap &json, QDataStream *stream);
  static QByteArray reply(const FeedNotice &source, int status, QDataStream *stream);
  static QByteArray request(const QByteArray &user, const QByteArray &channel, const QString &command, const QString &name, QDataStream *stream, const QVariantMap &json = QVariantMap());
};

#endif /* FEEDNOTICE_H_ */
