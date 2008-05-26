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
      socket->send(sChatOpcodeMaxDoublePingTimeout, PingInterval / 1000 * 2);
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
 * Слот вызывается при получении пакета с опкодом `sChatOpcodeNewProfile`.
 * Варианты действий:
 *  1. ник, не изменился, рассылаем уведомление `sChatOpcodeChangedProfile`.
 *  2. новый ник уже есть в списке участников, разрываем соединение.
 *  3. ник изменился, меняем ник в списке участников и рассылаем `sChatOpcodeChangedNick`.
 * ПАРАМЕТРЫ:
 *  quint16 sex         -> новый пол
 *  const QString &nick -> новый ник
 *  const QString &name -> новое полное имя * 
 */
void Server::clientSendNewProfile(quint16 sex, const QString &nick, const QString &name)
{
  #ifdef SCHAT_DEBUG 
  qDebug() << "void Server::clientSendNewProfile(quint16 sex, const QString &nick, const QString &name)" << sex << nick << name;
  #endif

  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    if (socket->nick() == nick) {
      socket->setSex(sex);
      socket->setFullName(name);
      QStringList list;
      list << nick << name;
      
      QHashIterator<QString, ServerSocket *> i(peers);
      while (i.hasNext()) {
        i.next();
        i.value()->send(sChatOpcodeChangedProfile, sex, list);
      }
    }
    else if (peers.contains(nick)) {
      socket->abort();
      return;
    }
    else {
      peers.remove(socket->nick());
      peers.insert(nick, socket);
      QStringList list;
      list << socket->nick() << nick << name;
      socket->setNick(nick);
      socket->setSex(sex);
      socket->setFullName(name);
      
      QHashIterator<QString, ServerSocket *> i(peers);
      while (i.hasNext()) {
        i.next();
        i.value()->send(sChatOpcodeChangedNick, sex, list);
      }
    }
  }
}


/** [public slots]
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
#ifdef SCHAT_CLIENT
void Server::appendDirectParticipant(const QString &p)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "Server::appendDirectParticipant(const QString &p)";
  #endif
  
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    if (!directPeers.contains(p) && localProfile->nick() != p) {
      directPeers.insert(p, socket);
      socket->send(sChatOpcodeGreetingOk);
      socket->send(sChatOpcodeMaxDoublePingTimeout, PingInterval / 1000 * 2);
      socket->sendLocalProfile();
      socket->setState(sChatStateReadyForUse);
      emit incomingDirectConnection(p, socket);
    }
    else {
      socket->setNick("#DUBLICATE");
      socket->send(sChatOpcodeError, sChatErrorNickAlreadyUse);
      socket->disconnectFromHost();
    }
  }  
}
#endif


/** [private slots]
 * 
 */
void Server::connectionError(QAbstractSocket::SocketError /* socketError */)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "Server::connectionError(QAbstractSocket::SocketError /* socketError */)";
  #endif
  
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    #ifdef SCHAT_CLIENT
    if (socket->isDirect())
      removeDirectConnection(socket);
    else
    #endif
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
    #ifdef SCHAT_CLIENT
    if (socket->isDirect())
      removeDirectConnection(socket);
    else
    #endif
      removeConnection(socket);
}


/** [private]
 * 
 */
void Server::incomingConnection(int socketId)
{
  ServerSocket *socket = new ServerSocket(this);
  socket->setSocketDescriptor(socketId);
  
  #ifdef SCHAT_CLIENT
  socket->setLocalProfile(localProfile);
  #endif
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
#ifdef SCHAT_CLIENT
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
#endif
