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


DaemonSettingsDialog::~DaemonSettingsDialog()
{
  qDebug() << "DaemonSettingsDialog::~DaemonSettingsDialog()";
}


/*!
 * \brief Конструктор класса DaemonCommonSettings.
 */
DaemonCommonSettings::DaemonCommonSettings(DaemonUiSettings *settings, QWidget *parent)
  : AbstractSettingsPage(DaemonSettingsDialog::CommonPage, parent), m_settings(settings)
{
  m_listenCombo = new QComboBox(this);
  m_listenCombo->addItem("0.0.0.0");
  m_portSpin = new QSpinBox(this);
  m_portSpin->setRange(1024, 65536);
  m_portSpin->setValue(m_settings->getInt("ListenPort"));

  QLabel *listenLabel = new QLabel(tr("&Адрес:"), this);
  listenLabel->setBuddy(m_listenCombo);
  QLabel *portLabel = new QLabel(tr("&Порт:"), this);
  portLabel->setBuddy(m_portSpin);

  QGroupBox *listenGroup = new QGroupBox(tr("Интерфейс сервера"), this);
  QHBoxLayout *listenLay = new QHBoxLayout(listenGroup);
  listenLay->addWidget(listenLabel);
  listenLay->addWidget(m_listenCombo);
  listenLay->addWidget(portLabel);
  listenLay->addWidget(m_portSpin);
  listenLay->addStretch();

  m_logSpin = new QSpinBox(this);
  m_logSpin->setRange(-1, 0);
  m_logSpin->setValue(m_settings->getInt("LogLevel"));
  QLabel *logLabel = new QLabel(tr("&Уровень журналирования:"), this);
  logLabel->setBuddy(m_logSpin);
  QHBoxLayout *logLevelLay = new QHBoxLayout;
  logLevelLay->addWidget(logLabel);
  logLevelLay->addWidget(m_logSpin);
  logLevelLay->addStretch();

  QGroupBox *logGroup = new QGroupBox(tr("Журналирование"), this);
  QVBoxLayout *logLay = new QVBoxLayout(logGroup);
  logLay->addLayout(logLevelLay);

  m_mainChannelLog = new QCheckBox(tr("Вести журнал главного канала"), this);
  m_mainChannelLog->setChecked(m_settings->getBool("ChannelLog"));
  logLay->addWidget(m_mainChannelLog);
  logLay->setMargin(6);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(listenGroup);
  mainLayout->addWidget(logGroup);
  mainLayout->addStretch();
}


void DaemonCommonSettings::reset(int page)
{
  if (page == m_id) {
//    m_styleComboBox->setCurrentIndex(m_styleComboBox->findText("Plastique"));
  }
}


void DaemonCommonSettings::save()
{

}
