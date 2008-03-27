/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include <QTcpSocket>
#include <QDataStream>
#include <QString>

class ServerSocket : public QTcpSocket
{
  Q_OBJECT

public:  
  ServerSocket(QObject *parent = 0);
  void setProtocolError(quint16 e) { protocolError = e; }
  void sendGreeting();
//  void send(quint16 opcode);
  void send(quint16 opcode, quint16 err);
  void send(quint16 opcode, const QString &s);
  void send(quint16 opcode, const QString &n, const QString &m);
  QString nickname() { return nick; }
  void setNick(const QString &n) { nick = n; }

signals:
  void appendParticipant(const QString &p);
  void needParticipantList();
  void relayMessage(const QString &channel, const QString &nick, const QString &message);

private slots:
  void readyRead();

private:
  bool readBlock();
  void readGreeting();
  
  QDataStream currentBlock;
  QString nick;
  QString userAgent;
  QString userMask;
  QString message;
  QString channel;
  
  quint16 protocolError;
  quint16 currentCommand;
  quint16 currentState;
  quint16 nextBlockSize;
};

#endif /*SERVERSOCKET_H_*/
