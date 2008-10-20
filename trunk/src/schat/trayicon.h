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

#ifndef TRAYICON_H_
#define TRAYICON_H_

#include <QSystemTrayIcon>

class QTimer;
class Settings;

/*!
 * \brief Иконка в системном трее.
 */
class TrayIcon : public QSystemTrayIcon {
  Q_OBJECT

public:
  enum Message {
    UpdateReady,
    UpdateAvailable
  };

  TrayIcon(QObject *parent = 0);
  inline Message message() const { return m_message; }
  static QString bytesToHuman(int size);
  TrayIcon(const QIcon &icon, QObject *parent = 0);
  void notice(bool enable);

private slots:
  void messageClicked();
  void notify(int code);
  void timeout();

private:
  void init();
  void updateAvailable();
  void updateReady();

  bool m_normal;
  Message m_message;
  QIcon m_icon;
  QIcon m_noticeIcon;
  QTimer *m_timer;
  Settings *m_settings;
};

#endif /* TRAYICON_H_ */
