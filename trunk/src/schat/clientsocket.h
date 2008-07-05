/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include <QListWidgetItem>
#include <QTcpSocket>
#include <QTimer>

#include "profile.h"

class ClientSocket : public QTcpSocket
{
  Q_OBJECT

public:
  ClientSocket(QObject *parent = 0);
  inline void setDirect(bool d)      { direct = d; }
  inline void setProfile(Profile *p) { profile = p; }
  quint16 protocolError() { return pError; }
  void devSend();
  void quit();
  void send(quint16 opcode);
  void send(quint16 opcode, const QString &n, const QString &m);
  void send(quint16 opcode, const QString &s);
  void send(quint16 opcode, quint16 s, const QStringList &list);

signals:
  void changedNick(quint16 sex, const QStringList &list);
  void changedProfile(quint16 sex, const QStringList &list, bool echo = true);
  void newMessage(const QString &nick, const QString &message);
  void newParticipant(quint16 sex, const QStringList &info, bool echo = true);
  void newPrivateMessage(const QString &nick, const QString &message, const QString &sender);
  void participantLeft(const QString &nick, const QString &bye);
  void readyForUse();
  
private slots:
  void initTimeout();
  void readyRead();
  void sendGreeting();
  void sendPing();
  
private:
  bool readBlock();
  void newParticipant(bool echo = true);
  void opParticipantLeft();
  void readChangedNick();
  void readChangedProfile();
  
  bool direct;
  int failurePongs;
  Profile *profile;
  QDataStream currentBlock;
  QString message;  
  QString remoteNick;
  QTimer pingTimeout;
  quint16 currentCommand;
  quint16 currentState;
  quint16 nextBlockSize;
  quint16 pError;
};

#endif /*CLIENTSOCKET_H_*/
