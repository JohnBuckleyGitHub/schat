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

#include "Channel.h"
#include "plugins/HookData.h"
#include "ui/tabs/ChatViewTab.h"

class ChatView;

class SCHAT_CORE_EXPORT PrivateTab : public ChatViewTab
{
  Q_OBJECT

public:
  PrivateTab(ClientChannel channel, TabWidget *parent);
  ~PrivateTab();
  bool bindMenu(QMenu *menu);
  inline ClientChannel channel() const { return m_channel; }
  void alert(bool start = true);

private:
  QIcon userIcon() const;

  ChatView *m_chatView;    ///< Виджет отображающий текст чата.
  ClientChannel m_channel; ///< Канал.
  int m_alerts;            ///< Количество непрочитанных уведомлений.
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
