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

#include <QLabel>

#include "Channel.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "sglobal.h"
#include "ui/fields/NickEdit.h"
#include "net/packets/ChannelNotice.h"

NickEdit::NickEdit(QWidget *parent)
  : LineEdit(parent)
  , m_error(0)
{
  setText(ChatClient::io()->nick());

  setMaxLength(Channel::MaxNameLength);

  connect(this, SIGNAL(editingFinished()), SLOT(editingFinished()));
  connect(this, SIGNAL(textChanged(const QString &)), SLOT(textChanged()));
  connect(ChatCore::settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(ChatClient::channels(), SIGNAL(notice(const ChannelNotice &)), SLOT(notice(const ChannelNotice &)));
}


void NickEdit::editingFinished()
{
  if (!Channel::isValidName(text()))
    return;

  if (ChatClient::state() != ChatClient::Online) {
    ChatClient::io()->setNick(text());
    ChatCore::settings()->setValue(LS("Profile/Nick"), ChatClient::channel()->name());
    return;
  }

  ChatClient::channels()->nick(text());
}


void NickEdit::notice(const ChannelNotice &notice)
{
  if (notice.status() == Notice::OK) {
    makeRed(false);
    return;
  }

  if (notice.command() != LS("update"))
    return;

  if (notice.sender() != ChatClient::id())
    return;

  makeRed();

  if (!m_error && notice.status() == Notice::ObjectAlreadyExists) {
    m_error = new QLabel(this);
    m_error->setPixmap(QPixmap(LS(":/images/exclamation-red-frame.png")));
    setToolTip(tr("Nickname is already in use"));
    addWidget(m_error, RightSide);
  }
}


void NickEdit::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == LS("Profile/Nick"))
    setText(value.toString());
}


void NickEdit::textChanged()
{
  makeRed(!Channel::isValidName(text()));
}


void NickEdit::makeRed(bool red)
{
  QPalette palette = this->palette();

  if (!red) {
    palette.setColor(QPalette::Active, QPalette::Base, Qt::white);
    if (m_error) {
      removeWidget(m_error);
      delete m_error;
      m_error = 0;
    }
  }
  else
    palette.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));

  setPalette(palette);
}
