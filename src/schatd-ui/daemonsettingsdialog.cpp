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
  QHBoxLayout *name = new QHBoxLayout(this);
  QLabel *label = new QLabel("test", this);
  name->addWidget(label);
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
