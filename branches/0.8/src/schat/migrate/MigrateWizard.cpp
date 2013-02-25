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

#include <QApplication>

#include "migrate/JSON.h"
#include "migrate/MigrateIntro.h"
#include "migrate/MigrateWizard.h"

#if defined(SCHAT_NO_AUTO_MIGRATE)
# include "migrate/MigrateManual.h"
#else
# include "migrate/Migrate.h"
# include "migrate/MigratePrepare.h"
# include "migrate/MigrateProgress.h"
#endif

MigrateWizard::MigrateWizard(const QString &data, QWidget *parent)
  : QWizard(parent)
{
  m_data = JSON::parse(data.toUtf8()).toMap();

  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

  setOption(QWizard::NoBackButtonOnStartPage, true);
  setWizardStyle(QWizard::ModernStyle);
  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/impomezia48.png"));
  setWindowTitle(QApplication::applicationName());

  setPage(PageIntro,  new MigrateIntro(m_data.value("intro", tr("Everything is ready for an upgrade to Simple Chat 2, this wizard will help you to upgrade.<br><br>Your network will soon stops supporting older versions of Simple Chat.")).toString(), this));

  const QString url = m_data.value("url", "schat://schat.me/achim").toString();

# if defined(SCHAT_NO_AUTO_MIGRATE)
  setPage(PageManual, new MigrateManual(url, this));
# else
  m_migrate = new Migrate(this);
  m_migrate->check();

  setPage(PagePrepare,  new MigratePrepare(m_migrate, this));
  setPage(PageProgress, new MigrateProgress(url, m_migrate, this));
# endif

  setStartId(PageIntro);
}


QString MigrateWizard::bytesToHuman(qint64 size)
{
  QString num;
  QString key;
  if (size < 1000000) {
    if (!size)
      num = QLatin1String("0");
    else if (size && size < 1024)
      num = QLatin1String("1");
    else
      num = QString::number((int) size / 1024);

    key = tr("kB");
  }
  else {
    num = QString::number((double) size / 1048576, 'f', 2);
    key = tr("MB");
  }

  return num += QLatin1Char(' ') + key;
}
