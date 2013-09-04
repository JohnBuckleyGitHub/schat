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

#ifndef NODEMASTERCH_H_
#define NODEMASTERCH_H_

#include "ChHook.h"

class FeedEvent;
class Net;

class NodeMasterCh : public ChHook
{
  Q_OBJECT

public:
  NodeMasterCh(QObject *parent = 0);
  void load(ChatChannel channel);

private slots:
  void onNotify(const FeedEvent &event);
  void onSubscriptionChanged(const QByteArray &id, const QString &path);

private:
  Net *m_net;
  QStringList m_paths;
};

#endif /* NODEMASTERCH_H_ */
