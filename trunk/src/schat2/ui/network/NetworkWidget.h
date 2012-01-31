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

#ifndef NETWORKWIDGET_H_
#define NETWORKWIDGET_H_

#include <QWidget>
#include <QPointer>

class LoginWidget;
class NetworkManager;
class Notify;
class Password;
class QComboBox;
class QLabel;
class QMenu;
class QToolBar;
class QToolButton;
class QVBoxLayout;
class SignUpWidget;

class NetworkWidget : public QWidget
{
  Q_OBJECT

public:
  NetworkWidget(QWidget *parent = 0, bool compact = false);
  QAction *connectAction();

public slots:
  void open();

protected:
  bool eventFilter(QObject *watched, QEvent *event);
  void changeEvent(QEvent *event);

private slots:
  int add(const QString &url = QLatin1String("schat://"));
  void edit();
  void indexChanged(int index);
  void notify(const Notify &notify);
  void password();
  void recovery();
  void reload();
  void remove();
  void showAccountMenu();
  void showMenu();
  void signIn();
  void signOut();
  void signUp();
  void signUpDone();

private:
  enum EditState {
    EditNone,
    EditSignIn,
    EditSignUp,
    EditRecovery,
    EditPassword
  };

  void createAccountButton();
  void createActionsButton();
  void load();
  void retranslateUi();
  void setEditState(EditState state);
  void setTitle(const QString &title);
  void updateIndex();

  EditState m_editState;         ///< Состояние виджета.
  NetworkManager *m_manager;     ///< Указатель на менеджер сетевых подключений.
  QAction *m_add;                ///< Действие для добавления сервера.
  QAction *m_connect;            ///< Действие для подключения или отключения от сервера.
  QAction *m_edit;               ///< Действие для редактирования текущего подключения.
  QAction *m_password;           ///< Действие для смены пароля.
  QAction *m_recovery;           ///< Действие для восстановления пароля.
  QAction *m_remove;             ///< Действие для удаления сервера.
  QAction *m_signIn;             ///< Действие для авторизации на сервере.
  QAction *m_signOut;            ///< Действие для выхода из текущей учётной записи.
  QAction *m_signUp;             ///< Действие для регистрации на сервере.
  QByteArray m_editing;          ///< Идентификатор редактируемой сети.
  QComboBox *m_combo;            ///< Комбобокс выбора серверов.
  QLabel *m_title;               ///< Заголовок текущего расширенного действия.
  QMenu *m_menu;                 ///< Меню дополнительных действий.
  QMenu *m_sign;                 ///< Меню действий над аккаунтом.
  QPointer<LoginWidget> m_login; ///< Виджет авторизации.
  QPointer<Password> m_pass;    ///< Виджет для изменения пароля или секретного вопроса.
  QPointer<SignUpWidget> m_reg;  ///< Виджет регистрации.
  QToolBar *m_toolBar;           ///< Тулбар для размещения основных действий.
  QToolButton *m_account;        ///< Кнопка аккаунта.
  QToolButton *m_actions;        ///< Кнопка дополнительных действий.
  QVBoxLayout *m_mainLayout;     ///< Основной компоновщик.
};

#endif /* NETWORKWIDGET_H_ */
