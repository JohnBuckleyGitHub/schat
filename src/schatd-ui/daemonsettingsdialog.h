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

#ifndef DAEMONSETTINGSDIALOG_H_
#define DAEMONSETTINGSDIALOG_H_

#include <QObject>

#include "abstractsettingsdialog.h"

class DaemonCommonSettings;
class DaemonNetSettings;
class DaemonUiSettings;
class QCheckBox;
class QComboBox;
class QSpinBox;

/*!
 * \brief Настройка сервера
 */
class DaemonSettingsDialog : public AbstractSettingsDialog
{
  Q_OBJECT

public:
  enum Page {
    CommonPage,
    NetPage
  };

  DaemonSettingsDialog(DaemonUiSettings *settings, QWidget *parent = 0);

public slots:
  void accept();

private:
  DaemonCommonSettings *m_commonPage;
  DaemonNetSettings *m_netPage;
  DaemonUiSettings *m_settings;
};


/*!
 * \brief Общие настройки сервера.
 */
class DaemonCommonSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  DaemonCommonSettings(DaemonUiSettings *settings, QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private:
  void createListenList();

  DaemonUiSettings *m_settings;
  QCheckBox *m_channelLog;
  QComboBox *m_listen;
  QSpinBox *m_logLevel;
  QSpinBox *m_maxUsers;
  QSpinBox *m_maxUsersPerIp;
  QSpinBox *m_port;
};


/*!
 * \brief Настройки сети серверов.
 */
class DaemonNetSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  DaemonNetSettings(DaemonUiSettings *settings, QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private:
  DaemonUiSettings *m_settings;
};

#endif /*DAEMONSETTINGSDIALOG_H_*/
