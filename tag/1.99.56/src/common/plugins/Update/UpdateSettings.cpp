/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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
#include <QVBoxLayout>
#include <QEvent>

#include "ChatCore.h"
#include "UpdateSettings.h"
#include "ChatSettings.h"
#include "sglobal.h"
#include "Path.h"

UpdateSettings::UpdateSettings(QWidget *parent)
  : QWidget(parent)
{
  m_autoDownload = new QCheckBox(this);
  m_autoDownload->setChecked(ChatCore::settings()->value(LS("Update/AutoDownload")).toBool());

  if (!Path::isPortable()) {
    m_autoDownload->setChecked(false);
    m_autoDownload->setEnabled(false);
  }

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_autoDownload);
  layout->setMargin(0);

  retranslateUi();

  connect(m_autoDownload, SIGNAL(clicked(bool)), SLOT(autoDownload(bool)));
}


void UpdateSettings::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void UpdateSettings::autoDownload(bool checked)
{
  ChatCore::settings()->setValue(LS("Update/AutoDownload"), checked);
}


void UpdateSettings::retranslateUi()
{
  m_autoDownload->setText(tr("Automatically download and install updates"));
}
