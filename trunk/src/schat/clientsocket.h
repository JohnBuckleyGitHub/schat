/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include <QListWidgetItem>
#include <QTcpSocket>
#include <QTimer>

class ClientSocket : public QTcpSocket
{
  Q_OBJECT

public:
  ClientSocket(QObject *parent = 0);
  quint16 protocolError() { return _protocolError; }
  void send(quint16 opcode);
  void send(quint16 opcode, const QString &n, const QString &m);
  void send(quint16 opcode, const QString &s);
  void setFullName(const QString &n) { fullName = n; }
  void setNick(const QString &n) { nick = n; }
  void setSex(quint8 s) { sex = s; }

signals:
  void newMessage(const QString &nick, const QString &message);
  void newParticipant(quint16 sex, const QStringList &info, bool echo = true);
  void newPrivateMessage(const QString &nick, const QString &message, const QString &sender);
  void participantLeft(const QString &nick);
  void readyForUse();
  
private slots:
  void initTimeout();
  void readyRead();
  void sendGreeting();
  void sendPing();
  
private:
  bool readBlock();
  void newParticipant(bool echo = true);
  
  int failurePongs;
  QDataStream currentBlock;
  QString fullName;
  QString message;  
  QString nick;
  QString userAgent;
  QString userMask;
  QTimer pingTimeout;
  quint16 _protocolError;
  quint16 currentCommand;
  quint16 currentState;
  quint16 nextBlockSize;
  quint8 sex;
};

#endif /*CLIENTSOCKET_H_*/
