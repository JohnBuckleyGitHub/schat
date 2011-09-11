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

#ifndef SIMPLECLIENT_H_
#define SIMPLECLIENT_H_

#include "client/AbstractClient.h"

class AbstractNotice;
class SimpleClientPrivate;

class SCHAT_EXPORT SimpleClient : public AbstractClient
{
  Q_OBJECT

public:
  explicit SimpleClient(QObject *parent = 0);
  ~SimpleClient();
  ClientChannel channel(const QByteArray &id) const;
  ClientUser user() const;
  ClientUser user(const QByteArray &id) const;
  void leave();
  void part(const QByteArray &channelId);
  void setCookieAuth(bool allow);

signals:
  void join(const QByteArray &channelId);
  void join(const QByteArray &channelId, const QByteArray &userId);
  void notice(const AbstractNotice &notice);
  void notice(quint16 type);
  void part(const QByteArray &channelId, const QByteArray &userId);
  void split(const QList<QByteArray> &users);
  void synced(const QByteArray &channelId);
  void userDataChanged(const QByteArray &userId);
  void userLeave(const QByteArray &userId);

protected:
  SimpleClient(SimpleClientPrivate &dd, QObject *parent);

protected slots:
  void requestAuth();

protected:
  void newPacketsImpl();

private:
  Q_DECLARE_PRIVATE(SimpleClient);
};

#endif /* SIMPLECLIENT_H_ */
