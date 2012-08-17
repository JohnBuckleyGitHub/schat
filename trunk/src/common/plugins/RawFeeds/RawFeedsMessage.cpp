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

#include <QDebug>

#include "ChatCore.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "JSON.h"
#include "net/packets/FeedNotice.h"
#include "net/SimpleID.h"
#include "RawFeedsMessage.h"
#include "sglobal.h"

RawFeedsMessage::RawFeedsMessage(const FeedNotice &packet)
  : Message()
{
  m_tab = packet.sender();

  m_data[LS("Type")]    = LS("raw-feeds");
  m_data[LS("Id")]      = QString(SimpleID::encode(ChatCore::randomId()));
  m_data[LS("Text")]    = QString(packet.raw());
  m_data[LS("Command")] = packet.command();
  m_data[LS("Date")]    = ChatClient::date();
  m_data[LS("Func")]    = LS("addRawFeedsMessage");

  QVariantMap status;
  status[LS("Code")] = packet.status();
  status[LS("Desc")] = FeedNotice::status(packet.status());

  m_data[LS("Status")] = status;

  QString title = packet.command();
  if (title == LS("headers")) {
    title = "Feeds";
    headers(packet.json().value(LS("feeds")).toMap());
  }

  m_data[LS("Title")] = title;
}


RawFeedsMessage::RawFeedsMessage(const QByteArray &tab, const QString &command, const QVariantMap &data)
  : Message()
{
  m_tab = tab;

  m_data[LS("Type")]    = LS("raw-feeds");
  m_data[LS("Id")]      = QString(SimpleID::encode(ChatCore::randomId()));
  m_data[LS("Text")]    = QString(JSON::generate(data));
  m_data[LS("Command")] = command;
  m_data[LS("Date")]    = ChatClient::date();
  m_data[LS("Func")]    = LS("addRawFeedsMessage");

  QVariantMap status;
  status[LS("Code")] = 200;
  status[LS("Desc")] = LS("OK");

  m_data[LS("Status")] = status;
}


/*!
 * Формирование данных для отображения списка фидов.
 */
void RawFeedsMessage::headers(const QVariantMap &data)
{
  ClientChannel channel = ChatClient::channels()->get(m_tab);
  if (!channel)
    return;

  QVariantMap feeds;

  QMapIterator<QString, QVariant> i(data);
  while (i.hasNext()) {
    i.next();
    QVariantMap feed;
    feed[LS("date")]   = i.value().toMap().value(LS("date"));
    feed[LS("cached")] = static_cast<bool>(channel->feed(i.key(), false));
    feeds[i.key()] = feed;
  }

  m_data[LS("Data")] = feeds;
}
