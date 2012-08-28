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

class HistoryImpl : public ChatPlugin
{
  Q_OBJECT

public:
  HistoryImpl(QObject *parent);
  static bool get(const QList<QByteArray> &ids);
  static bool getLast(const QByteArray &id);
  static bool getOffline();
  static QList<QByteArray> getLocal(const QList<QByteArray> &ids);

private slots:
  void getLast();
  void notify(const Notify &notify);
  void open();

private:
  HistoryChatView *m_chatView;
};


#endif /* HISTORYPLUGIN_P_H_ */
