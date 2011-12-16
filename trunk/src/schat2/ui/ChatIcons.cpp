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


QMap<int, QString> ChatIcons::m_icons;

/*!
 * Иконка канала.
 */
QIcon ChatIcons::icon(ClientChannel channel, int options)
{
  QString file = ":/images/channel.png";

  if (channel->type() != SimpleID::ChannelId) {
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

  if (options & OfflineStatus && channel->status().value() == Status::Offline && !(options & Statuses))
    options |= Statuses;

  if (options & Statuses) {
    if (options & OfflineStatus && (channel->status().value() == Status::Offline || !channel->isSynced()))
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


QIcon ChatIcons::icon(int name)
{
  if (!m_icons.contains(name))
    return QIcon(":/webkit/resources/missingImage.png");

  return QIcon(":/images/" + m_icons.value(name) + ".png");
}


void ChatIcons::init()
{
  m_icons[Channel]        = "channel";
  m_icons[ChannelAlert]   = "channel-alert";
  m_icons[Gear]           = "gear";
  m_icons[MainTabMenu]    = "main-tab-menu";
  m_icons[NetworkError]   = "network-error";
  m_icons[NetworkOffline] = "offline";
  m_icons[NetworkOnline]  = "online";
  m_icons[Connect]        = "plug";
  m_icons[Disconnect]     = "plug-disconnect";
  m_icons[Quit]           = "quit";
  m_icons[Secure]         = "secure";
  m_icons[Settings]       = "settings";
  m_icons[Sound]          = "sound";
  m_icons[SoundMute]      = "sound_mute";
  m_icons[Users]          = "users";
  m_icons[SmallLogo]      = "schat16";
  m_icons[SmallLogoNY]    = "schat16-ny";
  m_icons[TextBold]       = "text-bold";
  m_icons[TextItalic]     = "text-italic";
  m_icons[TextStrike]     = "text-strikethrough";
  m_icons[TextUnderline]  = "text-underline";
  m_icons[Send]           = "send";
  m_icons[Globe]          = "globe";
  m_icons[Add]            = "add";
  m_icons[Remove]         = "remove";
  m_icons[Profile]        = "profile";
  m_icons[OK]             = "ok";
  m_icons[InfoBalloon]    = "information-balloon";
  m_icons[EditClear]      = "edit-clear";
  m_icons[EditCopy]       = "edit-copy";
  m_icons[EditCut]        = "edit-cut";
  m_icons[EditPaste]      = "edit-paste";
  m_icons[EditSelectAll]  = "edit-select-all";
  m_icons[Balloon]        = "balloon";
  m_icons[Slash]          = "slash";
  m_icons[TopicEdit]      = "topic-edit";
  m_icons[Key]            = "key";
  m_icons[ExclamationRed] = "exclamation-red";
  m_icons[ArrowRight]     = "arrow-right";
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
