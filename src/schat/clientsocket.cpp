/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>
#include <QtNetwork>

#include "clientsocket.h"
#include "protocol.h"

ClientSocket::ClientSocket(QObject *parent)
  : QTcpSocket(parent)
{
  qDebug() << "ClientSocket::ClientSocket(QObject *parent)";
  
  currentState = sChatStateDisconnected;
  currentBlock.setDevice(this);
  currentBlock.setVersion(sChatStreamVersion);
  
  connect(this, SIGNAL(newParticipant(const QString &, bool)), parent, SLOT(newParticipant(const QString &, bool)));
  connect(this, SIGNAL(participantLeft(const QString &)), parent, SLOT(participantLeft(const QString &)));
  connect(this, SIGNAL(newMessage(const QString &, const QString &)), parent, SLOT(newMessage(const QString &, const QString &)));
  
  connect(this, SIGNAL(connected()), this, SLOT(sendGreeting()));
  connect(this, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(this, SIGNAL(disconnected()), parent, SLOT(disconnected()));
  connect(this, SIGNAL(error(QAbstractSocket::SocketError)), parent, SLOT(connectionError(QAbstractSocket::SocketError)));

  nextBlockSize = 0;
  _protocolError = 0;
}


/** [private slots]
 * Посылаем серверу приветствие
 * Опкод SCHAT_GREETING
 * Вызывается сигналом connected()
 * Устанавливает состояние соединения в SCHAT_STATE_SENDING_GREETING
 **
 * Формат пакета:
 * quint16 - размер пакета
 * quint16 - опкод SCHAT_GREETING
 * QString - Ник
 * QString - строка с названием и версией клиента (разделитель '/')
 */
void ClientSocket::sendGreeting()
{
  currentState = sChatStateWaitingForGreeting;
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0)
      << quint16(sChatOpcodeGreeting)
      << quint8(sChatProtocolVersion)
      << quint8(0)
      << nick
      << QString("IMPOMEZIA Simple Chat/0.0.0.0 Alpha");
    
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));
    
  qDebug() << "ClientSocket::sendGreeting()" << block.size();
    
  write(block);
}


/** [private slots]
 * 
 */
void ClientSocket::readyRead()
{
  qDebug() << "ClientSocket::readyRead()" << (int) state() ;
  
  // Если ещё не получили подтверждения приветствия (состояние SCHAT_STATE_WAITING_FOR_GREETING)
  // пытаемся прочитать блок, и если послана команда SCHAT_GREETING_OK
  // устанавливаем состояние SCHAT_STATE_READY_FOR_USE
  if (currentState == sChatStateWaitingForGreeting) {
    if (!readBlock())
      return;
    if (currentCommand == sChatOpcodeGreetingOk)
      currentState = sChatStateReadyForUse;
    else {
      if (currentCommand == sChatOpcodeError) {
        currentBlock >> _protocolError;
        qDebug() << "_protocolError:" << _protocolError;
      }
      abort();
      return;
    }
    send(sChatOpcodeNeedParticipantList);
  }
  
  QString textBlock;
  
  while (readBlock()) {
    
    switch (currentCommand) {
      case sChatOpcodeSendMessage:
        currentBlock >> textBlock >> message;
        emit newMessage(textBlock, message);
        break;
      
      case sChatOpcodeNewParticipant:
        newParticipant();
        break;
      
      case sChatOpcodeNewParticipantQuiet:
        newParticipant(false);
        break;
      
      case sChatOpcodeParticipantLeft:
        currentBlock >> textBlock;
        emit participantLeft(textBlock);
        break;
        
      default:
        qDebug() << "Invalid Opcode";
        abort();
        break;
    }    
  }  

}


/** [private]
 * 
 */
void ClientSocket::newParticipant(bool echo)
{
  QString participant;
  currentBlock >> participant;
  
  if (echo)
    emit newParticipant(participant);
  else
    emit newParticipant(participant, false);
    
  qDebug() << "ClientSocket::newParticipant()" << participant;
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
  out << quint16(0) << quint16(opcode);
  
  switch (opcode) {
    case sChatOpcodeSendMessage:
      out << s;      
      break;
  }
  
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
