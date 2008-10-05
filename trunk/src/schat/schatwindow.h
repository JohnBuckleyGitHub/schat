/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#include "clientservice.h"
#include "settingsdialog.h"

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
class SendWidget;
class Settings;
class Tab;
class UserView;
class WelcomeDialog;

/*!
 * \brief Главное окно чата.
 */
class SChatWindow : public QMainWindow
{
  Q_OBJECT

public:
  SChatWindow(QWidget *parent = 0);

protected:
  void closeEvent(QCloseEvent *event);
  bool event(QEvent *event);

public slots:
  #ifndef DISABLE_SINGLE_APP
    void handleMessage(const QString& message);
  #endif

private slots:
  void about();
  void accessDenied(quint16 reason);
  void accessGranted(const QString &network, const QString &server, quint16 level);
  void addTab(const QString &nick);
  void closeChat();
  void closeTab(int tab = -1);
  void connecting(const QString &server, bool network);
  void copy();
  void fatal();
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void linkLeave(quint8 numeric, const QString &network, const QString &name);
  void message(const QString &sender, const QString &message);
  void messageClicked();
  void newLink(quint8 numeric, const QString &network, const QString &name);
  void newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void newProfile(quint8 gender, const QString &nick, const QString &name);
  void newUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void notice();
  void privateMessage(quint8 flag, const QString &nick, const QString &message);
  void resetTabNotice(int index);
  void sendMsg(const QString &message);
  void serverMessage(const QString &msg);
  void settings();
  void settingsChanged(int notify);
  void unconnected(bool echo = true);
  void userLeave(const QString &nick, const QString &bye, quint8 flag);
  void welcomeOk();

  #ifdef SCHAT_UPDATE
  void update();
  void updateGetDone(int code);
  #endif

private:
  bool eventFilter(QObject *object, QEvent *event);
  bool parseCmd(AbstractTab *tab, const QString &message);
  int tabIndex(const QString &s, int start = 1) const;
  static void cmdHelp(AbstractTab *tab, const QString &cmd);
  void createActions();
  void createCornerWidgets();
  void createService();
  void createToolButtons();
  void createTrayIcon();
  void hideChat();
  void restoreGeometry();
  void saveGeometry();
  void showChat();
  void startNotice(int index);
  void uniqueNick();

  AbstractProfile *m_profile;
  bool m_currentTrayIcon;
  ClientService *m_clientService;
  MainChannel *m_main;
  QAction *m_aboutAction;
  QAction *m_closeTabAction;
  QAction *m_emoticonsSetAction;
  QAction *m_interfaceSetAction;
  QAction *m_networkSetAction;
  QAction *m_profileSetAction;
  QAction *m_quitAction;
  QHBoxLayout *m_toolsLay;
  QLabel *m_statusLabel;
  QMenu *m_trayMenu;
  QPointer<AboutDialog> m_about;
  QPointer<SettingsDialog> m_settingsDialog;
  QSplitter *m_splitter;
  QStatusBar *m_statusBar;
  QSystemTrayIcon *m_tray;
  QTabWidget *m_tabs;
  QTimer *m_noticeTimer;
  QTimer *m_updateTimer;
  QToolButton *m_settingsButton;
  QVBoxLayout *m_mainLay;
  QVBoxLayout *m_rightLay;
  QWidget *m_central;
  QWidget *m_right;
  SendWidget *m_send;
  Settings *m_settings;
  static QMap<QString, QString> m_cmds;
  UserView *m_users;
  WelcomeDialog *m_welcome;

  #ifdef SCHAT_UPDATE
  QAction *updateSetAction;
  QPointer<UpdateNotify> m_updateNotify;
  #endif
};

#endif /*SCHATWINDOW_H_*/
