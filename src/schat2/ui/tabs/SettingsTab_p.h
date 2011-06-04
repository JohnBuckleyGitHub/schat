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

#ifndef SETTINGSTAB_P_H_
#define SETTINGSTAB_P_H_

#include <QWidget>

class NetworkWidget;
class NickEdit;
class QCheckBox;
class QLabel;
class ChatSettings;

/*!
 * Базовый класс для страниц настроек.
 */
class AbstractSettingsPage : public QWidget
{
  Q_OBJECT

public:
  AbstractSettingsPage(QWidget *parent = 0);

protected:
  virtual void retranslateUi() {}
  void changeEvent(QEvent *event);

  ChatSettings *m_settings;
};


/*!
 * Страница настройки профиля.
 */
class ProfilePage : public AbstractSettingsPage
{
  Q_OBJECT

public:
  ProfilePage(QWidget *parent = 0);

private:
  NetworkWidget *m_networks;
  NickEdit *m_nickEdit;
  QCheckBox *m_defaultProfile;
  QLabel *m_networkLabel;
  QLabel *m_nickLabel;
  QLabel *m_profileLabel;
};


#endif /* SETTINGSTAB_P_H_ */
