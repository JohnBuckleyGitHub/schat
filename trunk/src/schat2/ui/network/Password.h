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

#ifndef PASSWORD_H_
#define PASSWORD_H_

#include <QWidget>

class PasswordWidget;
class QLabel;
class QLineEdit;
class QRadioButton;
class QToolButton;
class QuestionWidget;
class SecurityQuestion;

class Password : public QWidget
{
  Q_OBJECT

public:
  Password(QWidget *parent = 0);

private:
  enum Mode {
    PasswordMode,
    QuestionMode
  };

  void setMode(Mode mode);

  Mode m_mode;
  PasswordWidget *m_passwordWidget;
  QRadioButton *m_password;
  QRadioButton *m_question;
  QuestionWidget *m_questionWidget;
};


class PasswordWidget : public QWidget
{
  Q_OBJECT

public:
  PasswordWidget(QWidget *parent = 0);

private:
  QLabel *m_newLabel;
  QLabel *m_passwordLabel;
  QLineEdit *m_newEdit;
  QLineEdit *m_passwordEdit;
  QToolButton *m_ok;
};


class QuestionWidget : public QWidget
{
  Q_OBJECT

public:
  QuestionWidget(QWidget *parent = 0);

private:
  QLabel *m_answerLabel;
  QLabel *m_passwordLabel;
  QLabel *m_questionLabel;
  QLineEdit *m_answerEdit;
  QLineEdit *m_passwordEdit;
  SecurityQuestion *m_question;
  QToolButton *m_ok;
};

#endif /* PASSWORD_H_ */
