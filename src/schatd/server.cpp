/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>
#include <QtNetwork>

#include "serversocket.h"
#include "server.h"
#include "protocol.h"


/** [public]
 * 
 */
Server::Server(QObject *parent)
  : QTcpServer(parent)
{
}


/** [public]
 * 
 */
bool Server::start()
{
  readSettings();
  
  #ifdef SCHAT_DEBUG
  qDebug() << "Server::start()" << listenAddress << listenPort;
  #endif
  
  return listen(QHostAddress(listenAddress), listenPort);
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
      socket->send(sChatOpcodeMaxDoublePingTimeout, ((PingMinInterval + PingMutator) / 1000) * 2);
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


/** [private]
 * 
 */
void Server::relayMessage(const QString &channel, const QString &nick, const QString &message)
{
  #ifdef SCHAT_DEBUG 
  qDebug() << "Server::relayMessage(const QString &channel, const QString &nick, const QString &message)" << channel << nick << message;
  #endif
  
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


/** [public slots]
 * Отправляем имена участников, по одному имени на запрос
 * В конце посылаем команду завершения передачи списка.
 */
void Server::relayParticipantList(ServerSocket *socket)
{
  quint16 sex = socket->sex();
  QStringList info = socket->participantInfo();
  
  QHashIterator<QString, ServerSocket *> i(peers);
  while (i.hasNext()) {
    i.next();
    // Отсылаем новому участнику список участников
    socket->send(sChatOpcodeNewParticipantQuiet, i.value()->sex(), i.value()->participantInfo());
    
    // Отсылаем существующим участникам, профиль нового
    if (socket != i.value())
      i.value()->send(sChatOpcodeNewParticipant, sex, info);
  }
}


/** [public slots]
 * 
 */
void Server::appendDirectParticipant(const QString &p)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "Server::appendDirectParticipant(const QString &p)";
  #endif
  
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    if (!directPeers.contains(p)) {
      directPeers.insert(p, socket);
      socket->send(sChatOpcodeGreetingOk);
      socket->send(sChatOpcodeMaxDoublePingTimeout, ((PingMinInterval + PingMutator) / 1000) * 2);
      socket->sendLocalProfile();
      socket->setState(sChatStateReadyForUse);
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
void Server::connectionError(QAbstractSocket::SocketError /* socketError */)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "Server::connectionError(QAbstractSocket::SocketError /* socketError */)";
  #endif
  
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    #ifdef SCHAT_DEBUG
    qDebug() << "ERROR:" << socket->errorString();
    #endif
    if (socket->isDirect()) // FIXME добавить #define ...
      removeDirectConnection(socket);
    else
      removeConnection(socket);
  }
}


/** [private slots]
 * 
 */
void Server::disconnected()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "Server::disconnected()";
  #endif
  
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender()))
    if (socket->isDirect()) // FIXME добавить #define ...
      removeDirectConnection(socket);
    else
      removeConnection(socket);
}


/** [private]
 * 
 */
void Server::incomingConnection(int socketId)
{
  ServerSocket *socket = new ServerSocket(this);
  socket->setSocketDescriptor(socketId);
  
  // FIXME добавить #define ...
  socket->setLocalProfile(localProfile);
}


/** [private]
 * 
 */
void Server::participantLeft(const QString &nick)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "Server::participantLeft(const QString &nick)";
  #endif
  
  QHashIterator<QString, ServerSocket *> i(peers);
  while (i.hasNext()) {
    i.next();
    i.value()->send(sChatOpcodeParticipantLeft, nick);
  }
}


/** [private]
 * 
 */
void Server::readSettings()
{
  QSettings settings(QCoreApplication::instance()->applicationDirPath() + "/schatd.conf", QSettings::IniFormat, this);
  
  listenAddress = settings.value("ListenAddress", "0.0.0.0").toString();
  listenPort    = quint16(settings.value("ListenPort", 7666).toUInt());
}


/** [private]
 * 
 */
void Server::removeConnection(ServerSocket *socket)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "Server::removeConnection(ServerSocket *socket)";
  #endif
  
  QString nick = socket->nick();
  
  if (peers.contains(nick)) {
    peers.remove(nick);
    participantLeft(nick);
  }
  socket->deleteLater();
}


/** [private]
 * 
 */
void Server::removeDirectConnection(ServerSocket *socket)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "Server::removeDirectConnection(ServerSocket *socket)";
  #endif
  
  QString nick = socket->nick();
  
  if (directPeers.contains(nick)) {
    directPeers.remove(nick);
  }
  socket->deleteLater();
}
