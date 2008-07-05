/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
  inline QString byeMsg()                            { return m_profile->byeMsg(); }
  inline QString nick()                              { return m_profile->nick(); }
  inline QStringList participantInfo() const         { return m_profile->toList(); }
  inline quint16 sex()                               { return m_profile->sex(); }
  inline void setFullName(const QString &fullName)   { m_profile->setFullName(fullName); }
  inline void setNick(const QString &n)              { m_profile->setNick(n); }
  inline void setSex(const quint16 sex = 0)          { m_profile->setSex(sex); }
  inline void setState(quint16 state)                { m_state = state; }
  void send(quint16 opcode);
  void send(quint16 opcode, const QString &n, const QString &m);
  void send(quint16 opcode, const QString &s);
  void send(quint16 opcode, quint16 err);
  void send(quint16 opcode, quint16 s, const QStringList &list);
  
  #ifdef SCHAT_CLIENT
  inline bool isDirect()                             { if (m_flag == sChatFlagDirect) return true; else return false; }
  inline void setLocalProfile(Profile *p)            { localProfile = p; }
  void sendLocalProfile();
  #endif
  
signals:
  void appendParticipant(const QString &p);
  void clientSendNewProfile(quint16 sex, const QString &nick, const QString &name);
  void needParticipantList();
  void relayMessage(const QString &channel, const QString &nick, const QString &message);
  
  #ifdef SCHAT_CLIENT
  void appendDirectParticipant(const QString &p);
  void newMessage(const QString &nick, const QString &message);
  #endif

private slots:
  void readyRead();
  void sendPing();

private:
  bool readBlock();
  void clientSendNewProfile();
  void opSendByeMsg();
  void opSendMessage();
  void readGreeting();
  
  int m_failurePongs;
  Profile *m_profile;
  QDataStream m_stream;
  QTimer m_pingTimer;
  quint16 m_command;
  quint16 m_nextBlockSize;
  quint16 m_state;
  quint8 m_flag;
  
  #ifdef SCHAT_CLIENT
  Profile *localProfile;
  #endif
};

#endif /*SERVERSOCKET_H_*/
