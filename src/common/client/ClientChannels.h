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

#ifndef CLIENTCHANNELS_H_
#define CLIENTCHANNELS_H_

#include <QObject>

#include "Channel.h"

class ChannelPacket;
class SimpleClient;

class SCHAT_EXPORT ClientChannels : public QObject
{
  Q_OBJECT

public:
  ClientChannels(QObject *parent = 0);
  ClientChannel get(const QByteArray &id);
  void join(const QString &name);

signals:
  void channel(const QByteArray &id);
  void notice(const ChannelPacket &notice);

private slots:
  void notice(int type);

private:
  ClientChannel add();
  void channel();

  ChannelPacket *m_packet;                     ///< Текущий прочитанный пакет.
  QHash<QByteArray, ClientChannel> m_channels; ///< Таблица каналов.
  SimpleClient *m_client;                      ///< Клиент чата.
};

#endif /* CLIENTCHANNELS_H_ */
