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

#ifndef DIRECTCHANNEL_H_
#define DIRECTCHANNEL_H_

#include <QLineEdit>
#include <QPointer>
#include <QTime>
#include <QWidget>

#include "abstracttab.h"
#include "clientsocket.h"
#include "profile.h"

class QAction;
class QHBoxLayout;
class QLabel;
class QToolButton;
class QVBoxLayout;
class SChatWindow;

class DirectChannel : public AbstractTab {
  Q_OBJECT

public:
  enum ConnectionState {
    Disconnected,
    WaitingForConnected,
    Connected,
    Stopped
  };
  
  DirectChannel(Profile *p, QWidget *parent = 0);
  void displayChoiceServer(bool display);
  void sendText(const QString &text);
  
signals:
  void newDirectMessage();
  void newDirectParticipant(quint16 sex, const QStringList &info);

public slots:
  void newParticipant(quint16 sex, const QStringList &info, bool echo = true);
  void newPrivateMessage(const QString &nick, const QString &message, const QString &sender);
  void readyForUse();
  
private slots:
  void newConnection();
  void removeConnection();
  
private:
  void createActions();
  
  ConnectionState state;
  Profile *profile;
  QAction *connectCreateAction;
  QHBoxLayout *topLayout;
  QLabel *adrLabel;
  QLineEdit *remoteEdit;
  QPointer<ClientSocket> clientSocket;
  QString remoteNick;
  QToolButton *connectCreateButton;
  QVBoxLayout *mainLayout;
  SChatWindow *chat;
};

#endif /*DIRECTCHANNEL_H_*/
