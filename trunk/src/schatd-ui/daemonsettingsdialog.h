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

#ifndef SCHATD_NO_SERVICE
class QtServiceController;
#endif

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
 * \brief Страница установки/удаления сервиса.
 */
#ifndef SCHATD_NO_SERVICE
class DaemonServiceSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  /// Статус установки сервиса.
  enum Status {
    Unknown,        ///< Не определённое состояние.
    ReadyToInstall, ///< Всё готово к установке сервиса.
    Installed,      ///< Сервис установлен.
    ErrorInstall,   ///< Ошибка при установке сервиса.
    ErrorRemove,    ///< Ошибка при удалении сервиса.
  };

  DaemonServiceSettings(QWidget *parent = 0);
  ~DaemonServiceSettings();

public slots:
  void reset(int page);
  void save();

private slots:
  void clicked();

private:
  inline void setState(Status status) { m_status = status; setState(); }
  void detect();
  void setCommandLinkState();
  void setState();

  QCommandLinkButton *m_install;
  QGroupBox *m_installGroup;
  QLabel *m_serviceName;
  QLabel *m_stateLabel;
  QtServiceController *m_controller;
  Status m_status;
};
#endif

#endif /*DAEMONSETTINGSDIALOG_H_*/
