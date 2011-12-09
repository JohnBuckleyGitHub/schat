/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 - 2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QPushButton>

#include "abstractsettingsdialog.h"

class AbstractProfile;
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
class SoundWidget;


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
    UpdatePage,
    MiscPage
  };

  SettingsDialog(AbstractProfile *profile, QWidget *parent = 0);

public slots:
  void accept();

private:
  Settings *m_settings;
};


/*!
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

private slots:
  void openFolder();

private:
  NetworkWidget *m_networkWidget;
  QCheckBox *m_welcomeCheckBox;
  Settings *m_settings;
};


/*!
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
  void openFolder();

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
 * \brief Диалог "Настройка", страница "Звуки".
 */
class SoundSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  SoundSettings(QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private slots:
  void openFolder();
  void play(const QString &file);

private:
  QGroupBox *m_enable;
  Settings *m_settings;
  SoundWidget *m_message;
  SoundWidget *m_private;
  #ifdef Q_WS_X11
    QGroupBox *m_useCmd;
    QLineEdit *m_cmd;
  #endif
};


/*!
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
  QCheckBox *m_checkOnStartup;
  QComboBox *m_factor;
  QGroupBox *m_versionGroup;
  QSpinBox *m_interval;
  Settings *m_settings;
  #ifndef SCHAT_NO_UPDATE
    QCheckBox *m_autoClean;
    QCheckBox *m_autoDownload;
  #endif
};


/*!
 * \brief Диалог "Настройка", страница "Разное".
 */
class MiscSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  MiscSettings(QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private:
  #ifdef Q_WS_WIN
    void autostart();
    void writeAutostart();
  #endif

  QCheckBox *m_log;
  QCheckBox *m_logPrivate;
  Settings *m_settings;
  #ifdef Q_WS_WIN
    QCheckBox *m_autostart;
    QCheckBox *m_autostartDaemon;
  #endif
};

#endif /*SETTINGSDIALOG_H_*/