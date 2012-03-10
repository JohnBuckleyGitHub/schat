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

#ifndef DEBUGCLIENTPLUGIN_P_H_
#define DEBUGCLIENTPLUGIN_P_H_

#include <QAbstractSocket>
#include <QFile>
#include <QObject>

class ClientHelper;
class FileLocations;
class QTextStream;
class SimpleClient;

class DebugClient : public QObject
{
  Q_OBJECT

public:
  DebugClient(ClientHelper *helper, FileLocations *locations);

private slots:
  void connected();
  void disconnected();
  void error(QAbstractSocket::SocketError socketError);
  void requestAuth(quint64 id);
  void stateChanged(QAbstractSocket::SocketState socketState);

private:
  void append(const QString &text);

  ClientHelper *m_helper;
  FileLocations *m_locations;
  QFile m_file;
  QTextStream *m_stream;
  SimpleClient *m_client;
};

#endif /* DEBUGCLIENTPLUGIN_P_H_ */
