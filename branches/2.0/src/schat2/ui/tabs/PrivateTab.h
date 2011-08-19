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

#ifndef PRIVATETAB_H_
#define PRIVATETAB_H_

#include "plugins/HookData.h"
#include "ui/tabs/ChatViewTab.h"
#include "User.h"

class ChatView;

class SCHAT_CORE_EXPORT PrivateTab : public ChatViewTab
{
  Q_OBJECT

public:
  PrivateTab(ClientUser user, TabWidget *parent);
  ~PrivateTab();
  bool update(ClientUser user);
  inline ClientUser user() const { return m_user; }
  MenuBuilder *menu();
  void alert(bool start = true);
  void setOnline(bool online = true);

private:
  QIcon userIcon() const;

  ClientUser m_user;
};


class PrivateTabHook : public HookData
{
public:
  PrivateTabHook(PrivateTab *tab)
  : HookData(PrivateTabCreated)
  , tab(tab)
  {}

  const PrivateTab *tab;
};

#endif /* PRIVATETAB_H_ */
