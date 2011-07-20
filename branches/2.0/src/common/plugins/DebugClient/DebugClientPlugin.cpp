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

#include <QFile>
#include <QHostAddress>
#include <qplugin.h>
#include <QTextStream>

#include "client/ClientHelper.h"
#include "client/SimpleClient.h"
#include "DebugClientPlugin.h"
#include "DebugClientPlugin_p.h"
#include "FileLocations.h"
#include "FileLocations.h"

DebugClient::DebugClient(ClientHelper *helper, FileLocations *locations)
  : QObject(helper)
  , m_helper(helper)
  , m_locations(locations)
  , m_stream(0)
  , m_client(helper->client())
{
  bool bom = false;
  m_file.setFileName(m_locations->path(FileLocations::VarPath) + QLatin1String("/DebugClient.log"));
  if (!m_file.exists())
    bom = true;

  if (m_file.open(QIODevice::Append)) {
    m_stream = new QTextStream(&m_file);
    m_stream->setGenerateByteOrderMark(bom);
    m_stream->setCodec("UTF-8");
  }

  connect(m_client, SIGNAL(connected()), SLOT(connected()));
  connect(m_client, SIGNAL(disconnected()), SLOT(disconnected()));
  connect(m_client, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
  connect(m_client, SIGNAL(requestAuth(quint64)), SLOT(requestAuth(quint64)));
  connect(m_client, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
}


void DebugClient::connected()
{
  append(QString("connected() peerAddress: %1, peerPort: %2, localAddress: %3, localPort: %4")
      .arg(m_client->peerAddress().toString())
      .arg(m_client->peerPort())
      .arg(m_client->localAddress().toString())
      .arg(m_client->localPort()));
}


void DebugClient::disconnected()
{
  append(QString("disconnected() %1")
      .arg(m_client->errorString()));
}


void DebugClient::error(QAbstractSocket::SocketError socketError)
{
  append(QString("error() %1 %2")
      .arg(socketError)
      .arg(m_client->errorString()));
}


void DebugClient::requestAuth(quint64 id)
{
  Q_UNUSED(id)

  append(QString("requestAuth()"));
}


void DebugClient::stateChanged(QAbstractSocket::SocketState socketState)
{
  append(QString("stateChanged() %1")
      .arg(socketState));
}


void DebugClient::append(const QString &text)
{
  if (!m_stream)
    return;

  *m_stream << QDateTime(QDateTime::currentDateTime()).toString("yyyy.MM.dd hh:mm:ss.zzz ") << text << endl;
}


QObject *DebugClientPlugin::init(ClientHelper *helper, FileLocations *locations)
{
  d = new DebugClient(helper, locations);
  return d;
}

Q_EXPORT_PLUGIN2(DebugClient, DebugClientPlugin);
