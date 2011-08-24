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

#ifndef CHANNELTAB_H_
#define CHANNELTAB_H_

#include "Channel.h"
#include "ui/tabs/ChatViewTab.h"
#include "User.h"

class ChatView;
class QSplitter;
class UserView;

class ChannelTab : public ChatViewTab
{
  Q_OBJECT

public:
  ChannelTab(ClientChannel channel, TabWidget *parent);
  ~ChannelTab();
  bool add(ClientUser user);
  bool remove(const QByteArray &id);
  inline UserView *userView() { return m_userView; }
  void alert(bool start = true);
  void setOnline(bool online = true);
  void synced();

private slots:
  void settingsChanged(const QString &key, const QVariant &value);
  void userLeave(const QByteArray &userId);

private:
  void displayUserCount();

  bool m_userCount;
  ClientChannel m_channel;
  QSplitter *m_splitter;
  UserView *m_userView;
};

#endif /* CHANNELTAB_H_ */
