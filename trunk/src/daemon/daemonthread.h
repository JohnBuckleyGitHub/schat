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

#ifndef DAEMONTHREAD_H_
#define DAEMONTHREAD_H_

#include <QDataStream>
#include <QTcpSocket>
#include <QThread>

class Profile;

class DaemonThread : public QThread
{
  Q_OBJECT

public:
  enum State {
    WaitingGreeting,
    Accepted
  };
  
  DaemonThread(int socketDescriptor, QObject *parent = 0);
  ~DaemonThread();
  void run();
  void send(quint16 opcode);

signals:
//  void error(QTcpSocket::SocketError socketError);
  
private slots:
  void disconnected();
  void readyRead();

private:
  bool opcodeGreeting();
  quint16 verifyGreeting(quint16 version);
  void leave();
  
  bool m_quit;
  int m_descriptor;
  Profile *m_profile;
  QDataStream m_stream;
  QTcpSocket *m_socket;
  quint16 m_nextBlockSize;
  quint16 m_opcode;
  quint8 m_flag;
  State m_state;
};

#endif /*DAEMONTHREAD_H_*/
