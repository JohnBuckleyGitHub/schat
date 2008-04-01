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
  qDebug() << "ClientSocket::ClientSocket(QObject *parent)";
  
  currentState = sChatStateDisconnected;
  currentBlock.setDevice(this);
  currentBlock.setVersion(sChatStreamVersion);
  
  connect(this, SIGNAL(newParticipant(quint16, const QStringList &, bool)),
          parent, SLOT(newParticipant(quint16, const QStringList &, bool)));
  connect(this, SIGNAL(participantLeft(const QString &)),
          parent, SLOT(participantLeft(const QString &)));
  connect(this, SIGNAL(newMessage(const QString &, const QString &)),
          parent, SLOT(newMessage(const QString &, const QString &)));
  connect(this, SIGNAL(newPrivateMessage(const QString &, const QString &, const QString &)),
          parent, SLOT(newPrivateMessage(const QString &, const QString &, const QString &)));
  
  connect(this, SIGNAL(connected()), this, SLOT(sendGreeting()));
  connect(this, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(this, SIGNAL(readyForUse()), parent, SLOT(readyForUse()));
  connect(this, SIGNAL(disconnected()), parent, SLOT(disconnected()));
  connect(this, SIGNAL(error(QAbstractSocket::SocketError)), parent, SLOT(connectionError(QAbstractSocket::SocketError)));
  connect(&pingTimeout, SIGNAL(timeout()), this, SLOT(sendPing()));

  nextBlockSize = 0;
  _protocolError = 0;
  failurePongs = 0;
  pingTimeout.setInterval((PingMinInterval + PingMutator) * 2);
  
  QTimer::singleShot(InitTimeout, this, SLOT(initTimeout()));
}


/** [private slots]
 * Отправляем пакет с опкодом `sChatOpcodeGreeting`
 * Пакет является приветственным сообщением.
 * До начала отправки устанавливаем состояние
 * сокета `sChatStateWaitingForGreeting` вместо `sChatStateDisconnected`
 * Слот вызывается сигналом `connected()`
 **
 * Формат пакета:
 * quint16 - размер пакета
 * quint16 - опкод `sChatOpcodeGreeting`
 * quint8  - версия протокола `sChatProtocolVersion`
 * quint8  - флаг, в данное время всегда `0`
 * QString - ник участника
 * QString - полное имя участника
 * quint8  - пол участника (мужской: `0`, женский: `1`)
 * QString - строка с названием и версией клиента (разделитель '/')
 */
void ClientSocket::sendGreeting()
{
  qDebug() << "ClientSocket::sendGreeting()";
  
  currentState = sChatStateWaitingForGreeting;
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0)
      << quint16(sChatOpcodeGreeting)
      << quint8(sChatProtocolVersion)
      << quint8(0)
      << sex
      << nick
      << fullName
      << QString("Simple Chat/%1").arg(SCHAT_VERSION);
    
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));
  write(block);
}


/** [private slots]
 * Слот вызывается сигналом `readyRead()`
 * т.е. слот вызывается тогда когда сокет готов
 * прочитать новые данные. 
 */
void ClientSocket::readyRead()
{
  qDebug() << "ClientSocket::readyRead()" << (int) state() ;
  
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
        currentBlock >> _protocolError;
        qDebug() << "_protocolError:" << _protocolError;
      }
      abort();
      return;
    }
  }
  
  QString textBlock;
  quint16 err;
  
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
        emit newPrivateMessage(textBlock, message, nick);
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
        currentBlock >> err;
        qDebug() << "PROTOCOL ERROR:" << err;
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
        qDebug() << "Invalid Opcode";
        abort();
        break;
    }    
  }  
  pingTimeout.start();
}


/** [private]
 * 
 */
void ClientSocket::newParticipant(bool echo)
{
  qDebug() << "void ClientSocket::newParticipant(bool echo)" << echo;
  
  quint16 sex;
  QStringList info;
  currentBlock >> sex;
  
  for (int i = 0; i < 4; ++i) {
    QString s;
    currentBlock >> s;
    info << s;
  }
  
  for (int i = 0; i < info.size(); ++i)
    qDebug() << "::" << info.at(i);
  
  if (echo)
    emit newParticipant(sex, info);
  else
    emit newParticipant(sex, info, false);

}


/** [public]
 * 
 */
void ClientSocket::send(quint16 opcode)
{
  qDebug() << "ClientSocket::send(quint16 opcode)" << opcode;
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << quint16(opcode);
  
//  switch (opcode) {
//    case sChatOpcodeSendMessage:
//      out << nick << message;      
//      break;
//  }
  
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));
      
  write(block);  
}


/** [public]
 * 
 */
void ClientSocket::send(quint16 opcode, const QString &s)
{
  qDebug() << "ClientSocket::send(quint16 opcode, const QString &s)" << opcode;
  
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
void ClientSocket::send(quint16 opcode, const QString &n, const QString &m)
{
  qDebug() << "ClientSocket::send(quint16 opcode, const QString &n, const QString &m)" << opcode << n << m;
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << n << m;  
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);  
}


/** [private]
 * 
 */
bool ClientSocket::readBlock()
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


/** [private slots]
 * 
 */
void ClientSocket::sendPing()
{
  qDebug() << "ClientSocket::sendPing()";
  
  if (failurePongs < 1) {
    send(sChatOpcodePing);
    ++failurePongs;
  }
  else
    abort();
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
