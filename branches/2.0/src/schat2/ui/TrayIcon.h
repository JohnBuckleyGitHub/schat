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

#ifndef TRAYICON_H_
#define TRAYICON_H_

#include <QSystemTrayIcon>

class QAction;
class QBasicTimer;
class SimpleClient;

class TrayIcon : public QSystemTrayIcon
{
  Q_OBJECT

public:
  TrayIcon(QObject *parent = 0);
  ~TrayIcon();
  inline QAction *aboutAction() { return m_aboutAction; }
  inline QAction *quitAction() { return m_quitAction; }
  inline QAction *settingsAction() { return m_settingsAction; }
  void alert(bool start = true);
  void retranslateUi();

protected:
  void timerEvent(QTimerEvent *event);

private slots:
  void about();
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void quit();
  void settings();
  void update();

private:
  int m_alertIcon;           ///< Номер текущей иконки, при отображении уведомлений.
  QAction *m_aboutAction;    ///< О Simple Chat.
  QAction *m_quitAction;     ///< Quit.
  QAction *m_settingsAction; ///< Settings.
  QBasicTimer *m_timer;      ///< Таймер анимации.
  QIcon m_icon;              ///< Иконка.
  QMenu *m_menu;             ///< Меню.
  SimpleClient *m_client;    ///< Указатель на клиент.
};

#endif /* TRAYICON_H_ */
