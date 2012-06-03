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

#include <QCoreApplication>
#include <QDesktopServices>
#include <QtPlugin>
#include <QWebFrame>

#include "ChatCore.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "DateTime.h"
#include "hooks/ChatViewHooks.h"
#include "messages/Message.h"
#include "net/packets/MessageNotice.h"
#include "net/SimpleID.h"
#include "SendFileMessages.h"
#include "SendFilePlugin.h"
#include "SendFilePlugin_p.h"
#include "sglobal.h"
#include "Tr.h"
#include "Translation.h"
#include "ui/tabs/ChatView.h"
#include "ui/TabWidget.h"

class SendFileTr : public Tr
{
  Q_DECLARE_TR_FUNCTIONS(WebBridgeTr)

public:
  SendFileTr() : Tr() { m_prefix = LS("file-"); }

protected:
  QString valueImpl(const QString &key) const
  {
    if (key == LS("waiting"))        return tr("Waiting");
    else if (key == LS("cancel"))    return tr("Cancel");
    else if (key == LS("cancelled")) return tr("Cancelled");
    return QString();
  }
};


SendFilePluginImpl::SendFilePluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  m_tr = new SendFileTr();
  new SendFileMessages(this);

  ChatCore::translation()->addOther(LS("sendfile"));
  QDesktopServices::setUrlHandler(LS("chat-sendfile"), this, "openUrl");

  connect(ChatViewHooks::i(), SIGNAL(initHook(ChatView*)), SLOT(init(ChatView*)));
  connect(ChatViewHooks::i(), SIGNAL(loadFinishedHook(ChatView*)), SLOT(loadFinished(ChatView*)));
}


SendFilePluginImpl::~SendFilePluginImpl()
{
  delete m_tr;
}


/*!
 * Отправка одиночного файла.
 *
 * \param dest Идентификатор получателя.
 * \param file Файл, который будет отправлен.
 */
bool SendFilePluginImpl::sendFile(const QByteArray &dest, const QString &file)
{
  if (SimpleID::typeOf(dest) != SimpleID::UserId)
    return false;

  SendFileTransaction transaction(new SendFile::Transaction(dest, file));
  if (!transaction->isValid())
    return false;

  MessagePacket packet(new MessageNotice(ChatClient::id(), dest, LS("file"), DateTime::utc(), transaction->id()));
  packet->setCommand(packet->text());
  packet->setData(transaction->toReceiver());

  if (ChatClient::io()->send(packet, true)) {
    Message message(transaction->id(), dest, LS("file"), LS("addFileMessage"));
    message.setAuthor(ChatClient::id());
    message.setDate();
    message.data()[LS("File")]      = transaction->fileName();
    message.data()[LS("Direction")] = LS("outgoing");
    TabWidget::add(message);

    m_transactions[transaction->id()] = transaction;
    return true;
  }

  return false;
}


/*!
 * Чтение входящего пакета.
 */
void SendFilePluginImpl::read(const MessagePacket &packet)
{
  if (packet->text() == LS("file"))
    incomingFile(packet);
  else if (packet->text() == LS("cancel"))
    cancel(packet);
}


void SendFilePluginImpl::init(ChatView *view)
{
  if (SimpleID::typeOf(view->id()) != SimpleID::UserId)
    return;

  view->addJS(LS("qrc:/js/SendFile/SendFile.js"));
  view->page()->mainFrame()->addToJavaScriptWindowObject(LS("SendFile"), this);
}


void SendFilePluginImpl::loadFinished(ChatView *view)
{
  if (SimpleID::typeOf(view->id()) == SimpleID::UserId)
    view->addCSS(LS("qrc:/css/SendFile/SendFile.css"));
}


void SendFilePluginImpl::openUrl(const QUrl &url)
{
  QStringList path = url.path().split(LC('/'));
  if (path.size() < 2)
    return;

  QString action = path.at(0);
  QByteArray id = SimpleID::decode(path.at(1));
  if (SimpleID::typeOf(id) != SimpleID::MessageId)
    return;

  if (action == LS("cancel"))
    cancel(id);
}


/*!
 * Обработка отмены передачи файла вызванной удалённым клиентом.
 */
void SendFilePluginImpl::cancel(const MessagePacket &packet)
{
  SendFileTransaction transaction = m_transactions.value(packet->id());
  if (!transaction)
    return;

  m_transactions.remove(packet->id());
  emit cancelled(SimpleID::encode(packet->id()));
}


/*!
 * Обработка локально вызванной отмены передачи файла.
 */
void SendFilePluginImpl::cancel(const QByteArray &id)
{
  SendFileTransaction transaction = m_transactions.value(id);
  if (!transaction)
    return;

  m_transactions.remove(id);

  MessagePacket packet(new MessageNotice(ChatClient::id(), transaction->user(), LS("cancel"), DateTime::utc(), transaction->id()));
  packet->setCommand(LS("file"));
  ChatClient::io()->send(packet, true);

  emit cancelled(SimpleID::encode(id));
}


/*!
 * Обработка нового входящего файла.
 */
void SendFilePluginImpl::incomingFile(const MessagePacket &packet)
{
  if (m_transactions.contains(packet->id()))
    return;

  SendFileTransaction transaction(new SendFile::Transaction(packet->sender(), packet->id(), packet->json()));
  if (!transaction->isValid())
    return;

  Message message(packet->id(), packet->sender(), LS("file"), LS("addFileMessage"));
  message.setAuthor(packet->sender());
  message.setDate();
  message.data()[LS("File")]      = transaction->fileName();
  message.data()[LS("Direction")] = LS("incoming");
  TabWidget::add(message);

  m_transactions[transaction->id()] = transaction;
}


ChatPlugin *SendFilePlugin::create()
{
  m_plugin = new SendFilePluginImpl(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(SendFile, SendFilePlugin);
