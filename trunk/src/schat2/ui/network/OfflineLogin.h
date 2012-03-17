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

#ifndef OFFLINELOGIN_H_
#define OFFLINELOGIN_H_

#include "ui/network/NetworkExtra.h"

class QCheckBox;
class QLabel;
class QLineEdit;

class OfflineLogin : public NetworkExtra
{
  Q_OBJECT

public:
  OfflineLogin(QWidget *parent = 0);
  bool isAnonymous() const;
  QString name() const;
  QString password() const;

protected:
  void retranslateUi();

private slots:
  void clicked(bool checked);
  void clientStateChanged(int state = -1);
  void reload();

private:
  QCheckBox *m_anonymous;    ///< Чек бокс анонимного подключения.
  QLabel *m_nameLabel;       ///< Пояснительный текст для поля редактирования имени.
  QLabel *m_passwordLabel;   ///< Пояснительный текст для поля редактирования пароля.
  QLineEdit *m_nameEdit;     ///< Поле редактирования имени.
  QLineEdit *m_passwordEdit; ///< Поле редактирования пароля.
};

#endif /* OFFLINELOGIN_H_ */
