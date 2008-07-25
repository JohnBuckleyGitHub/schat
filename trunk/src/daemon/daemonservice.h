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

#ifndef DAEMONSERVICE_H_
#define DAEMONSERVICE_H_

#include <QDataStream>
#include <QTcpSocket>

class AbstractProfile;

class DaemonService : public QObject
{
  Q_OBJECT

public:
  DaemonService(QTcpSocket *socket, QObject *parent = 0);
  ~DaemonService();
  void accessDenied(quint16 reason = 0);
  void accessGranted(quint16 level = 0);
  bool send(quint16 opcode);
  bool send(quint16 opcode, quint16 err);
  
signals:
  void greeting(const QStringList &list);
  void leave(const QString &nick);
  
public slots:
  bool newUser(const QStringList &list, bool echo);
  bool userLeave(const QString &nick, const QString &bye, bool echo);
  void disconnected();
  void readyRead();
  
private slots:
  void check();

private:
  bool opcodeGreeting();
  quint16 verifyGreeting(quint16 version);
  void unknownOpcode();
  
  AbstractProfile *m_profile;
  bool m_accepted;
  QDataStream m_stream;
  QTcpSocket *m_socket;
  quint16 m_nextBlockSize;
  quint16 m_opcode;
  quint8 m_flag;
};

#endif /*DAEMONSERVICE_H_*/
