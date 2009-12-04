/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef SETTINGSDIALOG_H_
#define SETTINGSDIALOG_H_

#include "abstractsettingsdialog.h"

/*!
 * \brief Диалог "Настройка".
 */
class SettingsDialog : public AbstractSettingsDialog
{
  Q_OBJECT

public:
  enum Page {
    ProfilePage,
    NetworkPage,
    InterfacePage,
    EmoticonsPage,
    SoundPage,
    NotificationPage,
    UpdatePage,
    MiscPage
  };

  SettingsDialog(QWidget *parent = 0);

public slots:
  void accept();
};


/*!
 * \brief Диалог "Настройка", страница "Личные данные".
 */
class ProfileSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  ProfileSettings(QWidget *parent = 0);
  ~ProfileSettings();

public slots:
  void reset(int page);
  void save();

signals:
  void validNick(bool b);

private:
  class Private;
  Private * const d;
};


/*!
 * \brief Диалог "Настройка", страница "Сеть".
 */
class NetworkSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  NetworkSettings(QWidget *parent = 0);
  ~NetworkSettings();

signals:
  void validServer(bool valid);

public slots:
  void reset(int page);
  void save();

private slots:
  void openFolder();

private:
  class Private;
  Private * const d;
};


/*!
 * \brief Диалог "Настройка", страница "Интерфейс".
 */
class InterfaceSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  InterfaceSettings(QWidget *parent = 0);
  ~InterfaceSettings();

public slots:
  void reset(int page);
  void save();

private slots:
  #ifndef SCHAT_NO_WEBKIT
    void reloadVariants(int index);
  #endif

private:
  class Private;
  Private * const d;
};


/*!
 * \brief Диалог "Настройка", страница "Смайлики".
 */
class EmoticonsSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  EmoticonsSettings(QWidget *parent = 0);
  ~EmoticonsSettings();

public slots:
  void reset(int page);
  void save();

private slots:
  void enable(bool checked);
  void openFolder();

private:
  class Private;
  Private * const d;
};


/*!
 * \brief Диалог "Настройка", страница "Звуки".
 */
class SoundSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  SoundSettings(QWidget *parent = 0);
  ~SoundSettings();

public slots:
  void reset(int page);
  void save();

private slots:
  void openFolder();
  void play(const QString &file);

private:
  class Private;
  Private * const d;
};


/*!
 * \brief Диалог "Оповещатель", страница "Звуки".
 */
class NotificationSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  NotificationSettings(QWidget *parent = 0);
  ~NotificationSettings();

public slots:
  void reset(int page);
  void save();

private slots:
  void popupGroupState();

private:
  class Private;
  Private * const d;
};


/*!
 * \brief Диалог "Настройка", страница "Обновления".
 */
class UpdateSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  UpdateSettings(QWidget *parent = 0);
  ~UpdateSettings();

public slots:
  void reset(int page);
  void save();

private slots:
  void factorChanged(int index);
  void intervalChanged(int i);

private:
  class Private;
  Private * const d;
};


/*!
 * \brief Диалог "Настройка", страница "Разное".
 */
class MiscSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  MiscSettings(QWidget *parent = 0);
  ~MiscSettings();

public slots:
  void reset(int page);
  void save();

private:
  class Private;
  Private * const d;
};

#endif /*SETTINGSDIALOG_H_*/
