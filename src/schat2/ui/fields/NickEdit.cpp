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

#include "Channel.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "ui/fields/NickEdit.h"

NickEdit::NickEdit(QWidget *parent)
  : LineEdit(parent)
{
  setText(ChatClient::io()->nick());

  setMaxLength(Channel::MaxNameLength);

  connect(this, SIGNAL(editingFinished()), SLOT(editingFinished()));
  connect(this, SIGNAL(textChanged(const QString &)), SLOT(textChanged()));
  connect(ChatCore::settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
}


void NickEdit::editingFinished()
{
  if (!Channel::isValidName(text()))
    return;

  if (ChatClient::state() != ChatClient::Online) {
    ChatClient::io()->setNick(text());
    ChatCore::settings()->setValue("Profile/Nick", ChatClient::channel()->name());
    return;
  }

  ChatClient::channels()->update();
}


void NickEdit::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == "Profile/Nick")
    setText(value.toString());
}


void NickEdit::textChanged()
{
  QPalette palette = this->palette();

  if (Channel::isValidName(text()))
    palette.setColor(QPalette::Active, QPalette::Base, Qt::white);
  else
    palette.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));

  setPalette(palette);
}
