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

#ifndef CLIENTFEEDSIMPL_H_
#define CLIENTFEEDSIMPL_H_

#include "client/ClientHooks.h"

class ClientFeedsImpl : public Hooks::Feeds
{
  Q_OBJECT

public:
  ClientFeedsImpl(QObject *parent = 0);

  void readFeed(const FeedNotice &packet);

private:
  QStringList unsynced(ClientChannel channel, const QVariantMap &feeds);
  void feed();
  void get(const QByteArray &id, const QStringList &feeds);
  void headers();

  ClientChannel m_channel;
  const FeedNotice *m_packet;
};

#endif /* CLIENTFEEDSIMPL_H_ */
