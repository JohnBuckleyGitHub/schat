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

#ifndef NETWORKTABS_H_
#define NETWORKTABS_H_

#include <QTabWidget>

class LoginWidget;
class SignUpWidget;

class NetworkTabs : public QTabWidget
{
  Q_OBJECT

public:
  NetworkTabs(QWidget *parent = 0);
  bool canLogIn() const;
  bool canSignUp(const QByteArray &id = QByteArray()) const;
  void update();

protected:
  void changeEvent(QEvent *event);

private:
  void retranslateUi();
  void updateSignUp(const QByteArray &id = QByteArray());

  LoginWidget *m_login;   ///< Виджет входа.
  SignUpWidget *m_signup; ///< Виджет регистрации.
};

#endif /* NETWORKTABS_H_ */
