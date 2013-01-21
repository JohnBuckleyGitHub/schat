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

#ifndef HISTORYCHATVIEW_H_
#define HISTORYCHATVIEW_H_

#include "hooks/ChatViewHooks.h"

class Notify;

class HistoryChatView : public ChatViewHooks
{
  Q_OBJECT

public:
  HistoryChatView(QObject *parent = 0);

signals:
  void loading(const QString &id);

protected:
  void addImpl(ChatView *view);
  void initImpl(ChatView *view);
  void loadFinishedImpl(ChatView *view);

private slots:
  void notify(const Notify &notify);
  void ready();

private:
  bool compatible(const QByteArray &id) const;
  bool sync(const QByteArray &id, qint64 date = 0);
  void emulateLast(const QByteArray &channelId, const QList<QByteArray> &ids);
};

#endif /* HISTORYCHATVIEW_H_ */
