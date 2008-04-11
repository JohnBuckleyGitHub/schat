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

#include "chatbrowser.h"
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
  inline void append(const QString &message) { chatBrowser->add(message); }
  void displayChoiceServer(bool display);
  void sendText(const QString &text);
  
signals:
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
  
  ChatBrowser *chatBrowser;
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
