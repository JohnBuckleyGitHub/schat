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

#ifndef SETTINGSTABHOOK_H_
#define SETTINGSTABHOOK_H_

#include <QWidget>
#include <QIcon>

#include "schat.h"

class ChatSettings;

/*!
 * Базовый класс для страниц настроек.
 */
class SCHAT_CORE_EXPORT SettingsPage : public QWidget
{
  Q_OBJECT

public:
  SettingsPage(const QIcon &icon, const QString &id, QWidget *parent = 0);
  SettingsPage(QWidget *parent = 0);
  inline const QIcon& icon() const   { return m_icon; }
  inline const QString& id() const   { return m_id; }
  inline const QString& name() const { return m_name; }
  virtual void retranslateUi() {}

protected:
  ChatSettings *m_settings; ///< Указатель на объект настроек.
  QIcon m_icon;             ///< Иконка.
  QString m_id;             ///< Идентификатор.
  QString m_name;           ///< Имя страницы настроек.
};


class SCHAT_CORE_EXPORT SettingsTabHook : public QObject
{
  Q_OBJECT

public:
  SettingsTabHook(QObject *parent = 0);
  inline static SettingsTabHook *i() { return m_self; }

private:
  static SettingsTabHook *m_self; ///< Указатель на себя.
};

#endif /* SETTINGSTABHOOK_H_ */
