/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SCHATWINDOW_P_H_
#define SCHATWINDOW_P_H_

#include <QPointer>
#include <QPair>

#if defined(Q_OS_WINCE) && !defined(SCHAT_WINCE)
  #define SCHAT_WINCE
#endif

#include "idle/idle.h"

class AboutDialog;
class ClientService;
class PopupManager;
class SendWidget;
class SettingsDialog;
class SettingsDialog;
class TrayIcon;
class UserView;
class WelcomeDialog;
class SoundAction;
class Settings;

/*!
 * \brief Приватный D-класс для класса SChatWindow.
 */
class SChatWindowPrivate
{
public:
  enum Status {
    StatusOnline,
    StatusAway,
    StatusDnD,
    StatusOffline
  };

  SChatWindowPrivate(SChatWindow *parent);
  ~SChatWindowPrivate();

  bool parseCmd(AbstractTab *tab, const QString &message);
  bool sendStatus(quint32 status);
  bool startNotice(int index, const QString &key);
  QPair<int, AbstractTab *> createPrivateTab(const QString &nick);
  QPair<int, AbstractTab *> tabFromName(const QString &text, AbstractTab::Type type = AbstractTab::Private) const;
  QPair<int, AbstractTab *> updatePrivateTab(const AbstractProfile &prof);
  QString channel();
  QString colorizedPing() const;
  static void cmdHelp(AbstractTab *tab, const QString &cmd);
  void closeChat(bool update = false);
  void createStatusBar();
  void createTrayIcon();
  void displayStatus(quint32 status, const QString &nick);
  void hideChat();
  void msgToAllPrivateTabs(const QString &msg);
  void restoreGeometry();
  void saveGeometry();
  void sendMsg(const QString &msg, bool cmd);
  void setAwayOptions();
  void showChat();
  void statusAccessGranted(const QString &network, const QString &server);
  void statusConnecting(const QString &server, bool network);
  void statusUnconnected(bool echo);
  void uniqueNick();
  void universalStatus(const QList<quint32> &data1, const QStringList &data2);
  void updateStatus(int status);

  #ifndef Q_OS_WINCE
  void createToolButtons();
  #endif

  AbstractProfile *profile;
  bool autoAway;
  bool enableMotd;
  bool exitAwayOnSend;
  bool motd;
  ClientService *clientService;
  Idle idleDetector;
  int autoAwayTime;
  MainChannel *main;
  PopupManager *popupManager;
  QAction *awayAction;
  QAction *daemonAction;
  QAction *dndAction;
  QAction *offlineAction;
  QAction *onlineAction;
  QAction *settingsAction;
  QAction *statusAction;
  QLabel *connectLabel;
  QLabel *connectMovie;
  QLabel *statusLabel;
  QPointer<AboutDialog> about;
  QPointer<SettingsDialog> settingsDialog;
  QStatusBar *statusBar;
  QTabWidget *tabs;
  QTime pingTime;
  QVBoxLayout *mainLay;
  QWidget *central;
  SChatWindow *q;
  SendWidget *send;
  Settings *pref;
  SoundAction *soundAction;
  static QMap<QString, QString> cmds;
  TrayIcon *tray;
  UserView *users;
  #ifndef SCHAT_WINCE
    QComboBox *statusCombo;
    QToolBar *toolBar;
  #endif
};

#endif /* SCHATWINDOW_P_H_ */
