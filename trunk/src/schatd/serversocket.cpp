#include <QtCore>
#include <QtNetwork>

#include "serversocket.h"
#include "protocol.h"

ServerSocket::ServerSocket(QObject *parent)
  : QTcpSocket(parent)
{
  qDebug() << "ServerSocket::ServerSocket(QObject *parent)";
  
  currentState = SCHAT_STATE_WAITING_FOR_GREETING;
  currentBlock.setDevice(this);
  currentBlock.setVersion(sChatStreamVersion);
  protocolError = 0;
  
  connect(this, SIGNAL(appendParticipant(const QString &)), parent, SLOT(appendParticipant(const QString &)));
  connect(this, SIGNAL(needParticipantList()), parent, SLOT(needParticipantList()));
  connect(this, SIGNAL(relayMessage(const QString &, const QString &)), parent, SLOT(relayMessage(const QString &, const QString &)));
  
  connect(this, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(this, SIGNAL(disconnected()), parent, SLOT(disconnected()));
  connect(this, SIGNAL(error(QAbstractSocket::SocketError)), parent, SLOT(connectionError(QAbstractSocket::SocketError)));

  nextBlockSize = 0;
}

//
void ServerSocket::readyRead()
{
  qDebug() << "ServerSocket::readyRead()" << (int) state() ;
  
  // Если ещё не получили команду приветствия (состояние SCHAT_STATE_WAITING_FOR_GREETING)
  // пытаемся прочитать блок, и если послана команда SCHAT_GREETING
  // устанавливаем состояние SCHAT_STATE_READING_GREETING
  if (currentState == SCHAT_STATE_WAITING_FOR_GREETING) {
    if (!readBlock())
      return;
    if (currentCommand != sChatOpcodeGreeting) {
      abort();
      return;
    }
    currentState = SCHAT_STATE_READING_GREETING;
  }
  
  // Пытаемся извлечь данные из приветственного сообщения
  if (currentState == SCHAT_STATE_READING_GREETING)
    readGreeting();
    
  
  
  while (readBlock()) {
    
    switch (currentCommand) {
      case sChatOpcodeSendMessage:
        currentBlock >> message;
        emit relayMessage(nick, message);
        break;
      
      case sChatOpcodeNeedParticipantList:
        emit needParticipantList();
        break;
        
      default:
        qDebug() << "Invalid Opcode";
        abort();
        break;
    }    
  }
}


void ServerSocket::readGreeting()
{
  quint8 version;
  quint8 flag;
  quint16 err = 0;
  
  currentBlock >> version >> flag >> nick >> userAgent;
  
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
  
  currentState = SCHAT_STATE_WAITING_FOR_CHECKING;
}

// Читаем блок данных
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


/**
 * Посылаем подтверждение приветствия
 * Опкод SCHAT_GREETING_OK
 **
 * Формат пакета:
 * quint16 - размер пакета
 * quint16 - опкод SCHAT_GREETING_OK
 */
void ServerSocket::sendGreeting()
{
  qDebug() << "ServerSocket::sendGreeting()";
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(SCHAT_DATA_STREAM_VERSION);
  
  if (!protocolError)
    out << quint16(0) << quint16(SCHAT_GREETING_OK);
  else
    out << quint16(0) << quint16(SCHAT_PROTOCOL_ERROR) << protocolError;

  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));
  
  write(block);
  
  if (!protocolError)
    currentState = SCHAT_STATE_READY_FOR_USE;
}





/** [public]
 * 
 */
//void ServerSocket::send(quint16 opcode)
//{
//  qDebug() << "ServerSocket::send(quint16 opcode)" << opcode;
//  
//  QByteArray block;
//  QDataStream out(&block, QIODevice::WriteOnly);
//  out.setVersion(sChatStreamVersion);
//  out << quint16(0) << quint16(opcode);
//  
//  switch (opcode) {
//    case sChatOpcodeSendMessage:
//      out << nick << message;      
//      break;
//  }
//  
//  out.device()->seek(0);
//  out << quint16(block.size() - sizeof(quint16));
//      
//  write(block);
//}


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
