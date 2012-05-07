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

#include "ui/network/NetworkExtra.h"

class NetworkButton;
class Notify;
class PasswordWidget;
class QLabel;
class QLineEdit;
class QRadioButton;
class QToolButton;
class QuestionWidget;
class SecurityQuestion;

/*!
 * Виджет изменения пароля или секретного вопроса.
 */
class Password : public NetworkExtra
{
  Q_OBJECT

public:
  Password(QWidget *parent = 0);

protected:
  void retranslateUi();

private slots:
  void toggled();

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


/*!
 * Базовый виджет для отдельных виждетов смены пароля или секретного вопроса.
 */
class PasswordBase : public QWidget
{
  Q_OBJECT

public:
  PasswordBase(QWidget *parent = 0);
  virtual void retranslateUi();

signals:
  void done();

public slots:
  void reload();

private slots:
  void notify(const Notify &notify);

protected:
  virtual bool isReady() const;

  QLabel *m_passwordLabel;
  QLineEdit *m_passwordEdit;
  NetworkButton *m_ok;
};


/*!
 * Виджет для смены пароля.
 */
class PasswordWidget : public PasswordBase
{
  Q_OBJECT

public:
  PasswordWidget(QWidget *parent = 0);
  void retranslateUi();

protected:
  bool isReady() const;

private slots:
  void execute();

private:
  QLabel *m_newLabel;
  QLineEdit *m_newEdit;
};


/*!
 * Виджет для смены секретного вопроса.
 */
class QuestionWidget : public PasswordBase
{
  Q_OBJECT

public:
  QuestionWidget(QWidget *parent = 0);
  void retranslateUi();

protected:
  bool isReady() const;

private slots:
  void execute();

private:
  QLabel *m_answerLabel;
  QLabel *m_questionLabel;
  QLineEdit *m_answerEdit;
  SecurityQuestion *m_question;
};

#endif /* PASSWORD_H_ */
