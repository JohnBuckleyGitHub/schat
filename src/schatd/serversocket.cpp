/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>
#include <QtNetwork>
#include <stdlib.h>

#include "serversocket.h"
#include "version.h"
#include "profile.h"


/** [public]
 * 
 */
ServerSocket::ServerSocket(QObject *parent)
  : QTcpSocket(parent)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "ServerSocket::ServerSocket(QObject *parent)";
  #endif
  
  currentState = sChatStateWaitingForGreeting;
  currentBlock.setDevice(this);
  currentBlock.setVersion(sChatStreamVersion);
  protocolError = 0;
  failurePongs = 0;
  
  #ifdef SCHAT_CLIENT
  connect(this, SIGNAL(appendDirectParticipant(const QString &)), parent, SLOT(appendDirectParticipant(const QString &)));
  #endif
  
  connect(this, SIGNAL(appendParticipant(const QString &)), parent, SLOT(appendParticipant(const QString &)));
  connect(this, SIGNAL(relayMessage(const QString &, const QString &, const QString &)), parent, SLOT(relayMessage(const QString &, const QString &, const QString &)));
  
  connect(this, SIGNAL(disconnected()), &pingTimer, SLOT(stop()));
  connect(this, SIGNAL(disconnected()), parent, SLOT(disconnected()));
  connect(this, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(&pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
  connect(this, SIGNAL(error(QAbstractSocket::SocketError)), parent, SLOT(connectionError(QAbstractSocket::SocketError)));
  connect(this, SIGNAL(clientSendNewProfile(quint16, const QString &, const QString &)), parent, SLOT(clientSendNewProfile(quint16, const QString &, const QString &)));

  nextBlockSize = 0;
  pingTimer.setInterval(PingInterval);
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeGreetingOk
 */
void ServerSocket::send(quint16 opcode)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ServerSocket::send(quint16 opcode)" << opcode;
  #endif
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode;
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeSendPrivateMessage
 */
void ServerSocket::send(quint16 opcode, const QString &n, const QString &m)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ServerSocket::send(quint16 opcode, const QString &n, const QString &m)" << opcode;
  #endif
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << n << m;  
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);  
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeParticipantLeft
 */
void ServerSocket::send(quint16 opcode, const QString &s)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ServerSocket::send(quint16 opcode, const QString &s)" << opcode;
  #endif
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << s;  
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);  
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeError
 *   sChatOpcodeMaxDoublePingTimeout
 */
void ServerSocket::send(quint16 opcode, quint16 err)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ServerSocket::send(quint16 opcode, const QString &s)" << opcode << err;
  #endif
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << err; 
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));
  write(block);  
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeNewParticipant
 *   sChatOpcodeNewParticipantQuiet
 */
void ServerSocket::send(quint16 opcode, quint16 s, const QStringList &list)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ServerSocket::send(quint16 opcode, quint16 s, const QStringList &list)" << opcode << s << list;
  #endif
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << s;
  
  foreach (QString str, list)
    out << str;
  
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);  
}


/** [private slots]
 * Слот вызывается сигналом `readyRead()`
 * т.е. слот вызывается тогда когда сокет готов
 * прочитать новые данные. 
 */
void ServerSocket::readyRead()
{
  // Состояние `sChatStateWaitingForGreeting`
  // Ожидаем пакет с опкодом `sChatOpcodeGreeting`
  // Если пришёл другой пакет, рвём соединение `abort()`
  // Если получен нужный пакет устанавливаем состояние `sChatStateReadingGreeting`
  if (currentState == sChatStateWaitingForGreeting) {
    if (!readBlock())
      return;
    if (currentCommand != sChatOpcodeGreeting) {
      abort();
      return;
    }
    currentState = sChatStateReadingGreeting;
  }
  
  // Состояние `sChatStateReadingGreeting`
  // Вызываем функцию `readGreeting()` для чтения пакета `sChatOpcodeGreeting`
  if (currentState == sChatStateReadingGreeting)
    readGreeting();
  
  while (readBlock()) {
    
    switch (currentCommand) {
      case sChatOpcodeSendMessage:
        currentBlock >> channel >> message;
        
        #ifdef SCHAT_CLIENT
        if (pFlag == sChatFlagDirect) {
          send(sChatOpcodeSendPrvMessageEcho, channel, message);
          emit newMessage(profile->nick(), message);
        }
        else
        #endif
          emit relayMessage(channel, profile->nick(), message);
        break;
        
      case sChatOpcodeNewProfile:
        clientSendNewProfile();
        break;
      
      case sChatOpcodePong:
        failurePongs = 0;
        break;
      
      // Опкод `sChatOpcodePing`
      // В ответ отсылаем `sChatOpcodePong`
      case sChatOpcodePing:
        send(sChatOpcodePong);
        break;
      
      // Опкод `sChatOpcodeClientQuit`
      // Клиент выходит из часа
      case sChatOpcodeClientQuit:
        abort();
        break;
        
      default:
        #ifdef SCHAT_DEBUG
        qDebug() << "Invalid Opcode";
        #endif
        abort();
        break;
    }    
  }
  
  // После чтения данных запускаем Ping таймер.
  // Время срабатывания таймера выбирается случайно
  // в диапазоне от `PingMinInterval` до `PingMinInterval + PingMutator`
  // т.е. c настройками по умолчанию от 4 до 6 секунд.
  // Сигнал `timeout()` таймера вызывает слот `sendPing()`
  if (!pingTimer.isActive())
    pingTimer.start();
}


/** [private slots]
 * Отправляем пакет с опкодом `sChatOpcodePing`
 * и увеличиваем счётчик `failurePongs` на 1
 * ответный пакет `sChatOpcodePong` сбрасывает этот счётчик.
 * В случае если отправка двух пакетов завершилась неудачей,
 * т.е. не получено подтверждение `sChatOpcodePong`,
 * то разрываем соединение.
 */
void ServerSocket::sendPing()
{
  if (failurePongs < 2) {
    send(sChatOpcodePing);
    ++failurePongs;
  }
  else
    abort();
}


/** [private]
 * Читаем блок данных
 */
bool ServerSocket::readBlock()
{
  if (nextBlockSize == 0) {
    if (bytesAvailable() < sizeof(quint16))
      return false;
    currentBlock >> nextBlockSize;
  }

  if (bytesAvailable() < nextBlockSize)
    return false;

  currentBlock >> currentCommand;
  
  nextBlockSize = 0;
  return true;
}


/** [private]
 * 
 */
void ServerSocket::clientSendNewProfile()
{
  qDebug() << "void ServerSocket::clientSendNewProfile()";
  
  quint16 sex;
  QString nick;
  QString name;
  
  currentBlock >> sex >> nick >> name;
  
  if (Profile::isValidNick(nick))
    emit clientSendNewProfile(sex, nick, name);
}


/** [private]
 * Функция для чтения пакета `sChatOpcodeGreeting`
 * Проверяем корректность данных, в случае ошибки
 * посылаем пакет `sChatOpcodeError`
 * и рвём соединение `disconnectFromHost()`
 * 
 * В случае успеха вызываем сигнал `appendParticipant(const QString &p)`
 * и устанавливаем состояние `sChatStateWaitingForChecking`
 * т.е. сервер должен проверить имя на дубликат.
 */
void ServerSocket::readGreeting()
{
  QString FullName;
  QString Nick;
  QString UserAgent;
  quint16 err = 0;
  quint8 Sex;
  quint8 version;
  
  currentBlock >> version >> pFlag >> Sex >> Nick >> FullName >> UserAgent;
  
  profile = new Profile(Nick, FullName, Sex, this);
  profile->setUserAgent(UserAgent);
  profile->setHost(peerAddress().toString());
  
  if (version != sChatProtocolVersion)
    err = sChatErrorBadProtocolVersion;
  else if (!(pFlag == sChatFlagNone || pFlag == sChatFlagDirect))
    err = sChatErrorBadGreetingFlag;
  else if (!profile->isValidNick())
    err = sChatErrorBadNickName;
  else if (!profile->isValidUserAgent())
    err = sChatErrorBadUserAgent;
  else if (!isValid())
    err = sChatErrorInvalidConnection;
  
  #ifndef SCHAT_CLIENT
  if (pFlag == sChatFlagDirect) {
    err = sChatErrorDirectNotAllow;
    profile->setNick("#DUBLICATE");
  }
  #endif
  
  if (err) {
    send(sChatOpcodeError, err);
    disconnectFromHost();
    return;
  }
  
  #ifdef SCHAT_DEBUG
//  qDebug() << (nick + "@" + peerAddress().toString() + ":" + QString::number(peerPort())) << userAgent;
  #endif
  
  // При прямом подключении `sChatFlagDirect` не проверяем имя на дубликаты
  // и не отсылаем его в общий список.
  #ifdef SCHAT_CLIENT
  if (pFlag == sChatFlagDirect)
    emit appendDirectParticipant(profile->nick());
  else
  #endif
    emit appendParticipant(profile->nick());
  
  currentState = sChatStateWaitingForChecking;
}


/** [private]
 * 
 */
#ifdef SCHAT_CLIENT
void ServerSocket::sendLocalProfile()
{
  localProfile->setHost(localAddress().toString());  
  send(sChatOpcodeNewParticipantQuiet, localProfile->sex(), localProfile->toList());  
}
#endif
