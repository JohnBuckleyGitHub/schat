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

#ifndef LOGINWIDGET_H_
#define LOGINWIDGET_H_

#include <QWidget>

class QLabel;
class QLineEdit;
class QToolButton;
class QProgressIndicator;
class Notice;

class LoginWidget : public QWidget
{
  Q_OBJECT

public:
  LoginWidget(QWidget *parent = 0);
  bool canLogIn() const;
  void retranslateUi();
  void update();

protected:
  void showEvent(QShowEvent *event);

private slots:
  void login();
  void notice(const Notice &notice);
  void textChanged();

private:
  QLabel *m_nameLabel;
  QLabel *m_passwordLabel;
  QLineEdit *m_nameEdit;
  QLineEdit *m_passwordEdit;
  QProgressIndicator *m_progress; ///< Прогресс бар.
  QToolButton *m_error;
  QToolButton *m_login;
};

#endif /* LOGINWIDGET_H_ */
