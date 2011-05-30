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
#include <QDir>

#include "debugstream.h"
#include "ChatCore.h"
#include "NetworkManager.h"
#include "net/SimpleClient.h"
#include "net/ServerData.h"
#include "ChatSettings.h"
#include "net/SimpleID.h"

NetworkManager::NetworkManager(QObject *parent)
  : QObject(parent)
{
  m_settings = ChatCore::i()->settings();
  m_client = ChatCore::i()->client();

  connect(m_client, SIGNAL(clientStateChanged(int)), SLOT(clientStateChanged(int)));
}


void NetworkManager::clientStateChanged(int state)
{
  if (state != SimpleClient::ClientOnline)
    return;

  ServerData *data = m_client->serverData();

  root(data->id());
}


QString NetworkManager::root(const QByteArray &id)
{
  QString out = m_settings->root() + "/.networks/" + SimpleID::toBase64(id);
  if (!QFile::exists(out))
    QDir().mkpath(out);

  qDebug() << id.toBase64();
  return out;
}
