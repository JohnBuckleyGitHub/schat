/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SETTINGSDIALOG_H_
#define SETTINGSDIALOG_H_

#include <QPushButton>

#include "abstractsettingsdialog.h"

class AbstractProfile;
class EmoticonsSettings;
class InterfaceSettings;
class NetworkSettings;
class NetworkWidget;
class ProfileSettings;
class ProfileWidget;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QSpinBox;
class Settings;
class UpdateSettings;


/*!
 * \class SettingsDialog
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
    UpdatePage
  };

  SettingsDialog(AbstractProfile *profile, QWidget *parent = 0);

public slots:
  void accept();

private:
  EmoticonsSettings *m_emoticonsPage;
  InterfaceSettings *m_interfacePage;
  NetworkSettings *m_networkPage;
  ProfileSettings *m_profilePage;
  Settings *m_settings;
  UpdateSettings *m_updatePage;
};


/*!
 * \class ProfileSettings
 * \brief Диалог "Настройка", страница "Личные данные".
 */
class ProfileSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  ProfileSettings(AbstractProfile *profile, QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

signals:
  void validNick(bool b);

private:
  AbstractProfile *m_profile;
  ProfileWidget *m_profileWidget;
  QLineEdit *m_byeMsgEdit;
  Settings *m_settings;
};


/*!
 * \class NetworkSettings
 * \brief Диалог "Настройка", страница "Сеть".
 */
class NetworkSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  NetworkSettings(QWidget *parent = 0);

signals:
  void validServer(bool valid);

public slots:
  void reset(int page);
  void save();

private:
  NetworkWidget *m_networkWidget;
  QCheckBox *m_welcomeCheckBox;
  Settings *m_settings;
};


/*!
 * \class InterfaceSettings
 * \brief Диалог "Настройка", страница "Интерфейс".
 */
class InterfaceSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  InterfaceSettings(QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private:
  QComboBox *m_styleComboBox;
  Settings *m_settings;
};


/*!
 * \class EmoticonsSettings
 * \brief Диалог "Настройка", страница "Смайлики".
 */
class EmoticonsSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  EmoticonsSettings(QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private slots:
  void enable(bool checked);

private:
  bool createThemeList();

  QCheckBox *m_animateCheck;
  QCheckBox *m_enableCheck;
  QCheckBox *m_requireSpacesCheck;
  QComboBox *m_themeCombo;
  QGroupBox *m_themeGroup;
  Settings *m_settings;
};


/*!
 * \class UpdateSettings
 * \brief Диалог "Настройка", страница "Обновления".
 */
class UpdateSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  UpdateSettings(QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private slots:
  void factorChanged(int index);
  void intervalChanged(int i);

private:
  int m_last;
  QCheckBox *m_autoClean;
  QCheckBox *m_autoDownload;
  QCheckBox *m_checkOnStartup;
  QComboBox *m_factor;
  QGroupBox *m_versionGroup;
  QSpinBox *m_interval;
  Settings *m_settings;
};

#endif /*SETTINGSDIALOG_H_*/
