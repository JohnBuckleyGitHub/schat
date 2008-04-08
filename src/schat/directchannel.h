/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef DIRECTCHANNEL_H_
#define DIRECTCHANNEL_H_

#include <QLineEdit>
#include <QPointer>
#include <QTextBrowser>
#include <QTime>
#include <QWidget>

#include "clientsocket.h"
#include "profile.h"

class QAction;
class QHBoxLayout;
class QLabel;
class QToolButton;
class QVBoxLayout;
class SChatWindow;

class DirectChannel : public QWidget {
  Q_OBJECT

public:
  enum ConnectionState {
    Disconnected,
    WaitingForConnected,
    Connected,
    Stopped
  };
  
  DirectChannel(Profile *p, QWidget *parent = 0);
  void append(const QString &message);
  void sendText(const QString &text);
  
signals:
  void newDirectParticipant(quint16 sex, const QStringList &info);

public slots:
  void connectionError(QAbstractSocket::SocketError socketError);
  void disconnected();
  void newParticipant(quint16 sex, const QStringList &info, bool echo = true);
  void newPrivateMessage(const QString &nick, const QString &message, const QString &sender);
  void readyForUse();
  
private slots:
  void newConnection();
  
private:
  QString currentTime() { return QTime::currentTime().toString("hh:mm:ss"); }
  void createActions();
  void removeConnection();
  void scroll();
  
  Profile *profile;
  ConnectionState state;
  QAction *connectCreateAction;
  QHBoxLayout *topLayout;
  QLabel *adrLabel;
  QLineEdit *remoteEdit;
  QPointer<ClientSocket> clientSocket;
  QString remoteNick;
  QTextBrowser chatText;
  QToolButton *connectCreateButton;
  QVBoxLayout *mainLayout;
  SChatWindow *chat;
};

#endif /*DIRECTCHANNEL_H_*/
