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
#include "settingsdialog.h"
#include "clientservice.h"

#ifdef SCHAT_UPDATE
#include "win32/updatenotify.h"
#endif

class AboutDialog;
class AbstractProfile;
class AbstractTab;
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
  SChatWindow(QWidget *parent = 0);
  inline void restoreSplitter(QByteArray state) { splitter->restoreState(state); }
  inline QByteArray saveSplitter() const        { return splitter->saveState(); }
  
protected:
  void closeEvent(QCloseEvent *event);
  bool event(QEvent *event);
  
public slots:
  void handleMessage(const QString& message);
  
private slots:
  inline void settingsInterface() { settingsPage(SettingsDialog::InterfacePage); }
  inline void settingsNetwork()   { settingsPage(SettingsDialog::NetworkPage); }
  inline void settingsProfile()   { settingsPage(SettingsDialog::ProfilePage); }
  inline void settingsUpdate()    { settingsPage(SettingsDialog::UpdatePage); }
  void about();
  void accessGranted(const QString &network, const QString &server, quint16 level);
  void addTab(const QModelIndex &i);
  void changedNick(quint16 sex, const QStringList &list);
  void changedProfile(quint16 sex, const QStringList &list, bool echo = true);
  void closeChat();
  void closeTab();
  void connecting(const QString &server, bool network);
  void errorNickAlreadyUse();
  void fatal();
  void genericMessage(const QString &info);
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void message(const QString &sender, const QString &message);
  void messageClicked();
  void newProfile(quint8 gender, const QString &nick, const QString &name, bool echo);
  void newUser(const QStringList &list, bool echo);
  void notice();
  void privateMessage(quint8 flag, const QString &nick, const QString &message);
  void resetTabNotice(int index);
  void returnPressed();
  void settingsChanged(int notify);
  void unconnected(bool echo = true);
  void userLeave(const QString &nick, const QString &bye, bool echo);
  void welcomeOk();
  
  #ifdef SCHAT_UPDATE  
  void update();
  void updateGetDone(int code);
  #endif
  
private:
  bool parseCmd(AbstractTab *tab, const QString &text);
  int tabIndex(const QString &s, int start = 1) const;
  QStandardItem* findItem(const QString &nick) const;
  static QString userToolTip(const AbstractProfile &profile);
  void createActions();
  void createCornerWidgets();
  void createService();
  void createToolButtons();
  void createTrayIcon();
  void hideChat();
  void settingsPage(int page = 0);
  void showChat();
  void startNotice(int index);
  void uniqueNick();
  
  AbstractProfile *m_profile;
  bool currentTrayIcon;
  ClientService *m_clientService;
  MainChannel *mainChannel;
  QAction *aboutAction;
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
  QPointer<SettingsDialog> settingsDialog;
  QSplitter *splitter;
  QStandardItemModel model;
  QStatusBar *statusbar;
  QSystemTrayIcon *trayIcon;
  QTabWidget *m_tabs;
  QTimer *m_updateTimer;
  QTimer *noticeTimer;
  QToolButton *m_settingsButton;
  QToolButton *sendButton;
  QVBoxLayout *mainLayout;
  QVBoxLayout *rightLayout;
  QWidget *centralWidget;
  QWidget *rightWidget;
  Settings *settings;
  WelcomeDialog *welcomeDialog;
  
  #ifdef SCHAT_UPDATE
  QAction *updateSetAction;
  QPointer<UpdateNotify> m_updateNotify;
  #endif
};

#endif /*SCHATWINDOW_H_*/
