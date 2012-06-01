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

#include <QDebug>
#include <QtPlugin>

#include "SendFileCmd.h"
#include "SendFilePlugin.h"
#include "SendFilePlugin_p.h"
#include "ChatCore.h"
#include "net/SimpleID.h"
#include "DateTime.h"
#include "sglobal.h"
#include "client/ChatClient.h"
#include "net/packets/MessageNotice.h"
#include "client/SimpleClient.h"
#include "SendFileTransaction.h"

SendFilePluginImpl::SendFilePluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  new SendFileCmd(this);
}


/*!
 * Базовая функция отправки.
 */
bool SendFilePluginImpl::send(const QByteArray &dest, const QVariantMap &data, const QByteArray &id)
{
  if (SimpleID::typeOf(dest) != SimpleID::UserId)
    return false;

  MessagePacket packet(new MessageNotice(ChatClient::id(), dest, QString(), DateTime::utc(), id));
  packet->setCommand(LS("file"));
  packet->setData(data);

  return ChatClient::io()->send(packet, true);
}


/*!
 * Отправка одиночного файла.
 *
 * \param dest Идентификатор получателя.
 * \param file Файл, который будет отправлен.
 */
bool SendFilePluginImpl::sendFile(const QByteArray &dest, const QString &file)
{
  SendFile::Transaction transaction(dest, file);
  if (!transaction.isValid())
    return false;

  return send(dest, transaction.toReceiver(), transaction.id());
}


ChatPlugin *SendFilePlugin::create()
{
  m_plugin = new SendFilePluginImpl(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(SendFile, SendFilePlugin);
