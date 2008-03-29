/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SCHATWINDOW_H_
#define SCHATWINDOW_H_

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTcpSocket>
#include <QPointer>

class QAction;
class QHBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QListView;
class QSplitter;
class QStatusBar;
class QTabWidget;
class QToolButton;
class QVBoxLayout;
class Tab;
class WelcomeDialog;

#include "clientsocket.h"

class SChatWindow : public QMainWindow
{
  Q_OBJECT

public:
  enum ConnectionState {
    Disconnected,
    WaitingForConnected,
    Connected,
    CriticalError
  };
  
  SChatWindow(QWidget *parent = 0);
  void setNick(const QString &n) { nick = n; }
  
public slots:
  void newMessage(const QString &nick, const QString &message);
  void newPrivateMessage(const QString &nick, const QString &message, const QString &sender);
  void newParticipant(quint16 sex, const QStringList &info, bool echo = true);
  void participantLeft(const QString &nick);
  void readyForUse();
  
private slots:
  void addTab();
  void addTab(const QModelIndex &index);
  void closeTab();
  void connectionError(QAbstractSocket::SocketError socketError);
  void disconnected();
  void newConnection();
  void returnPressed();
  void welcomeOk();

private:
  int tabIndex(const QString &s, int start = 1);
  QString currentTime();
  void createActions();
  void removeConnection(ClientSocket *socket);
  
  ConnectionState state;
  QPointer<ClientSocket> clientSocket;
  QAction *addTabAction;
  QAction *closeTabAction;
  QAction *sendAction;
  QHBoxLayout *sendLayout;
  QLabel *statusLabel;
  QLineEdit *lineEdit;  
  QListView *listView;
  QSplitter *splitter;
  QStandardItemModel model;
  QStatusBar *statusbar;
  QString fullName;
  QString nick;
  QTabWidget *tabWidget;
  QToolButton *sendButton;
  quint8 sex;
  QVBoxLayout *mainLayout;
  QVBoxLayout *rightLayout;
  QWidget *centralWidget;
  QWidget *rightWidget;
  Tab *currentChannel;
  Tab *mainChannel;
  WelcomeDialog *welcomeDialog;
};

#endif /*SCHATWINDOW_H_*/
