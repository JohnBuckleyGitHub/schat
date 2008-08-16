/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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

#include <QDialog>
#include <QPushButton>

class InterfaceSettings;
class NetworkSettings;
class NetworkWidget;
class AbstractProfile;
class ProfileSettings;
class ProfileWidget;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QSpinBox;
class QStackedWidget;
class SChatWindow;
class Settings;

#ifdef SCHAT_UPDATE
class UpdateSettings;
#endif


/*!
 * \class SettingsDialog
 * \brief Диалог "Настройка".
 */
class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  enum {
    ProfilePage,
    NetworkPage,
    InterfacePage,
    UpdatePage
  };
  
  SettingsDialog(AbstractProfile *p, Settings *s, QWidget *parent = 0);
  void setPage(int page = 0);

public slots:
  void accept();
  void changePage(QListWidgetItem *current, QListWidgetItem *previous);
  void reset();

private slots:
  inline void validNick(bool b) { m_okButton->setEnabled(b); }
  
private:
  InterfaceSettings *m_interfacePage;
  NetworkSettings *m_networkPage;
  ProfileSettings *m_profilePage;
  QListWidget *m_contentsWidget;
  QPushButton *m_cancelButton;
  QPushButton *m_okButton;
  QPushButton *m_resetButton;
  QStackedWidget *m_pagesWidget;
  
  #ifdef SCHAT_UPDATE
  UpdateSettings *m_updatePage;
  #endif
};


/*!
 * \class SettingsDialog
 * \brief Диалог "Настройка", страница "Личные данные".
 */
class ProfileSettings : public QWidget
{
  Q_OBJECT
  
public:
  ProfileSettings(Settings *settings, AbstractProfile *profile, QWidget *parent = 0);
  void reset();
  void save();
  
signals:
  void validNick(bool b);
  
private:
  ProfileWidget *m_profileWidget;
  QLineEdit *m_byeMsgEdit;
  Settings *m_settings;
  AbstractProfile *m_profile;
};


/*!
 * \class NetworkSettings
 * \brief Диалог "Настройка", страница "Сеть".
 */
class NetworkSettings : public QWidget
{
  Q_OBJECT
  
public:
  NetworkSettings(Settings *settings, QWidget *parent = 0);
  void reset();
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
class InterfaceSettings : public QWidget
{
  Q_OBJECT
  
public:
  InterfaceSettings(Settings *settings, QWidget *parent = 0);
  void reset();
  void save();
  
private:
  QComboBox *m_styleComboBox;
  Settings *m_settings;
};


/*!
 * \class UpdateSettings
 * \brief Диалог "Настройка", страница "Обновления".
 */
#ifdef SCHAT_UPDATE
class UpdateSettings : public QWidget
{
  Q_OBJECT
  
public:
  UpdateSettings(Settings *settings, QWidget *parent = 0);
  void reset();
  void save();
  
private:
  QCheckBox *m_autoClean;
  QCheckBox *m_autoDownload;
  QSpinBox *m_interval;
  Settings *m_settings;
};
#endif

#endif /*SETTINGSDIALOG_H_*/
