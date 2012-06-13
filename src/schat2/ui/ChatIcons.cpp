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

#include <QPixmap>
#include <QPainter>

#include "net/SimpleID.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"

QMap<int, QString> ChatIcons::m_icons;

/*!
 * Иконка канала.
 */
QIcon ChatIcons::icon(ClientChannel channel, int options)
{
  QString file = LS(":/images/channel.png");

  if (channel->type() != SimpleID::ChannelId) {
    file = LS(":/images/user");
    int gender = channel->gender().value();
    int color  = channel->gender().color();

    if (gender == Gender::Unknown) {
      file += LS("-unknown");
    }
    else if (gender == Gender::Ghost) {
      file += LS("-ghost");
    }
    else if (gender == Gender::Bot) {
      file += LS("-bot");
    }
    else if (color != Gender::Default) {
      file += LS("-") + Gender::colorToString(color);
    }

    if (gender == Gender::Female)
      file += LS("-female");

    file += LS(".png");
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
    return QIcon(LS(":/webkit/resources/missingImage.png"));

  return QIcon(LS(":/images/") + m_icons.value(name) + LS(".png"));
}


/*!
 * Возвращает имя файла с иконкой оверлеем для статуса \p status пользователя.
 */
QString ChatIcons::overlay(int status)
{
  if (status == Status::Away || status == Status::AutoAway)
    return LS(":/images/away-small.png");
  else if (status == Status::DnD)
    return LS(":/images/dnd-small.png");
  else if (status == Status::FreeForChat)
    return LS(":/images/ffc-small.png");
  else
    return QString();
}


void ChatIcons::init()
{
  m_icons[Channel]        = LS("channel");
  m_icons[ChannelAlert]   = LS("channel-alert");
  m_icons[Gear]           = LS("gear");
  m_icons[MainTabMenu]    = LS("main-tab-menu");
  m_icons[NetworkError]   = LS("network-error");
  m_icons[NetworkOffline] = LS("offline");
  m_icons[NetworkOnline]  = LS("online");
  m_icons[Connect]        = LS("plug");
  m_icons[Disconnect]     = LS("plug-disconnect");
  m_icons[Quit]           = LS("quit");
  m_icons[Secure]         = LS("secure");
  m_icons[Settings]       = LS("settings");
  m_icons[Sound]          = LS("sound");
  m_icons[SoundMute]      = LS("sound_mute");
  m_icons[Users]          = LS("users");
  m_icons[SmallLogo]      = LS("schat16");
  m_icons[SmallLogoNY]    = LS("schat16-ny");
  m_icons[TextBold]       = LS("text-bold");
  m_icons[TextItalic]     = LS("text-italic");
  m_icons[TextStrike]     = LS("text-strikethrough");
  m_icons[TextUnderline]  = LS("text-underline");
  m_icons[Send]           = LS("send");
  m_icons[Globe]          = LS("globe");
  m_icons[Add]            = LS("add");
  m_icons[Remove]         = LS("remove");
  m_icons[Profile]        = LS("profile");
  m_icons[OK]             = LS("ok");
  m_icons[InfoBalloon]    = LS("information-balloon");
  m_icons[Information]    = LS("information");
  m_icons[EditClear]      = LS("edit-clear");
  m_icons[EditCopy]       = LS("edit-copy");
  m_icons[EditCut]        = LS("edit-cut");
  m_icons[EditPaste]      = LS("edit-paste");
  m_icons[EditSelectAll]  = LS("edit-select-all");
  m_icons[Balloon]        = LS("balloon");
  m_icons[Slash]          = LS("slash");
  m_icons[TopicEdit]      = LS("topic-edit");
  m_icons[Key]            = LS("key");
  m_icons[Exclamation]    = LS("exclamation");
  m_icons[ExclamationRed] = LS("exclamation-red");
  m_icons[ArrowRight]     = LS("arrow-right");
  m_icons[SignIn]         = LS("sign-in");
  m_icons[SignOut]        = LS("sign-out");
  m_icons[SignUp]         = LS("sign-up");
  m_icons[Password]       = LS("password");
  m_icons[Locale]         = LS("locale");
  m_icons[Computer]       = LS("computer");
  m_icons[Reload]         = LS("reload");
}
