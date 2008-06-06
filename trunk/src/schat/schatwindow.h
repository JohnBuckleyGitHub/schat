/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SCHATWINDOW_H_
#define SCHATWINDOW_H_

#include <QMainWindow>
#include <QPointer>
#include <QSplitter>
#include <QStandardItemModel>
#include <QSystemTrayIcon>
#include <QTcpSocket>
#include <QTime>

#include "clientsocket.h"
#include "server.h"
#include "settingsdialog.h"
#include "updatenotify.h"

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
class QStandardItem;
class QStatusBar;
class QTabWidget;
class QTimer;
class QToolButton;
class QVBoxLayout;
class Settings;
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
    Stopped,
    Ignore
  };
  
  SChatWindow(QWidget *parent = 0);
  inline void restoreSplitter(QByteArray state) { splitter->restoreState(state); }
  inline QByteArray saveSplitter()              { return splitter->saveState(); }
  void reconnect();
  
protected:
  void closeEvent(QCloseEvent *event);
  bool event(QEvent *event);
  
public slots:
  void incomingDirectConnection(const QString &n, ServerSocket *socket);
  void newDirectMessage();
  void newDirectParticipant(quint16 sex, const QStringList &info);
  void newMessage(const QString &nick, const QString &message);
  void newParticipant(quint16 sex, const QStringList &info, bool echo = true);
  void newPrivateMessage(const QString &nick, const QString &message, const QString &sender);
  void participantLeft(const QString &nick);
  void readyForUse();
  
private slots:
  inline void settingsInterface() { settingsPage(SettingsDialog::InterfacePage); }
  inline void settingsNetwork()   { settingsPage(SettingsDialog::NetworkPage); }
  inline void settingsProfile()   { settingsPage(SettingsDialog::ProfilePage); }
  void about();
  void addTab();
  void addTab(const QModelIndex &i);
  void changedNick(quint16 sex, const QStringList &list);
  void changedProfile(quint16 sex, const QStringList &list, bool echo = true);
  void closeChat();
  void closeTab();
  void connectionError(QAbstractSocket::SocketError socketError);
  void disconnected();
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void messageClicked();
  void networkSettingsChanged();
  void newConnection();
  void notice();
  void profileSettingsChanged();
  void resetTabNotice(int index);
  void returnPressed();
  void update();
  void updateGetDone(int code);
  void welcomeOk();

private:
  int tabIndex(const QString &s, int start = 1) const;
  Profile* profileFromItem(const QStandardItem *item);
  QStandardItem* findItem(const QString &nick) const;
  void createActions();
  void createCornerWidgets();
  void createToolButtons();
  void createTrayIcon();
  void removeConnection();
  void settingsPage(int page = 0);
  void startNotice(int index);
  void uniqueNick();
  
  bool currentTrayIcon;
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
  QPointer<UpdateNotify> m_updateNotify;
  QSplitter *splitter;
  QStandardItemModel model;
  QStatusBar *statusbar;
  QSystemTrayIcon *trayIcon;
  QTabWidget *tabWidget;
  QTimer *m_reconnectTimer;
  QTimer *m_updateTimer;
  QTimer *noticeTimer;
  QToolButton *sendButton;
  QVBoxLayout *mainLayout;
  QVBoxLayout *rightLayout;
  QWidget *centralWidget;
  QWidget *rightWidget;
  Settings *settings;
  WelcomeDialog *welcomeDialog;
};

#endif /*SCHATWINDOW_H_*/
