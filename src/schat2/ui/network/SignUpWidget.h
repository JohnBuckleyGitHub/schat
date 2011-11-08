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

#ifndef SIGNUPWIDGET_H_
#define SIGNUPWIDGET_H_

#include <QWidget>

class NetworkManager;
class QComboBox;
class QLabel;
class QLineEdit;
class QProgressIndicator;
class QPushButton;
class SimpleClient;

class SignUpWidget : public QWidget
{
  Q_OBJECT

public:
  /// Состояние виджета.
  enum WidgetState {
    Idle,    ///< Ожидание действий пользователя.
    Progress ///< Запущен процесс регистрации.
  };

  SignUpWidget(QWidget *parent = 0);
  ~SignUpWidget();
  bool ready() const;
  static bool canSignUp();
  void retranslateUi();
  void setSmall(bool small = true);

public slots:
  void reload();

private slots:
  void signUp();

private:
  void setState(WidgetState state);

  NetworkManager *m_manager;      ///< Указатель на менеджер сетевых подключений.
  QComboBox *m_question;          ///< Секретный вопрос.
  QLabel *m_answerLabel;
  QLabel *m_nameLabel;            ///< Пояснительный текст для поля редктирования имени.
  QLabel *m_passwordLabel;        ///< Пояснительный текст для поля редктирования пароля.
  QLabel *m_questionLabel;
  QLineEdit *m_answerEdit;        ///< Поле редактирования секретного вопроса.
  QLineEdit *m_nameEdit;          ///< Поле редактирования имени.
  QLineEdit *m_passwordEdit;      ///< Поле редактирования пароля.
  QProgressIndicator *m_progress; ///< Прогресс бар.
  QPushButton *m_signUp;
  SimpleClient *m_client;         ///< Указатель на клиент.
  WidgetState m_state;
};

#endif /* SIGNUPWIDGET_H_ */
