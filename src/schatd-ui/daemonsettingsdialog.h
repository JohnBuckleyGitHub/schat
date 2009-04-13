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

#ifndef DAEMONSETTINGSDIALOG_H_
#define DAEMONSETTINGSDIALOG_H_

#include <QMap>
#include <QObject>
#include <QPalette>

#include "abstractsettingsdialog.h"

class QCheckBox;
class QComboBox;
class QCommandLinkButton;
class QGroupBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class ServiceInstaller;

/*!
 * \brief Настройка сервера
 */
class DaemonSettingsDialog : public AbstractSettingsDialog
{
  Q_OBJECT

public:
  enum Page {
    CommonPage,
    NetPage,
    #ifndef SCHATD_NO_SERVICE
    ServicePage
    #endif
  };

  DaemonSettingsDialog(QWidget *parent = 0);

public slots:
  void accept();
};


/*!
 * \brief Общие настройки сервера.
 */
class DaemonCommonSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  DaemonCommonSettings(QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private:
  void createListenList();

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
  DaemonNetSettings(QWidget *parent = 0);

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


/*!
 * \brief Общие настройки сервера.
 */
#ifndef SCHATD_NO_SERVICE
class DaemonServiceSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  /// Статус установки сервиса.
  enum Status {
    Unknown,        ///< Не определёное состояние.
    Invalid,        ///< Ошибка, установка сервиса не возможна, не найдены файлы из Windows Resource Kit.
    ReadyToInstall, ///< Всё готово к установке сервиса.
    Installed,      ///< Сервис установлен.
    Wait            ///< Ожидание установки/удаления сервиса.
  };

  DaemonServiceSettings(QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private slots:
  void clicked();
  void serviceNameChanged(const QString &text);

private:
  bool exist(QLabel *label, const QString &file) const;
  void detect();
  void setCommandLinkState();

  QCommandLinkButton *m_install;
  QGroupBox *m_installGroup;
  QLabel *m_info;
  QLabel *m_instsrvExe;
  QLabel *m_srvanyExe;
  QLabel *m_state;
  QLineEdit *m_serviceName;
  ServiceInstaller* const m_installer;
  Status m_status;
};
#endif

#endif /*DAEMONSETTINGSDIALOG_H_*/
