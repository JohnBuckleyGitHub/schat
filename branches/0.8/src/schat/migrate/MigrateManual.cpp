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

#include <QLabel>
#include <QGridLayout>
#include <QApplication>
#include <QClipboard>

#include "migrate/MigrateManual.h"

MigrateManual::MigrateManual(const QString &url, QWidget *parent)
  : QWizardPage(parent)
  , m_url(url)
{
  setTitle(tr("Manual upgrade"));
  setSubTitle(tr("Perform required actions for manual upgrade"));

  QLabel *step1 = new QLabel(tr("First, download the upgrade from %1").arg("<a href=\"https://schat.me\" style=\"text-decoration:none; color:#216ea7;\">www.schat.me</a>"), this);
  step1->setWordWrap(true);
  step1->setOpenExternalLinks(true);

  QLabel *step2 = new QLabel(tr("Then install and run Simple Chat 2"), this);
  step2->setWordWrap(true);

  QLabel *step3 = new QLabel(tr("Finally, enter your nick and server address <b>%1</b>. %2").arg(url).arg("<a href=\"#\" style=\"text-decoration:none; color:#216ea7;\">" + tr("Copy") + "</a>"), this);
  step3->setWordWrap(true);
  step3->setTextInteractionFlags(Qt::TextBrowserInteraction);

  QGridLayout *layout = new QGridLayout(this);
  layout->addWidget(step1, 0, 0);
  layout->addWidget(step2, 1, 0);
  layout->addWidget(step3, 2, 0);
  layout->setSpacing(16);

  connect(step3, SIGNAL(linkActivated(QString)), SLOT(linkActivated(QString)));
}


void MigrateManual::linkActivated(const QString &link)
{
  QApplication::clipboard()->setText(m_url);
}

