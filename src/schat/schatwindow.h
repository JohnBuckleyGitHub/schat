/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SCHATWINDOW_H_
#define SCHATWINDOW_H_

#include <QMainWindow>
#include <QPointer>
#include <QStandardItemModel>
#include <QSystemTrayIcon>
#include <QTcpSocket>
#include <QTime>

#include "clientsocket.h"

class MainChannel;
class QAction;
class QHBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QListView;
class QMenu;
class QSplitter;
class QStatusBar;
class QTabWidget;
class QToolButton;
class QVBoxLayout;
class Tab;
class WelcomeDialog;

class SChatWindow : public QMainWindow
{
  Q_OBJECT

public:
  enum ConnectionState {
    Disconnected,
    WaitingForConnected,
    Connected,
    Stopped
  };
  
  SChatWindow(QWidget *parent = 0);
  QString getNick() { return nick; }
  QString getFullName() { return fullName; }
  quint8 getSex() { return sex; }
  
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
  QString currentTime() const { return QTime::currentTime().toString("hh:mm:ss"); }
  void createActions();
  void createTrayIcon();
  void readSettings();
  void removeConnection();
  void uniqueNick();
  void writeSettings();
  
  bool firstRun;
  bool hideWelcome;
  ConnectionState state;
  MainChannel *mainChannel;
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
  QString server;
  QSystemTrayIcon *trayIcon;
  QTabWidget *tabWidget;
  QToolButton *sendButton;
  quint16 serverPort;
  quint8 sex;
  QVBoxLayout *mainLayout;
  QVBoxLayout *rightLayout;
  QWidget *centralWidget;
  QWidget *rightWidget;
  WelcomeDialog *welcomeDialog;
};

#endif /*SCHATWINDOW_H_*/
