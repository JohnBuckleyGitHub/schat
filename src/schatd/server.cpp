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


/** [public slots]
 * Вызывается для добавления нового участника (с проверкой на дубликат)
 */
void Server::appendParticipant(const QString &p)
{
  qDebug() << "Server::appendParticipant(const QString &p)";
  
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    if (!peers.contains(p)) {
      peers.insert(p, socket);
      socket->sendGreeting();
    }
    else {
      socket->setNick("#DUBLICATE");
      socket->send(sChatOpcodeError, sChatErrorNickAlreadyUse);
      socket->disconnectFromHost();
    }
  }  
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
  
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender()))
    removeConnection(socket);
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
 * Отправляем имена участников, по одному имени на запрос
 * В конце посылаем команду завершения передачи списка.
 */
void Server::needParticipantList()
{
  qDebug() << "Server::needParticipantList()";
  
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    QString nick = socket->nickname();
    QHashIterator<QString, ServerSocket *> i(peers);
    while (i.hasNext()) {
      i.next();
      socket->send(sChatOpcodeNewParticipantQuiet, i.key());
      if (socket != i.value())
        i.value()->send(sChatOpcodeNewParticipant, nick);
    }
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
void Server::relayMessage(const QString &nick, const QString &message)
{
  qDebug() << "Server::relayMessage(const QString &nick, const QString &message)";
  
  QHashIterator<QString, ServerSocket *> i(peers);
  while (i.hasNext()) {
    i.next();
    i.value()->send(sChatOpcodeSendMessage, nick, message);
  }
}
