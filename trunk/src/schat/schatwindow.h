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
#include "server.h"
#include "settingsdialog.h"

class AboutDialog;
class MainChannel;
class Profile;
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
  
protected:
  void closeEvent(QCloseEvent *event);
  
public slots:
  void incomingDirectConnection(const QString &n, ServerSocket *socket);
  void newDirectParticipant(quint16 sex, const QStringList &info);
  void newParticipant(quint16 sex, const QStringList &info, bool echo = true);
  void newPrivateMessage(const QString &nick, const QString &message, const QString &sender);
  void participantLeft(const QString &nick);
  void readyForUse();
  void serverChanged();
  
private slots:
  inline void settingsNetwork()   { settingsPage(SettingsDialog::NetworkPage); }
  inline void settingsProfile()   { settingsPage(SettingsDialog::ProfilePage); }
  inline void settingsInterface() { settingsPage(SettingsDialog::InterfacePage); }
  void about();
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
  Profile* profileFromItem(const QStandardItem *item);
  void createActions();
  void createCornerWidgets();
  void createToolButtons();
  void createTrayIcon();
  void readSettings();
  void removeConnection();
  void settingsPage(int page = 0);
  void uniqueNick();
  void writeSettings();
  
  bool firstRun;
  bool hideWelcome;
  ConnectionState state;
  MainChannel *mainChannel;
  Profile *profile;
  QAction *aboutAction;
  QAction *addTabAction;
  QAction *closeTabAction;
  QAction *interfaceSetAction;
  QAction *networkSetAction;
  QAction *profileSetAction;
  QAction *quitAction;
  QAction *sendAction;
  QAction *settingsAction;
  QHBoxLayout *sendLayout;
  QHBoxLayout *toolsLayout;
  QLabel *statusLabel;
  QLineEdit *lineEdit;  
  QListView *listView;
  QMenu *trayIconMenu;
  QPointer<AboutDialog> aboutDialog;
  QPointer<ClientSocket> clientSocket;
  QPointer<Server> daemon;
  QPointer<SettingsDialog> settingsDialog;
  QSplitter *splitter;
  QStandardItemModel model;
  QStatusBar *statusbar;
  QString server;
  QSystemTrayIcon *trayIcon;
  QTabWidget *tabWidget;
  QToolButton *sendButton;
  quint16 serverPort;
  QVBoxLayout *mainLayout;
  QVBoxLayout *rightLayout;
  QWidget *centralWidget;
  QWidget *rightWidget;
  WelcomeDialog *welcomeDialog;
};

#endif /*SCHATWINDOW_H_*/
