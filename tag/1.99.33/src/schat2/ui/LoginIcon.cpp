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

#include "Account.h"
#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "sglobal.h"
#include "ui/LoginIcon.h"

LoginIcon::LoginIcon(QWidget *parent)
  : QLabel(parent)
{
  setPixmap(QPixmap(LS(":/images/key.png")));
  setVisible(false);
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
  connect(ChatClient::io(), SIGNAL(clientStateChanged(int, int)), SLOT(reload()));

  reload();
}


void LoginIcon::notify(const Notify &notify)
{
  if (notify.type() == Notify::FeedData) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.name() != LS("account"))
      return;

    if (n.channel() != ChatClient::id())
      return;

    reload();
  }
}


void LoginIcon::reload()
{
  if (ChatClient::state() != ChatClient::Online) {
    setVisible(false);
    return;
  }

  QString account = ChatClient::channel()->account()->name();
  if (!account.isEmpty()) {
    setVisible(true);
    setToolTip(account);
  }
  else
    setVisible(false);
}
