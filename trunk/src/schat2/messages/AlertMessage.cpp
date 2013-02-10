/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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
#include "messages/AlertMessage.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "ui/tabs/ChannelBaseTab.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/ServerTab.h"
#include "ui/TabWidget.h"

AlertMessage::AlertMessage(const QString &text, const QString &extra)
  : Message(ChatCore::randomId(), ChatCore::currentId(), LS("alert"), LS("addAlertMessage"))
{
  m_data[MESSAGE_TEXT] = text;
  m_data[MESSAGE_EXTRA] = LS("alert-box-") + extra;
}


bool AlertMessage::show(const Message &message)
{
  if (!TabWidget::i() || !message.isValid())
    return false;

  if (SimpleID::typeOf(message.tab()) == SimpleID::ServerId) {
    TabWidget::i()->serverTab()->chatView()->add(message);
    return true;
  }

  ChannelBaseTab *tab = TabWidget::i()->channelTab(message.tab(), false, false);
  if (!tab)
    return false;

  tab->chatView()->add(message);
  return true;
}


bool AlertMessage::show(const QString &text, const QString &extra)
{
  return show(AlertMessage(text, extra));
}
