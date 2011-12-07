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

#include "ChatCore.h"
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "net/SimpleID.h"
#include "ui/ChannelUtils.h"


QString ChannelUtils::webIcon(const QString &action)
{
  QString icon;
  if (action == "edit/topic")
    icon = "topic-edit";

  if (icon.isEmpty())
    return icon;

  return "qrc:/images/" + icon + ".png";
}


QVariantMap ChannelUtils::toWebButton(const QByteArray &id, const QString &action, const QString &title)
{
  QVariantMap map;
  ClientChannel channel = ChatClient::channels()->get(id);
  if (!channel)
    return map;

  map["url"] = ChatUrls::toUrl(channel, action);
  map["icon"] = webIcon(action);
  map["title"] = title;
  return map;
}


/*!
 * Иконка пользователя.
 *
 * \param user    Пользователь.
 * \param status  true если необходимо отрисовывать статус.
 * \param offline true если необходимо отрисовывать статус User::OfflineStatus.
 */
QIcon ChannelUtils::icon(ClientChannel channel, int options)
{
  QString file = ":/images/user";
  int gender = channel->gender().value();
  int color  = channel->gender().color();

  if (gender == Gender::Unknown) {
    file += "-unknown";
  }
  else if (gender == Gender::Ghost) {
    file += "-ghost";
  }
  else if (gender == Gender::Bot) {
    file += "-bot";
  }
  else if (color != Gender::Default) {
    file += "-" + Gender::colorToString(color);
  }

  if (gender == Gender::Female)
    file += "-female";

  file += ".png";

  if (options & OfflineStatus && channel->status().value() == Status::Offline && !(options & Statuses))
    options |= Statuses;

  if (options & Statuses) {
    if (options & OfflineStatus && channel->status().value() == Status::Offline)
      return QIcon(QIcon(file).pixmap(16, 16, QIcon::Disabled));

    return ChatCore::icon(file, overlay(channel->status().value()));
  }

  return QIcon(file);
}


/*!
 * Возвращает имя файла с иконкой оверлеем для статуса \p status пользователя.
 */
QString ChannelUtils::overlay(int status)
{
  if (status == Status::Away || status == Status::AutoAway)
    return ":/images/away-small.png";
  else if (status == Status::DnD)
    return ":/images/dnd-small.png";
  else if (status == Status::FreeForChat)
    return ":/images/ffc-small.png";
  else
    return QString();
}
