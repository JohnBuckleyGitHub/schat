/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>
#include <QtNetwork>

#include "serversocket.h"
#include "server.h"
#include "protocol.h"

Server::Server(QObject *parent)
  : QTcpServer(parent)
{
}

void Server::incomingConnection(int socketId)
{
  ServerSocket *socket = new ServerSocket(this);
  socket->setSocketDescriptor(socketId);
}


/** [private slots]
 * 
 */
void Server::disconnected()
{
  qDebug() << "Server::disconnected()";
  
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender()))
    removeConnection(socket);
}


/** [private slots]
 * 
 */
void Server::connectionError(QAbstractSocket::SocketError /* socketError */)
{
  qDebug() << "Server::connectionError(QAbstractSocket::SocketError /* socketError */)";
  
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    qDebug() << "ERROR:" << socket->errorString();
    removeConnection(socket);
  }
}


/** [private]
 * 
 */
void Server::removeConnection(ServerSocket *socket)
{
  qDebug() << "Server::removeConnection(ServerSocket *socket)";
  
  QString nick = socket->nickname();
  
  if (peers.contains(nick)) {
    peers.remove(nick);
    participantLeft(nick);
  }
  socket->deleteLater();
}


/** [public slots]
 * Слот вызывается сигналом, отправленным из `ServerSocket::readGreeting()`
 * В случае если участник с таким именем не подключен к серверу,
 * то всё в порядке отправляем подтверждение приветствия `sChatOpcodeGreetingOk`
 * и устанавливаем состояние сокета `sChatStateWaitingForGetPList`
 * для ожидания пакета `sChatOpcodeNeedParticipantList`
 * 
 * В случае ошибки меняем, ник на специальное значение "#DUBLICATE"
 * Отсылаем пакет `sChatOpcodeError` с кодом ошибки `sChatErrorNickAlreadyUse`
 * и разрываем соединение `disconnectFromHost()`
 */
void Server::appendParticipant(const QString &p)
{
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    if (!peers.contains(p)) {
      peers.insert(p, socket);
      socket->send(sChatOpcodeGreetingOk);
      socket->setState(sChatStateReadyForUse);
      relayParticipantList(socket);
    }
    else {
      socket->setNick("#DUBLICATE");
      socket->send(sChatOpcodeError, sChatErrorNickAlreadyUse);
      socket->disconnectFromHost();
    }
  }  
}


/** [public slots]
 * Отправляем имена участников, по одному имени на запрос
 * В конце посылаем команду завершения передачи списка.
 */
void Server::relayParticipantList(ServerSocket *socket)
{
  quint16 sex = socket->participantSex();
  QStringList info = socket->participantInfo();
  
  QHashIterator<QString, ServerSocket *> i(peers);
  while (i.hasNext()) {
    i.next();
    // Отсылаем новому участнику список участников
    socket->send(sChatOpcodeNewParticipantQuiet, i.value()->participantSex(), i.value()->participantInfo());
    
    // Отсылаем существующим участникам, профиль нового
    if (socket != i.value())
      i.value()->send(sChatOpcodeNewParticipant, sex, info);
  }
}


/** [private]
 * 
 */
void Server::participantLeft(const QString &nick)
{
  qDebug() << "Server::participantLeft(const QString &nick)";
  
  QHashIterator<QString, ServerSocket *> i(peers);
  while (i.hasNext()) {
    i.next();
    i.value()->send(sChatOpcodeParticipantLeft, nick);
  }
}


/** [private]
 * 
 */
void Server::relayMessage(const QString &channel, const QString &nick, const QString &message)
{
  qDebug() << "Server::relayMessage(const QString &channel, const QString &nick, const QString &message)" << channel << nick << message;
  
  if (channel == "#main") {
    QHashIterator<QString, ServerSocket *> i(peers);
    while (i.hasNext()) {
      i.next();
      i.value()->send(sChatOpcodeSendMessage, nick, message);
    }
  } else
    if (peers.contains(channel)) {
      if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender()))
        socket->send(sChatOpcodeSendPrvMessageEcho, channel, message);
      peers[channel]->send(sChatOpcodeSendPrivateMessage, nick, message);
    }
    else
      if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender()))
        socket->send(sChatOpcodeError, sChatErrorNoSuchChannel);  
}
