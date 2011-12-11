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

#include <QPixmap>
#include <QPainter>

#include "net/SimpleID.h"
#include "ui/ChatIcons.h"

/*!
 * Иконка канала.
 */
QIcon ChatIcons::icon(ClientChannel channel, int options)
{
  QString file = ":/webkit/resources/missingImage.png";

  if (channel->type() == SimpleID::UserId) {
    file = ":/images/user";
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
  }
  else if (channel->type() == SimpleID::ChannelId)
    file = ":/images/channel.png";

  if (options & OfflineStatus && channel->status().value() == Status::Offline && !(options & Statuses))
    options |= Statuses;

  if (options & Statuses) {
    if (options & OfflineStatus && channel->status().value() == Status::Offline)
      return QIcon(QIcon(file).pixmap(16, 16, QIcon::Disabled));

    return ChatIcons::icon(file, overlay(channel->status().value()));
  }

  return QIcon(file);
}


/*!
 * Наложение маленькой иконки \p overlay на большую \p icon в правый нижний угол.
 *
 * \param icon    Иконка размером 16x16 пикселей.
 * \param overlay Иконка-оверлей размером 10x10 пикселей.
 */
QIcon ChatIcons::icon(const QIcon &icon, const QString &overlay)
{
  if (overlay.isEmpty())
    return icon;

  QPixmap pixmap = icon.pixmap(16, 16);
  QPainter painter(&pixmap);
  painter.drawPixmap(6, 6, QPixmap(overlay));
  painter.end();

  return QIcon(pixmap);
}


/*!
 * Наложение маленькой иконки \p overlay на большую \p file в правый нижний угол.
 *
 * \param file    Файл иконки размером 16x16 пикселей.
 * \param overlay Иконка-оверлей размером 10x10 пикселей.
 */
QIcon ChatIcons::icon(const QString &file, const QString &overlay)
{
  if (overlay.isEmpty())
    return QIcon(file);

  QPixmap pixmap(file);
  QPainter painter(&pixmap);
  painter.drawPixmap(6, 6, QPixmap(overlay));
  painter.end();

  return QIcon(pixmap);
}


/*!
 * Возвращает имя файла с иконкой оверлеем для статуса \p status пользователя.
 */
QString ChatIcons::overlay(int status)
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
