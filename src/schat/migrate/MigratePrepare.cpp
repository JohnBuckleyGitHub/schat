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

#include "migrate/Migrate.h"
#include "migrate/MigratePrepare.h"
#include "migrate/Spinner.h"
#include "migrate/MigrateWizard.h"

MigratePrepare::MigratePrepare(Migrate *migrate, QWidget *parent)
  : QWizardPage(parent)
  , m_migrate(migrate)
{
  setTitle(tr("Preparing to upgrade"));
  setSubTitle(tr("Checking for upgrades..."));
  setCommitPage(true);

  m_spinner = new Spinner(this);
  m_spinner->start();

  m_text = new QLabel(subTitle(), this);
  m_text->setWordWrap(true);

  QGridLayout *layout = new QGridLayout(this);
  layout->addWidget(m_spinner, 0, 0);
  layout->addWidget(m_text, 0, 1);
  layout->setColumnStretch(1, 1);

  connect(m_migrate, SIGNAL(done(int)), SLOT(reload(int)));
}


void MigratePrepare::reload(int status)
{
  if (status == Migrate::CheckError) {
    setSubTitle(tr("An error occurred while checking for upgrades."));
    m_text->setText(subTitle());

    m_spinner->stop();
  }
  else if (status == Migrate::UpdateAvailable) {
    setSubTitle(tr("New version is ready for download"));
    m_text->setText(tr("New version of Simple Chat available. Version <b>%1</b>, size <b>%2</b>.").arg(m_migrate->info().version).arg(MigrateWizard::bytesToHuman(m_migrate->info().size)));

    m_spinner->stop();
  }
}

