/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include <QDataStream>
#include <QTcpSocket>
#include <QTimer>

class ServerSocket : public QTcpSocket
{
  Q_OBJECT

public:  
  ServerSocket(QObject *parent = 0);
  void setProtocolError(quint16 e) { protocolError = e; }
  void send(quint16 opcode);
  void send(quint16 opcode, quint16 err);
  void send(quint16 opcode, const QString &s);
  void send(quint16 opcode, const QString &n, const QString &m);
  void send(quint16 opcode, quint16 s, const QStringList &list);
  QString nickname() { return nick; }
  void setNick(const QString &n) { nick = n; }
  void setState(quint16 state) { currentState = state; }
  QStringList participantInfo();
  quint16 participantSex() { return sex; }  

signals:
  void appendParticipant(const QString &p);
  void needParticipantList();
  void relayMessage(const QString &channel, const QString &nick, const QString &message);

private slots:
  void readyRead();
  void sendPing();

private:
  bool readBlock();
  void readGreeting();
  
  int failurePongs;
  QDataStream currentBlock;
  QString nick;
  QString fullName;
  quint8 sex;
  QString userAgent;
  QString userMask;
  QString message;
  QString channel;
  QTimer pingTimer;
  quint16 protocolError;
  quint16 currentCommand;
  quint16 currentState;
  quint16 nextBlockSize;
};

#endif /*SERVERSOCKET_H_*/
