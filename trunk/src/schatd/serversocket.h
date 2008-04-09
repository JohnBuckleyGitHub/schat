/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include <QDataStream>
#include <QTcpSocket>
#include <QTimer>

#include "profile.h"
#include "protocol.h"

class ServerSocket : public QTcpSocket
{
  Q_OBJECT

public:  
  ServerSocket(QObject *parent = 0);
  inline QString nick()                              { return profile->nick(); }
  inline QStringList participantInfo() const         { return profile->toList(); }
  inline quint16 sex()                               { return profile->sex(); }
  inline void setNick(const QString &n)              { profile->setNick(n); }
  inline void setProtocolError(quint16 e)            { protocolError = e; }
  inline void setState(quint16 state)                { currentState = state; }
  void send(quint16 opcode);
  void send(quint16 opcode, const QString &n, const QString &m);
  void send(quint16 opcode, const QString &s);
  void send(quint16 opcode, quint16 err);
  void send(quint16 opcode, quint16 s, const QStringList &list);
  
  // FIXME добавить #define ...
  inline bool isDirect()                             { if (pFlag == sChatFlagDirect) return true; else return false; }
  inline void setLocalProfile(Profile *p)            { localProfile = p; }
  void sendLocalProfile();

signals:
  void appendParticipant(const QString &p);
  void needParticipantList();
  void relayMessage(const QString &channel, const QString &nick, const QString &message);
  
  // FIXME добавить #define ...
  void appendDirectParticipant(const QString &p);

private slots:
  void readyRead();
  void sendPing();

private:
  bool readBlock();
  void readGreeting();
  
  int failurePongs;
  Profile *profile;
  QDataStream currentBlock;
  QString channel;
  QString message;
  QTimer pingTimer;
  quint16 currentCommand;
  quint16 currentState;
  quint16 nextBlockSize;
  quint16 protocolError;
  quint8 pFlag;
  
  // FIXME добавить #define ...
  Profile *localProfile;
};

#endif /*SERVERSOCKET_H_*/
