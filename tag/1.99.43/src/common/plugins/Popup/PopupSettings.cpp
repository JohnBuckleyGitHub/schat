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
#include <QSpinBox>

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

  m_timeLabel = new QLabel(this);
  m_timeBox = new QSpinBox(this);
  m_timeBox->setRange(0, 60);
  m_timeBox->setValue(ChatCore::settings()->value(LS("Alerts/PopupTimeout")).toInt());

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(m_enable);
  layout->setContentsMargins(10, 0, 0, 0);

  QHBoxLayout *timeLay = new QHBoxLayout();
  timeLay->addWidget(m_timeLabel);
  timeLay->addWidget(m_timeBox);
  timeLay->addStretch();
  timeLay->setContentsMargins(10, 0, 0, 0);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_label);
  mainLay->addLayout(layout);
  mainLay->addLayout(timeLay);
  mainLay->setContentsMargins(0, 6, 0, 0);

  retranslateUi();

  connect(m_enable, SIGNAL(clicked(bool)),SLOT(enable(bool)));
  connect(m_timeBox, SIGNAL(valueChanged(int)),SLOT(timeChanged(int)));
}


void PopupSettings::enable(bool enable)
{
  ChatCore::settings()->setValue(LS("Alerts/Popup"), enable);
}


void PopupSettings::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == LS("Alerts/Popup"))
    m_enable->setChecked(value.toBool());
  else if (key == LS("Alerts/PopupTimeout"))
    m_timeBox->setValue(value.toInt());
}


void PopupSettings::timeChanged(int time)
{
  ChatCore::settings()->setValue(LS("Alerts/PopupTimeout"), time);
}


void PopupSettings::retranslateUi()
{
  m_label->setText(LS("<b>") + tr("Popup windows") + LS("</b>"));
  m_enable->setText(tr("Enable"));
  m_timeLabel->setText(tr("Time to display popup window"));
  m_timeBox->setSuffix(tr(" sec"));
}
