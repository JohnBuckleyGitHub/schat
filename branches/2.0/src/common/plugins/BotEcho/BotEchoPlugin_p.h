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

#ifndef BOTECHOPLUGIN_P_H_
#define BOTECHOPLUGIN_P_H_

#include <QObject>

class ClientHelper;
class FileLocations;
class MessageData;
class SimpleClient;

class BotEcho : public QObject
{
  Q_OBJECT

public:
  BotEcho(ClientHelper *helper, FileLocations *locations);

private slots:
  void join(const QByteArray &channelId, const QByteArray &userId);
  void join(const QByteArray &channelId, const QList<QByteArray> &usersId);
  void message(const MessageData &data);

private:
  ClientHelper *m_helper;
  FileLocations *m_locations;
  SimpleClient *m_client;
};

#endif /* BOTECHOPLUGIN_P_H_ */
