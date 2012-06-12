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

#include <QHBoxLayout>

#include "ChatCore.h"
#include "QProgressIndicator/QProgressIndicator.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/tabs/ProgressTab.h"

ProgressTab::ProgressTab(TabWidget *parent)
  : AbstractTab(QByteArray(), LS("progress"), parent)
{
  m_progress = new QProgressIndicator(this);
  m_progress->setAnimationDelay(100);
  m_progress->setMinimumSize(100, 100);
  m_progress->startAnimation();

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_progress, Qt::AlignCenter);

  setIcon(SCHAT_ICON(SmallLogo));
  retranslateUi();
}


void ProgressTab::retranslateUi()
{
  setText(tr("Connecting"));
}
