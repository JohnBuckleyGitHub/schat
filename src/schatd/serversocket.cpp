/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>
#include <QtNetwork>
#include <stdlib.h>

#include "serversocket.h"
#include "protocol.h"

ServerSocket::ServerSocket(QObject *parent)
  : QTcpSocket(parent)
{
  qDebug() << "ServerSocket::ServerSocket(QObject *parent)";
  
  currentState = sChatStateWaitingForGreeting;
  currentBlock.setDevice(this);
  currentBlock.setVersion(sChatStreamVersion);
  protocolError = 0;
  failurePongs = 0;
  
  connect(this, SIGNAL(appendParticipant(const QString &)),
          parent, SLOT(appendParticipant(const QString &)));
  connect(this, SIGNAL(relayMessage(const QString &, const QString &, const QString &)),
          parent, SLOT(relayMessage(const QString &, const QString &, const QString &)));
  
  connect(this, SIGNAL(disconnected()), &pingTimer, SLOT(stop()));
  connect(this, SIGNAL(disconnected()), parent, SLOT(disconnected()));
  connect(this, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(&pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
  connect(this, SIGNAL(error(QAbstractSocket::SocketError)), parent, SLOT(connectionError(QAbstractSocket::SocketError)));

  nextBlockSize = 0;
  qsrand(time(NULL));
}


/** [public]
 * 
 */
QStringList ServerSocket::participantInfo()
{
  QStringList stringList;
  stringList << nick
             << fullName
             << userAgent
             << peerAddress().toString();
  
  return stringList;  
}


/** [public]
 * 
 */
void ServerSocket::send(quint16 opcode)
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode;
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);
}


/** [public]
 * 
 */
void ServerSocket::send(quint16 opcode, const QString &n, const QString &m)
{
  qDebug() << "ServerSocket::send(quint16 opcode, const QString &n, const QString &m)" << opcode;
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << n << m;  
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);  
}


/** [public]
 * 
 */
void ServerSocket::send(quint16 opcode, const QString &s)
{
  qDebug() << "ServerSocket::send(quint16 opcode, const QString &s)" << opcode;
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << s;  
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);  
}


/** [public]
 * 
 */
void ServerSocket::send(quint16 opcode, quint16 err)
{
  qDebug() << "ServerSocket::send(quint16 opcode, const QString &s)" << opcode;
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << err; 
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));
  write(block);  
}


/** [public]
 * 
 */
void ServerSocket::send(quint16 opcode, quint16 s, const QStringList &list)
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << s;
  
  for (int i = 0; i < list.size(); ++i)
    out << list.at(i);
  
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
  qDebug() << "ServerSocket::readyRead()" << (int) state() ;
  
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
        emit relayMessage(channel, nick, message);
        break;
      
      case sChatOpcodePong:
        failurePongs = 0;
        break;
      
      // Опкод `sChatOpcodePing`
      // В ответ отсылаем `sChatOpcodePong`
      case sChatOpcodePing:
        send(sChatOpcodePong);
        break;
        
      default:
        qDebug() << "Invalid Opcode";
        abort();
        break;
    }    
  }
  
  // После чтения данных запускаем Ping таймер.
  // Время срабатывания таймера выбирается случайно
  // в диапазоне от `PingMinInterval` до `PingMinInterval + PingMutator`
  // т.е. c настройками по умолчанию от 4 до 6 секунд.
  // Сигнал `timeout()` таймера вызывает слот `sendPing()`
  pingTimer.start(PingMinInterval + rand() % PingMutator);
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
  qDebug() << "ServerSocket::sendPing()" << failurePongs;
  
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
  qDebug() << "ServerSocket::readBlock()";
  
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
  quint8 version;
  quint8 flag;
  quint16 err = 0;
  
  currentBlock >> version >> flag >> sex >> nick >> fullName >> userAgent;
  
  if (version != sChatProtocolVersion)
    err = sChatErrorBadProtocolVersion;
  else if (flag != 0)
    err = sChatErrorBadGreetingFlag;
  else if (nick.isEmpty() || nick == "#DUBLICATE")
    err = sChatErrorBadNickName;
  else if (userAgent.isEmpty())
    err = sChatErrorBadUserAgent;
  else if (!isValid())
    err = sChatErrorInvalidConnection;
  
  if (err) {
    send(sChatOpcodeError, err);
    disconnectFromHost();
  }
  
  userMask = nick + "@" + peerAddress().toString() + ":" + QString::number(peerPort());
  qDebug() << userMask << userAgent;
  
  emit appendParticipant(nick);
  
  currentState = sChatStateWaitingForChecking;
}
