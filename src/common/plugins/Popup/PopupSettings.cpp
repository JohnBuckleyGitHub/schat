/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "PopupSettings.h"
#include "sglobal.h"

PopupSettings::PopupSettings(QWidget *parent)
  : QWidget(parent)
{
  m_label = new QLabel(this);
  m_enable = new QCheckBox(this);
  m_enable->setChecked(ChatCore::settings()->value(LS("Alerts/Popup")).toBool());

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(m_enable);
  layout->setContentsMargins(10, 0, 3, 0);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_label);
  mainLay->addLayout(layout);
  mainLay->setContentsMargins(0, 6, 0, 0);

  retranslateUi();

  connect(m_enable, SIGNAL(clicked(bool)),SLOT(enable(bool)));
}


void PopupSettings::enable(bool enable)
{
  ChatCore::settings()->setValue(LS("Alerts/Popup"), enable);
}


void PopupSettings::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == LS("Alerts/Popup"))
    m_enable->setChecked(value.toBool());
}


void PopupSettings::retranslateUi()
{
  m_label->setText(LS("<b>") + tr("Popup windows") + LS("</b>"));
  m_enable->setText(tr("Enable"));
}
