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
#include <QSystemTrayIcon>

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
class QMenu;
class MainChannel;

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
  
protected:
  void closeEvent(QCloseEvent *event);
  
public slots:
  void newMessage(const QString &nick, const QString &message);
  void newParticipant(quint16 sex, const QStringList &info, bool echo = true);
  void newPrivateMessage(const QString &nick, const QString &message, const QString &sender);
  void participantLeft(const QString &nick);
  void readyForUse();
  void serverChanged();
  
private slots:
  void addTab();
  void addTab(const QModelIndex &index);
  void closeChat();
  void closeTab();
  void connectionError(QAbstractSocket::SocketError socketError);
  void disconnected();
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void newConnection();
  void returnPressed();
  void welcomeOk();

private:
  int tabIndex(const QString &s, int start = 1);
  QString currentTime();
  void createActions();
  void createTrayIcon();
  void readSettings();
  void removeConnection(ClientSocket *socket);
  void writeSettings();
  
  bool firstRun;
  bool hideWelcome;
  QString server;
  quint16 serverPort;
  ConnectionState state;
  QAction *addTabAction;
  QAction *closeTabAction;
  QAction *quitAction;
  QAction *sendAction;
  QHBoxLayout *sendLayout;
  QLabel *statusLabel;
  QLineEdit *lineEdit;  
  QListView *listView;
  QMenu *trayIconMenu;
  QPointer<ClientSocket> clientSocket;
  QSplitter *splitter;
  QStandardItemModel model;
  QStatusBar *statusbar;
  QString fullName;
  QString nick;
  QSystemTrayIcon *trayIcon;
  QTabWidget *tabWidget;
  QToolButton *sendButton;
  quint8 sex;
  QVBoxLayout *mainLayout;
  QVBoxLayout *rightLayout;
  QWidget *centralWidget;
  QWidget *rightWidget;
  MainChannel *mainChannel;
  WelcomeDialog *welcomeDialog;
};

#endif /*SCHATWINDOW_H_*/
