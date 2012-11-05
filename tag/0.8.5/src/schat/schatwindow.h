/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#ifndef SCHATWINDOW_H_
#define SCHATWINDOW_H_

#include <QMainWindow>
#include <QSystemTrayIcon>

#if defined(Q_OS_WINCE) && !defined(SCHAT_WINCE)
  #define SCHAT_WINCE
#endif

class SChatWindowPrivate;

/*!
 * \brief Главное окно чата.
 */
class SChatWindow : public QMainWindow
{
  Q_OBJECT

public:
  SChatWindow(QWidget *parent = 0);
  ~SChatWindow();
  bool isActiveChatWindow();
  void showChat();

protected:
  bool event(QEvent *event);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void showEvent(QShowEvent *event);
  #if defined(Q_WS_WIN)
  bool winEvent(MSG *message, long *result);
  #endif

private slots:
  void about();
  void accessDenied(quint16 reason);
  void accessGranted(const QString &network, const QString &server, quint16 level);
  void addTab(const QString &nick);
  void closeChat(bool update = false);
  void closeTab(int tab = -1);
  void connecting(const QString &server, bool network);
  void copy();
  void daemonUi();
  void fatal();
  void handleMessage(const QString& message);
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void linkLeave(quint8 numeric, const QString &network, const QString &name);
  void message(const QString &sender, const QString &msg);
  void newLink(quint8 numeric, const QString &network, const QString &name);
  void newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void newProfile(quint8 gender, const QString &nick, const QString &name);
  void newUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void onSecondsIdle(int seconds);
  void openChat(const QString &nick, bool pub, bool open);
  void privateMessage(quint8 flag, const QString &nick, const QString &msg);
  void sendMsg(const QString &msg);
  void serverMessage(const QString &msg);
  void settingsChanged(int notify);
  void showSettingsPage(int page = 0);
  void statusChangedByUser();
  void statusChangedByUser(int index);
  void stopNotice(int index);
  void syncUsersEnd();
  void unconnected(bool echo = true);
  void universal(quint16 sub, const QList<quint32> &data1, const QStringList &data2);
  void userLeave(const QString &nick, const QString &bye, quint8 echo);

  #ifndef SCHAT_NO_UPDATE
    void messageClicked();
  #endif

  #ifdef SCHAT_BENCHMARK
    void benchmark();
  #endif

private:
  bool eventFilter(QObject *object, QEvent *event);
  void createActions();
  void createMenu();
  void createService();
  void retranslateUi();

  SChatWindowPrivate * const d;
};

#endif /* SCHATWINDOW_H_ */
