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
  m_netPage    = new DaemonNetSettings(settings, this);

  createPage(QIcon(":/images/daemonsettings.png"), tr("Общие"), m_commonPage);
  createPage(QIcon(":/images/network.png"), tr("Сеть"), m_netPage);
}


void DaemonSettingsDialog::accept()
{
  emit save();
  m_settings->write();
  close();
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
  m_logLevel->setToolTip(tr("Уровень детализации журнала\n-1 журналирование отключено"));
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
  m_maxUsers->setToolTip(tr("Ограничение максимального количества пользователей которые могут быть подключены к серверу\n0 - без ограничений"));
  m_maxUsersPerIp = new QSpinBox(this);
  m_maxUsersPerIp->setRange(0, 10000);
  m_maxUsersPerIp->setValue(m_settings->getInt("MaxUsersPerIp"));
  m_maxUsersPerIp->setToolTip(tr("Ограничение максимального количества пользователей с одного адреса\n0 - без ограничений"));

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

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(listenGroup);
  mainLay->addWidget(logGroup);
  mainLay->addWidget(limitsGroup);
  mainLay->addStretch();
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
  m_settings->setString("ListenAddress", m_listen->currentText());
  m_settings->setInt("ListenPort",       m_port->value());
  m_settings->setInt("LogLevel",         m_logLevel->value());
  m_settings->setBool("ChannelLog",      m_channelLog->isChecked());
  m_settings->setInt("MaxUsers",         m_maxUsers->value());
  m_settings->setInt("MaxUsersPerIp",    m_maxUsersPerIp->value());
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


/*!
 * \brief Конструктор класса DaemonNetSettings.
 */
DaemonNetSettings::DaemonNetSettings(DaemonUiSettings *settings, QWidget *parent)
  : AbstractSettingsPage(DaemonSettingsDialog::NetPage, parent), m_settings(settings)
{
  m_network = new QCheckBox(tr("Разрешить поддержку &сети"), this);
  m_network->setChecked(m_settings->getBool("Network"));
  m_network->setToolTip(tr("Включить поддержку взаимодействия с другими серверами"));
  connect(m_network, SIGNAL(clicked(bool)), SLOT(enableAll(bool)));

  m_root = new QCheckBox(tr("&Корневой сервер"), this);
  m_root->setChecked(m_settings->getBool("RootServer"));
  m_root->setToolTip(tr("Определяет роль этого сервера в сети"));
  connect(m_root, SIGNAL(clicked(bool)), SLOT(changeRole(bool)));

  m_netName = new QLineEdit("Unknown Network", this);
  m_netName->setMaxLength(64);
  m_netName->setToolTip(tr("Название сети"));
  QLabel *netNameLabel = new QLabel(tr("&Название:"), this);
  netNameLabel->setBuddy(m_netName);

  m_key = new QLineEdit(this);
  m_key->setMaxLength(64);
  m_key->setEchoMode(QLineEdit::Password);
  m_key->setToolTip(tr("Уникальный ключ сети\nДолжен быть одинаков на всех серверах"));
  QLabel *keyLabel = new QLabel(tr("&Ключ:"), this);
  keyLabel->setBuddy(m_key);

  m_rootAddr = new QLineEdit(this);
  m_rootAddr->setToolTip(tr("Адрес корневого сервера\nЭто поле не используется корневым сервером"));
  QLabel *rootLabel = new QLabel(tr("Ко&рневой сервер:"), this);
  rootLabel->setBuddy(m_rootAddr);

  m_netGroup = new QGroupBox(tr("Сеть"), this);
  QGridLayout *netLay = new QGridLayout(m_netGroup);
  netLay->addWidget(netNameLabel, 0, 0);
  netLay->addWidget(m_netName, 0, 1);
  netLay->addWidget(keyLabel, 1, 0);
  netLay->addWidget(m_key, 1, 1);
  netLay->addWidget(rootLabel, 2, 0);
  netLay->addWidget(m_rootAddr, 2, 1);

  m_name = new QLineEdit("Unknown Server", this);
  if (!m_settings->getString("Name").isEmpty())
    m_name->setText(m_settings->getString("Name"));
  m_name->setMaxLength(64);
  m_name->setToolTip(tr("Имя данного сервера\nРекомендуется указывать реальное DNS имя"));
  QLabel *nameLabel = new QLabel(tr("Имя:"), this);
  nameLabel->setBuddy(m_name);
  QHBoxLayout *nameLay = new QHBoxLayout;
  nameLay->addWidget(nameLabel);
  nameLay->addWidget(m_name);

  m_numeric = new QSpinBox(this);
  m_numeric->setRange(1, 255);
  m_numeric->setValue(m_settings->getInt("Numeric"));
  m_numeric->setToolTip(tr("Уникальный для сети номер сервера"));
  QLabel *numericLabel = new QLabel(tr("Уникальный &номер:"), this);
  numericLabel->setBuddy(m_numeric);

  m_limit = new QSpinBox(this);
  m_limit->setRange(0, 255);
  m_limit->setValue(m_settings->getInt("MaxLinks"));
  m_limit->setToolTip(tr("Ограничение количества серверов которые могут быть подключены\nИспользуется только корневым сервером"));
  QLabel *limitLabel = new QLabel(tr("&Максимум серверов:"), this);
  limitLabel->setBuddy(m_limit);

  m_daemonGroup = new QGroupBox(tr("Сервер"), this);
  QGridLayout *daemonLay = new QGridLayout(m_daemonGroup);
  daemonLay->addLayout(nameLay, 0, 0, 1, 3);
  daemonLay->addWidget(numericLabel, 1, 0);
  daemonLay->addWidget(m_numeric, 1, 1);
  daemonLay->addWidget(limitLabel, 2, 0);
  daemonLay->addWidget(m_limit, 2, 1);
  daemonLay->setColumnStretch(2, 1);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_network);
  mainLay->addWidget(m_root);
  mainLay->addWidget(m_netGroup);
  mainLay->addWidget(m_daemonGroup);
  mainLay->addStretch();

  enableAll(m_network->isChecked());
  changeRole(m_root->isChecked());
}


void DaemonNetSettings::reset(int page)
{
  if (page == m_id) {
    m_network->setChecked(false);
    m_root->setChecked(false);
    m_netName->setText("Unknown Server");
    m_key->setText("");
    m_rootAddr->setText("");
    m_name->setText("Unknown Server");
    m_numeric->setValue(1);
    m_limit->setValue(0);
    enableAll(false);
    changeRole(false);
  }
}


void DaemonNetSettings::save()
{
  m_settings->setBool("Network", m_network->isChecked());

  if (m_network->isChecked()) {
    m_settings->setBool("RootServer", m_root->isChecked());
    m_settings->setInt("Numeric",     m_numeric->value());
    m_settings->setInt("MaxLinks",    m_limit->value());
    m_settings->setString("Name",     m_name->text());
  }
}


void DaemonNetSettings::changeRole(bool root)
{
  m_rootAddr->setEnabled(!root);
  m_limit->setEnabled(root);
}


void DaemonNetSettings::enableAll(bool enable)
{
  m_root->setEnabled(enable);
  m_netGroup->setEnabled(enable);
  m_daemonGroup->setEnabled(enable);
}
