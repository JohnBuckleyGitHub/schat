/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include <QTcpSocket>
#include <QListWidgetItem>

class ClientSocket : public QTcpSocket
{
  Q_OBJECT

public:
  ClientSocket(QObject *parent = 0);
  void setNick(const QString &n) { nick = n; }
  void send(quint16 opcode);
  void send(quint16 opcode, const QString &s);
  void send(quint16 opcode, const QString &n, const QString &m);
  quint16 protocolError() { return _protocolError; }

signals:
  void newParticipant(const QString &p, bool echo = true);
  void participantLeft(const QString &nick);
  void newMessage(const QString &nick, const QString &message);
  void newPrivateMessage(const QString &nick, const QString &message, const QString &sender);
  
private slots:
  void readyRead();
  void sendGreeting();
  
private:
  bool readBlock();
  void newParticipant(bool echo = true);
  
  QDataStream currentBlock;
  QString nick;
  QString userAgent;
  QString userMask;
  QString message;
  
  quint16 _protocolError;
  quint16 currentCommand;
  quint16 currentState;
  quint16 nextBlockSize;
};

#endif /*CLIENTSOCKET_H_*/
