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

#ifndef SETTINGSTAB_P_H_
#define SETTINGSTAB_P_H_

#include <QWidget>
#include <QIcon>

class ChatSettings;
class GenderField;
class LanguageField;
class NetworkWidget;
class NickEdit;
class ProfileLayout;
class QCheckBox;
class QLabel;

/*!
 * Базовый класс для страниц настроек.
 */
class AbstractSettingsPage : public QWidget
{
  Q_OBJECT

public:
  AbstractSettingsPage(const QIcon &icon, const QString &id, QWidget *parent = 0);
  AbstractSettingsPage(QWidget *parent = 0);
  inline QIcon icon() const { return m_icon; }
  inline QString id() const { return m_id; }
  inline QString name() const { return m_name; }
  virtual void retranslateUi() {}

protected:
  ChatSettings *m_settings; ///< Указатель на объект настроек.
  QIcon m_icon;             ///< Иконка.
  QString m_id;             ///< Идентификатор.
  QString m_name;           ///< Имя страницы настроек.
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
  void retranslateUi();

  GenderField *m_genderField;
  NickEdit *m_nickEdit;
  ProfileLayout *m_layout;    ///< Виджет для размещения полей профиля.
  QLabel *m_genderLabel;
  QLabel *m_nickLabel;
  QLabel *m_profileLabel;
};


/*!
 * Страница настройки профиля.
 */
class NetworkPage : public AbstractSettingsPage
{
  Q_OBJECT

public:
  NetworkPage(QWidget *parent = 0);

private:
  void retranslateUi();

  NetworkWidget *m_network;
  QLabel *m_networkLabel;
};


/*!
 * Страница настройки профиля.
 */
class LocalePage : public AbstractSettingsPage
{
  Q_OBJECT

public:
  LocalePage(QWidget *parent = 0);

private:
  void retranslateUi();

  LanguageField *m_language; ///< Виджет выбора языка.
  QLabel *m_localeLabel;
};


#endif /* SETTINGSTAB_P_H_ */
