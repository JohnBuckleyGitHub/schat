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

#ifndef HISTORYPLUGIN_P_H_
#define HISTORYPLUGIN_P_H_

#include "plugins/ChatPlugin.h"
#include "text/MessageId.h"

class HistoryChatView;
class Notify;
class FeedNotify;

class History : public ChatPlugin
{
  Q_OBJECT

public:
  History(QObject *parent);
  static bool get(const QList<MessageId> &ids);
  static bool getLast(const QByteArray &id);
  static bool getOffline();
  static QList<MessageId> getLocal(const QList<MessageId> &ids);

private slots:
  void getLast();
  void notify(const Notify &notify);
  void open();

private:
  void lastReady(const FeedNotify &notify);

  HistoryChatView *m_chatView;
};


#endif /* HISTORYPLUGIN_P_H_ */
