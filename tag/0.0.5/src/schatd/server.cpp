/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>
#include <QtNetwork>

#include "daemonsettings.h"
#include "log.h"
#include "protocol.h"
#include "server.h"
#include "serversocket.h"


/** [public]
 * 
 */
Server::Server(QObject *parent)
  : QTcpServer(parent)
{
  m_settings = new DaemonSettings(this);
}


/** [public]
 * 
 */
bool Server::start()
{
  m_settings->read();
  
  if (m_settings->channelLog) {
    m_channelLog = new ChannelLog(this);
    m_channelLog->setChannel("#main");
    m_channelLog->setMode(ChannelLog::Plain);
  }
  
  #ifdef SCHAT_PRIVATE_LOG
  if (m_settings->privateLog) {
    m_privateLog = new ChannelLog(this);
    m_privateLog->setChannel("#private");
    m_privateLog->setMode(ChannelLog::Plain);
  }
  #endif
  
  QString address = m_settings->listenAddress;
  quint16 port    = m_settings->listenPort;  
  bool result     = listen(QHostAddress(address), port);
  
  if (result) {
    LOG(0, tr("Simple Chat Daemon успешно запущен, адрес %1, порт %2").arg(address).arg(port));
  }
  else {
    LOG(0, tr("Ошибка запуска Simple Chat Daemon, %1").arg(errorString()));
  }
  
  return result;
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
      
      QStringList info = socket->participantInfo();
      QString fullName;
      if (info.at(1).isEmpty())
        fullName = "empty";
      else
        fullName = info.at(1);
      
      LOG(0, tr("Новый участник: %1, %2, %3, %4").arg(info.at(3)).arg(info.at(0)).arg(fullName).arg(info.at(2)));
      
      if (m_settings->channelLog)
        if (socket->sex())
          m_channelLog->msg(tr("`%1` зашла в чат").arg(info.at(0)));
        else
          m_channelLog->msg(tr("`%1` зашёл в чат").arg(info.at(0)));
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
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    if (socket->nick() == nick) {
      socket->setSex(sex);
      socket->setFullName(name);
      QStringList list;
      list << nick << name;
      
      if (m_settings->channelLog)
        if (sex)
          m_channelLog->msg(tr("`%1` изменила свой профиль").arg(nick));
        else
          m_channelLog->msg(tr("`%1` изменил свой профиль").arg(nick));
      
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
      
      if (m_settings->channelLog)
        if (sex)
          m_channelLog->msg(tr("`%1` теперь известна как `%2`").arg(socket->nick()).arg(nick));
        else
          m_channelLog->msg(tr("`%1` теперь известен как `%2`").arg(socket->nick()).arg(nick));
      
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
 * channel -> получатель сообщения
 * nick    -> отправитель сообщения
 * message -> сообщение
 */
void Server::relayMessage(const QString &channel, const QString &nick, const QString &message)
{
  if (channel == "#main") {
    
    if (m_settings->channelLog)
      m_channelLog->msg(tr("%1: %2").arg(nick).arg(message));
    
    QHashIterator<QString, ServerSocket *> i(peers);
    while (i.hasNext()) {
      i.next();
      i.value()->send(sChatOpcodeSendMessage, nick, message);
    }
  } else {
    ServerSocket *socket = qobject_cast<ServerSocket *>(sender());
    
    if (peers.contains(channel)) {
      
      #ifdef SCHAT_PRIVATE_LOG
      if (m_settings->privateLog)
        m_privateLog->msg(tr("`%1` -> `%2`: %3").arg(nick).arg(channel).arg(message));
      #endif
      
      if (socket)
        socket->send(sChatOpcodeSendPrvMessageEcho, channel, message);
      peers[channel]->send(sChatOpcodeSendPrivateMessage, nick, message);
    }
    else if (socket)
      socket->send(sChatOpcodeError, sChatErrorNoSuchChannel);
  }
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
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    LOG(0, tr("Участник %1 вышел в результате ошибки соединения: %2").arg(socket->nick()).arg(socket->errorString()));
    
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
  if (ServerSocket *socket = qobject_cast<ServerSocket *>(sender())) {
    LOG(0, tr("Соединение с участником %1 (%2) разорвано").arg(socket->nick()).arg(socket->peerAddress().toString()));
    
    #ifdef SCHAT_CLIENT
    if (socket->isDirect())
      removeDirectConnection(socket);
    else
    #endif
      removeConnection(socket);
  }
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
void Server::participantLeft(quint16 sex, const QString &nick, const QString &bye)
{
  if (m_settings->channelLog)
    if (sex)
      m_channelLog->msg(tr("`%1` вышла из чата: %2").arg(nick).arg(bye));
    else
      m_channelLog->msg(tr("`%1` вышел из чата: %2").arg(nick).arg(bye));
  
  QHashIterator<QString, ServerSocket *> i(peers);
  while (i.hasNext()) {
    i.next();
    i.value()->send(sChatOpcodeParticipantLeft, nick, bye);
  }
}


/** [private]
 * 
 */
void Server::removeConnection(ServerSocket *socket)
{
  QString nick = socket->nick();
  QString bye  = socket->byeMsg();
  
  if (peers.contains(nick)) {
    peers.remove(nick);
    participantLeft(socket->sex(), nick, bye);
  }
  socket->deleteLater();
}


/** [private]
 * 
 */
#ifdef SCHAT_CLIENT
void Server::removeDirectConnection(ServerSocket *socket)
{
  QString nick = socket->nick();
  
  if (directPeers.contains(nick)) {
    directPeers.remove(nick);
  }
  socket->deleteLater();
}
#endif
