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

#ifndef CHANNELBASETAB_H_
#define CHANNELBASETAB_H_

#include "Channel.h"
#include "ui/tabs/AbstractTab.h"

class ChannelInfo;
class ChatView;
class Message;

/*!
 * Базовый класс для вкладок каналов.
 */
class SCHAT_CORE_EXPORT ChannelBaseTab : public AbstractTab
{
  Q_OBJECT

public:
  ChannelBaseTab(ClientChannel channel, TabType type, TabWidget *parent);
  ~ChannelBaseTab();

  inline ChatView *chatView()          { return m_chatView; }
  inline ClientChannel channel() const { return m_channel; }
  inline int alerts() const            { return m_alerts; }

  bool bindMenu(QMenu *menu);
  virtual void alert(bool start = true);
  void add(const Message &message);
  void setOnline(bool online = true);

private slots:
  void channel(const ChannelInfo &info);
  void offline();

protected:
  QIcon channelIcon() const;
  virtual void reload();
  void rename(const QByteArray &id);

  ChatView *m_chatView;    ///< Виджет отображающий текст чата.
  ClientChannel m_channel; ///< Канал.
  int m_alerts;            ///< Количество непрочитанных уведомлений.
};

#endif /* CHANNELBASETAB_H_ */
