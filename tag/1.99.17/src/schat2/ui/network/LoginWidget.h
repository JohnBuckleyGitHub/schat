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

#ifndef LOGINWIDGET_H_
#define LOGINWIDGET_H_

#include "ui/network/NetworkExtra.h"

class Notify;
class QLabel;
class QLineEdit;
class NetworkButton;

/*!
 * Виджет авторизации.
 */
class LoginWidget : public NetworkExtra
{
  Q_OBJECT

public:
  LoginWidget(QWidget *parent = 0);
  QString title() const;

protected:
  void focusInEvent(QFocusEvent *event);
  void retranslateUi();

private slots:
  void login();
  void notify(const Notify &notify);
  void reload();

private:
  bool isReady() const;

  QLabel *m_nameLabel;       ///< Пояснительный текст для поля редактирования имени.
  QLabel *m_passwordLabel;   ///< Пояснительный текст для поля редактирования пароля.
  QLineEdit *m_nameEdit;     ///< Поле редактирования имени.
  QLineEdit *m_passwordEdit; ///< Поле редактирования пароля.
  NetworkButton *m_login;    ///< Кнопка немедленного входа.
};

#endif /* LOGINWIDGET_H_ */
