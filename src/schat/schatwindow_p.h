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

class AboutDialog;
class ClientService;
class SendWidget;
class SettingsDialog;
class SettingsDialog;
class TrayIcon;
class UserView;
class WelcomeDialog;

/*!
 * \brief Приватный D-класс для класса SChatWindow.
 */
class SChatWindowPrivate
{
public:
  SChatWindowPrivate(SChatWindow *parent);
  ~SChatWindowPrivate();

  bool parseCmd(AbstractTab *tab, const QString &message);
  int tabIndex(const QString &text) const;
  static void cmdHelp(AbstractTab *tab, const QString &cmd);
  void closeChat(bool update = false);
  void createToolButtons();
  void createTrayIcon();
  void displayAway(quint32 status, const QString &nick);
  void hideChat();
  void restoreGeometry();
  void saveGeometry();
  void sendMsg(const QString &msg, bool cmd);
  void showChat();
  void startNotice(int index, const QString &key);
  void uniqueNick();
  #if QT_VERSION < 0x040500
    void createCornerWidgets();
  #endif

  AbstractProfile *profile;
  bool motd;
  bool motdEnable;
  bool sound;
  ClientService *clientService;
  MainChannel *main;
  QAction *aboutAction;
  QAction *daemonAction;
  QAction *emoticonsSetAction;
  QAction *interfaceSetAction;
  QAction *miscSetAction;
  QAction *networkSetAction;
  QAction *profileSetAction;
  QAction *quitAction;
  QAction *soundAction;
  QAction *soundSetAction;
  QAction *updateSetAction;
  QHBoxLayout *toolsLay;
  QLabel *statusLabel;
  QMenu *trayMenu;
  QPointer<AboutDialog> about;
  QPointer<SettingsDialog> settingsDialog;
  QSplitter *splitter;
  QStatusBar *statusBar;
  QTabWidget *tabs;
  QToolButton *settingsButton;
  QToolButton *soundButton;
  QVBoxLayout *mainLay;
  QVBoxLayout *rightLay;
  QWidget *central;
  QWidget *right;
  SChatWindow *q;
  SendWidget *send;
  Settings *pref;
  static QMap<QString, QString> cmds;
  TrayIcon *tray;
  UserView *users;
  WelcomeDialog *welcome;
  #if QT_VERSION < 0x040500
    QAction *closeTabAction;
  #endif
};

#endif /* SCHATWINDOW_P_H_ */
