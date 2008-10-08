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

#include <QMap>
#include <QObject>
#include <QPalette>

#include "abstractsettingsdialog.h"

class DaemonCommonSettings;
class DaemonNetSettings;
class DaemonSettings;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLineEdit;
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

  DaemonSettingsDialog(DaemonSettings *settings, QWidget *parent = 0);

public slots:
  void accept();

private:
  DaemonCommonSettings *m_commonPage;
  DaemonNetSettings *m_netPage;
  DaemonSettings *m_settings;
};


/*!
 * \brief Общие настройки сервера.
 */
class DaemonCommonSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  DaemonCommonSettings(DaemonSettings *settings, QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private:
  void createListenList();

  DaemonSettings *m_settings;
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
  DaemonNetSettings(DaemonSettings *settings, QWidget *parent = 0);

signals:
  void validInput(bool valid);

public slots:
  void reset(int page);
  void save();

private slots:
  void changeRole(bool root);
  void enableAll();
  void inputChanged(const QString &text);

private:
  bool revalidate();
  void readNetwork();

  DaemonSettings *m_settings;
  QCheckBox *m_network;
  QCheckBox *m_root;
  QGroupBox *m_daemonGroup;
  QGroupBox *m_netGroup;
  QLineEdit *m_key;
  QLineEdit *m_name;
  QLineEdit *m_netName;
  QLineEdit *m_rootAddr;
  QMap<QString, QString> m_meta;
  QPalette m_normal;
  QPalette m_red;
  QSpinBox *m_limit;
  QSpinBox *m_numeric;
};

#endif /*DAEMONSETTINGSDIALOG_H_*/
