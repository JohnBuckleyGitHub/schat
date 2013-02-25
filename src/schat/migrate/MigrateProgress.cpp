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
#include <QLabel>
#include <QProcess>
#include <QProgressBar>
#include <QSettings>
#include <QTextEdit>
#include <QVBoxLayout>

#include "abstractprofile.h"
#include "migrate/Migrate.h"
#include "migrate/MigrateProgress.h"
#include "settings.h"

MigrateProgress::MigrateProgress(const QString &url, Migrate *migrate, QWidget *parent)
  : QWizardPage(parent)
  , m_completed(false)
  , m_migrate(migrate)
  , m_url(url)
{
  setTitle(tr("Downloading and installing upgrade"));
  setSubTitle(tr("Please wait..."));
  setFinalPage(true);

  m_label = new QLabel(tr("Downloading upgrade..."), this);
  m_progress = new QProgressBar(this);
  m_progress->setValue(0);
  m_log = new QTextEdit(this);
  m_log->setReadOnly(true);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_label);
  mainLay->addWidget(m_progress);
  mainLay->addWidget(m_log);
  mainLay->setMargin(0);
}


void MigrateProgress::initializePage()
{
  m_log->append(m_label->text());

  m_progress->setRange(0, m_migrate->info().size);
  m_migrate->download();

  connect(m_migrate, SIGNAL(done(int)), SLOT(done(int)));
  connect(m_migrate, SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64)));
}


void MigrateProgress::done(int status)
{
  if (m_migrate->state() != Migrate::Idle)
    return;

  if (status == Migrate::DownloadError) {
    m_log->append(QLatin1String("<span style='color:#900;'>") + tr("An error occurred when downloading upgrade.") + QLatin1String("</span>"));
    return;
  }

  m_progress->setRange(0, 100);
  m_label->setText(tr("Preparing to upgrade..."));
  m_log->append(m_label->text());

  QSettings settings(QApplication::applicationDirPath() + QLatin1String("/updates/default.conf"), QSettings::IniFormat, this);
  settings.setIniCodec("UTF-8");
  settings.setValue(QLatin1String("DefaultServer"), m_url);
  settings.setValue(QLatin1String("Profile/Nick"),  SimpleSettings->profile()->nick());

  m_progress->setValue(10);
  m_label->setText(tr("Installing upgrade..."));
  m_log->append(m_label->text());

  QProcess *process = new QProcess(this);
  connect(process, SIGNAL(error(QProcess::ProcessError)), SLOT(finished()));
  connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(finished(int)));

  process->start('"' + m_migrate->file().fileName() + '"', QStringList("/S"));
}


void MigrateProgress::downloadProgress(qint64 bytesReceived)
{
  m_progress->setValue(bytesReceived);
}


void MigrateProgress::finished(int exitCode)
{
  if (exitCode) {
    m_log->append(QLatin1String("<span style='color:#900;'>") + tr("An error occurred when installing upgrade.") + QLatin1String("</span>"));
    return;
  }

  m_label->setText(tr("Installation complete"));
  m_log->append(m_label->text());

  m_progress->setValue(100);
  m_completed = true;
  emit completeChanged();

  QProcess::startDetached('"' + QSettings(QLatin1String("HKEY_CURRENT_USER\\Software\\IMPOMEZIA\\Simple Chat 2"), QSettings::NativeFormat).value(QLatin1String(".")).toString() + QLatin1String("\\schat2.exe") + '"');
  QSettings(QLatin1String("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"), QSettings::NativeFormat).remove(QApplication::applicationName());

  emit closeChat();
}
