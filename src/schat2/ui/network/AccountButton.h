/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#ifndef ACCOUNTBUTTON_H_
#define ACCOUNTBUTTON_H_

#include <QToolButton>

class NetworkWidget;

class AccountButton : public QToolButton
{
  Q_OBJECT

public:
  AccountButton(NetworkWidget *parent = 0);

protected:
  void changeEvent(QEvent *event);

private slots:
  void menuTriggered(QAction *action);
  void showMenu();

private:
  void retranslateUi();

  NetworkWidget *m_network;      ///< Виджет редактирования сети.
  QAction *m_computers;          ///< Действие для просмотра компьютеров.
//  QAction *m_signIn;             ///< Действие для авторизации на сервере.
  QAction *m_signOut;            ///< Действие для выхода из текущей учётной записи.
//  QAction *m_signUp;             ///< Действие для регистрации на сервере.
  QMenu *m_menu;                 ///< Меню действий над аккаунтом.
};

#endif /* ACCOUNTBUTTON_H_ */
