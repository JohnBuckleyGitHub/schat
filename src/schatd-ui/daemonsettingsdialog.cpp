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

#include <QtGui>
#include <QtNetwork>

#include "daemonsettingsdialog.h"
#include "daemonuisettings.h"

/*!
 * \brief Конструктор класса DaemonSettingsDialog.
 */
DaemonSettingsDialog::DaemonSettingsDialog(DaemonUiSettings *settings, QWidget *parent)
  : AbstractSettingsDialog(parent), m_settings(settings)
{
  m_commonPage = new DaemonCommonSettings(settings, this);
  createPage(QIcon(":/images/daemonsettings.png"), tr("Общие"), m_commonPage);
}


/*!
 * \brief Конструктор класса DaemonCommonSettings.
 */
DaemonCommonSettings::DaemonCommonSettings(DaemonUiSettings *settings, QWidget *parent)
  : AbstractSettingsPage(DaemonSettingsDialog::CommonPage, parent), m_settings(settings)
{
  m_listen = new QComboBox(this);
  m_listen->setToolTip(tr("Адрес на котором сервер будет ожидать подключения"));
  createListenList();

  m_port = new QSpinBox(this);
  m_port->setRange(1024, 65536);
  m_port->setValue(m_settings->getInt("ListenPort"));
  m_port->setToolTip(tr("Порт на котором сервер будет ожидать подключения"));

  QLabel *listenLabel = new QLabel(tr("&Адрес:"), this);
  listenLabel->setBuddy(m_listen);
  QLabel *portLabel = new QLabel(tr("&Порт:"), this);
  portLabel->setBuddy(m_port);

  QGroupBox *listenGroup = new QGroupBox(tr("Интерфейс сервера"), this);
  QHBoxLayout *listenLay = new QHBoxLayout(listenGroup);
  listenLay->addWidget(listenLabel);
  listenLay->addWidget(m_listen);
  listenLay->addWidget(portLabel);
  listenLay->addWidget(m_port);
  listenLay->addStretch();

  m_logLevel = new QSpinBox(this);
  m_logLevel->setRange(-1, 0);
  m_logLevel->setValue(m_settings->getInt("LogLevel"));
  m_logLevel->setToolTip(tr("Уровень детализации журнала, -1 журналирование отключено"));
  QLabel *logLabel = new QLabel(tr("&Уровень журналирования:"), this);
  logLabel->setBuddy(m_logLevel);
  QHBoxLayout *logLevelLay = new QHBoxLayout;
  logLevelLay->addWidget(logLabel);
  logLevelLay->addWidget(m_logLevel);
  logLevelLay->addStretch();

  QGroupBox *logGroup = new QGroupBox(tr("Журналирование"), this);
  QVBoxLayout *logLay = new QVBoxLayout(logGroup);
  logLay->addLayout(logLevelLay);

  m_channelLog = new QCheckBox(tr("Вести журнал &главного канала"), this);
  m_channelLog->setChecked(m_settings->getBool("ChannelLog"));
  m_channelLog->setToolTip(tr("Управляет режимом записи событий основного канала в специальный журнал"));
  logLay->addWidget(m_channelLog);

  m_maxUsers = new QSpinBox(this);
  m_maxUsers->setRange(0, 10000);
  m_maxUsers->setValue(m_settings->getInt("MaxUsers"));
  m_maxUsers->setToolTip(tr("Ограничение максимального количества пользователей которые могут быть подключены к серверу, 0 - без ограничений"));
  m_maxUsersPerIp = new QSpinBox(this);
  m_maxUsersPerIp->setRange(0, 10000);
  m_maxUsersPerIp->setValue(m_settings->getInt("MaxUsersPerIp"));
  m_maxUsersPerIp->setToolTip(tr("Ограничение максимального количества пользователей с одного адреса, 0 - без ограничений"));

  QLabel *maxUsersLabel = new QLabel(tr("&Лимит пользователей:"), this);
  maxUsersLabel->setBuddy(m_maxUsers);
  QLabel *maxUsersPerIpLabel = new QLabel(tr("Лимит подключений с &одного адреса:"), this);
  maxUsersPerIpLabel->setBuddy(m_maxUsersPerIp);

  QGroupBox *limitsGroup = new QGroupBox(tr("Ограничения"), this);
  QGridLayout *limitsLay = new QGridLayout(limitsGroup);
  limitsLay->addWidget(maxUsersLabel, 0, 0);
  limitsLay->addWidget(m_maxUsers, 0, 1);
  limitsLay->addWidget(maxUsersPerIpLabel, 1, 0);
  limitsLay->addWidget(m_maxUsersPerIp, 1, 1);
  limitsLay->setColumnStretch(2, 1);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(listenGroup);
  mainLayout->addWidget(logGroup);
  mainLayout->addWidget(limitsGroup);
  mainLayout->addStretch();
}


void DaemonCommonSettings::reset(int page)
{
  if (page == m_id) {
    m_listen->setCurrentIndex(0);
    m_port->setValue(7666);
    m_logLevel->setValue(0);
    m_channelLog->setChecked(false);
    m_maxUsers->setValue(0);
    m_maxUsersPerIp->setValue(0);
  }
}


void DaemonCommonSettings::save()
{

}


void DaemonCommonSettings::createListenList()
{
  m_listen->addItem("0.0.0.0");

  QList<QHostAddress> list = QNetworkInterface::allAddresses();
  foreach (QHostAddress addr, list)
    m_listen->addItem(addr.toString());

  QString listenAddress = m_settings->getString("ListenAddress");
  int index = m_listen->findText(listenAddress);
  if (index == -1)
    m_listen->setCurrentIndex(0);
  else
    m_listen->setCurrentIndex(index);
}
