/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>
#include <QtNetwork>

#include "clientsocket.h"
#include "protocol.h"
#include "version.h"

static const int InitTimeout = 5 * 1000;

ClientSocket::ClientSocket(QObject *parent)
  : QTcpSocket(parent)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "ClientSocket::ClientSocket(QObject *parent)";
  #endif  
  
  currentState = sChatStateDisconnected;
  currentBlock.setDevice(this);
  currentBlock.setVersion(sChatStreamVersion);
  
  connect(this, SIGNAL(connected()), this, SLOT(sendGreeting()));
  connect(this, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(&pingTimeout, SIGNAL(timeout()), this, SLOT(sendPing()));

  nextBlockSize = 0;
  pError = 0;
  failurePongs = 0;
  direct = false;
  pingTimeout.setInterval(PingInterval * 2);
  
  QTimer::singleShot(InitTimeout, this, SLOT(initTimeout()));
}


/** [public]
 * 
 */
void ClientSocket::quit()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "ClientSocket::quit()" << state();
  #endif
  
  if (state() == QAbstractSocket::ConnectedState) {
    send(sChatOpcodeClientQuit);
    disconnectFromHost();
    waitForDisconnected(1000);
  }
}



/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeClientQuit
 *   sChatOpcodePing
 *   sChatOpcodePong
 */
void ClientSocket::send(quint16 opcode)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ClientSocket::send(quint16 opcode)" << opcode;
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
 *   sChatOpcodeSendMessage
 * 
 */
void ClientSocket::send(quint16 opcode, const QString &n, const QString &m)
{
  #ifdef SCHAT_DEBUG 
  qDebug() << "ClientSocket::send(quint16 opcode, const QString &n, const QString &m)" << opcode << n << m;
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
 * 
 */
void ClientSocket::send(quint16 opcode, const QString &s)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "ClientSocket::send(quint16 opcode, const QString &s)" << opcode;
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
 *   sChatOpcodeNewProfile
 */
void ClientSocket::send(quint16 opcode, quint16 s, const QStringList &list)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ClientSocket::send(quint16 opcode, quint16 s, const QStringList &list)" << opcode << s << list;
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
 * Если спустя `InitTimeout` секунд состояние
 * всё еще равно `QAbstractSocket::ConnectingState`
 * то возможно пытаемся подключится к несуществующему ip
 * таким образом экономим 25 секунд на каждой попытки подключения.
 */
void ClientSocket::initTimeout()
{
  if (state() == QAbstractSocket::ConnectingState)
    emit disconnected();
}


/** [private slots]
 * Слот вызывается сигналом `readyRead()`
 * т.е. слот вызывается тогда когда сокет готов
 * прочитать новые данные. 
 */
void ClientSocket::readyRead()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "ClientSocket::readyRead()" << (int) state();
  #endif
  
  // Состояние `sChatStateWaitingForGreeting`
  // Ожидаем пакет `sChatOpcodeGreetingOk`
  if (currentState == sChatStateWaitingForGreeting) {
    if (!readBlock())
      return;
    if (currentCommand == sChatOpcodeGreetingOk) {
      currentState = sChatStateReadyForUse;
      emit readyForUse();
    }
    else {
      if (currentCommand == sChatOpcodeError) {
        currentBlock >> pError;
        #ifdef SCHAT_DEBUG
        qDebug() << "PROTOCOL ERROR:" << pError;
        #endif
      }
      abort();
      return;
    }
  }
  
  QString textBlock;
  quint16 err;
  pError = 0;
  
  while (readBlock()) {
    
    switch (currentCommand) {
      case sChatOpcodeMaxDoublePingTimeout:
        currentBlock >> err;
        pingTimeout.setInterval(err * 1000);
        break;
      
      case sChatOpcodeSendMessage:      
        currentBlock >> textBlock >> message;
        emit newMessage(textBlock, message);
        break;
      
      case sChatOpcodeSendPrivateMessage:
        currentBlock >> textBlock >> message;
        emit newPrivateMessage(textBlock, message, textBlock);
        break;
        
      case sChatOpcodeSendPrvMessageEcho:
        currentBlock >> textBlock >> message;
        emit newPrivateMessage(textBlock, message, profile->nick());
        break;
      
      // Опкод `sChatOpcodeNewParticipant` - новый участник вошёл в чат
      case sChatOpcodeNewParticipant:
        newParticipant();
        break;
      
      // Опкод `sChatOpcodeNewParticipantQuiet` - аналогичен `sChatOpcodeNewParticipant`
      // но участник добавляется тихо, без уведомления в окне чата.
      // Данные команды отправляются сразу после `sChatOpcodeGreetingOk`
      // и служат для получения списка участников (один пакет - один участник)
      case sChatOpcodeNewParticipantQuiet:
        newParticipant(false);
        break;
      
      // Опкод `sChatOpcodeParticipantLeft` - Уведомляет в выходе из чата участника
      case sChatOpcodeParticipantLeft:
        currentBlock >> textBlock;
        emit participantLeft(textBlock);
        break;
        
      // Опкод `sChatOpcodeError` - Сервер вернул ошибку
      case sChatOpcodeError:
        currentBlock >> pError;
        #ifdef SCHAT_DEBUG
        qDebug() << "PROTOCOL ERROR:" << pError;
        #endif
        break;
        
      case sChatOpcodePong:
        failurePongs = 0;
        break;
      
      // Опкод `sChatOpcodePing`
      // В ответ отсылаем `sChatOpcodePong`
      case sChatOpcodePing:
        send(sChatOpcodePong);
        pingTimeout.start();
        break;
        
      default:
        #ifdef SCHAT_DEBUG
        qDebug() << "Invalid Opcode";
        #endif
        abort();
        break;
    }    
  }  
  pingTimeout.start();
}


/** [private slots]
 * Отправляем пакет с опкодом `sChatOpcodeGreeting`
 * Пакет является приветственным сообщением.
 * До начала отправки устанавливаем состояние
 * сокета `sChatStateWaitingForGreeting` вместо `sChatStateDisconnected`
 * Слот вызывается сигналом `connected()`
 */
void ClientSocket::sendGreeting()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "ClientSocket::sendGreeting()";
  #endif
  
  currentState = sChatStateWaitingForGreeting;
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0)
      << sChatOpcodeGreeting
      << sChatProtocolVersion;
  
  if (direct)
    out << sChatFlagDirect;
  else
    out << sChatFlagNone;
  
  profile->toStream(out); // Записываем профиль в поток
    
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));
  write(block);
}


/** [private slots]
 * 
 */
void ClientSocket::sendPing()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "ClientSocket::sendPing()";
  #endif
  
  if (failurePongs < 1) {
    send(sChatOpcodePing);
    ++failurePongs;
  }
  else
    abort();
}


/** [private]
 * 
 */
bool ClientSocket::readBlock()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "ServerSocket::readBlock()";
  #endif
  
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
void ClientSocket::newParticipant(bool echo)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ClientSocket::newParticipant(bool echo)" << echo;
  #endif
  
  quint16 sex;
  QStringList info;
  currentBlock >> sex;
  
  for (int i = 0; i < 4; ++i) {
    QString s;
    currentBlock >> s;
    info << s;
  }
  
  if (direct)
    remoteNick = info.at(0);
    
  if (echo)
    emit newParticipant(sex, info);
  else
    emit newParticipant(sex, info, false);
}
