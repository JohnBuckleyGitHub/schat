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
  QString nickname() { return nick; }
  QStringList participantInfo() const;
  quint16 participantSex() { return sex; } 
  void send(quint16 opcode);
  void send(quint16 opcode, const QString &n, const QString &m);
  void send(quint16 opcode, const QString &s);
  void send(quint16 opcode, quint16 err);
  void send(quint16 opcode, quint16 s, const QStringList &list);
  void setNick(const QString &n) { nick = n; }
  void setProtocolError(quint16 e) { protocolError = e; }
  void setState(quint16 state) { currentState = state; }
  
  // FIXME добавить #define ...
  void setLocalFullName(const QString &name) { localFullName = name; }
  void setLocalNick(const QString &nick) { localNick = QChar('#') + nick; } 
  void setLocalSex(quint8 s) { localSex = s; }

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
  QString channel;
  QString fullName;
  QString message;
  QString nick;
  QString userAgent;
  QTimer pingTimer;
  quint16 currentCommand;
  quint16 currentState;
  quint16 nextBlockSize;
  quint16 protocolError;
  quint8 pFlag;
  quint8 sex;
  
  // FIXME добавить #define ...
  void sendLocalProfile();
  QString localFullName;
  QString localNick; 
  quint8 localSex;
};

#endif /*SERVERSOCKET_H_*/
