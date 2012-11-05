/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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


#ifndef CONNECTIONSTATUS_H_
#define CONNECTIONSTATUS_H_

#include <QStatusBar>

class QLabel;
class StatusMenu;
class StatusWidget;

/*!
 * \brief Статус бар для отображения информации о подключении и статусе пользователя.
 */
class ConnectionStatus : public QStatusBar
{
  Q_OBJECT

public:
  /// Состояние подключения.
  enum State {
    UnconnectedState,
    ConnectingState,
    ConnectedState
  };

  ConnectionStatus(StatusMenu *menu, QWidget *parent);
  QString echoText() const;
  void setGender(bool male);
  void setState(State state, const QString &server = "", const QString &network = "");
  void setUserStatus(int status);

protected:
  bool event(QEvent *event);
  void changeEvent(QEvent *event);

private:
  void retranslateUi();
  void updateStyleSheet();

  QLabel *m_icon;             ///< Иконка состояния.
  QLabel *m_movie;            ///< Иконка подключения.
  QLabel *m_status;           ///< Текст состояния.
  QString m_network;          ///< Имя сети.
  QString m_server;           ///< Имя сервера.
  State m_state;              ///< Состояние подключения.
  StatusWidget *m_userStatus; ///< Отображает статус пользователя.
};

#endif /* CONNECTIONSTATUS_H_ */
