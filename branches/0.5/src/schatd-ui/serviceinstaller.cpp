/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QtCore>

#include "serviceinstaller.h"

/*!
 * Конструктор класса ServiceInstaller.
 */
ServiceInstaller::ServiceInstaller(QObject *parent)
  : QObject(parent),
  m_process(0),
  m_state(Ready)
{
}


ServiceInstaller::~ServiceInstaller()
{
  qDebug() << "~ServiceInstaller()";
}


/*!
 * Запуск процедуры установки сервиса.
 */
void ServiceInstaller::install(const QString &name)
{
  qDebug() << "install()" << name;

  if (m_state != Ready)
    return;

  if (name.isEmpty() || exists(name))
    return;

  m_state = Installing;
  m_name  = name;

  if (!m_process)
    m_process = new QProcess(this);

  connect(m_process, SIGNAL(error(QProcess::ProcessError)), SLOT(error()));
  connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(finished(int, QProcess::ExitStatus)));

  m_process->start('"' + QCoreApplication::applicationDirPath() + "/instsrv.exe\" \"" + name + "\" "
      + QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/srvany.exe") + '"');
}


void ServiceInstaller::uninstall(const QString &name)
{
  qDebug() << "uninstall()" << name;
}


/*!
 * Проверка на наличие сервера с именем \p name в реестре.
 *
 * \return \a true если сервис найден.
 */
bool ServiceInstaller::exists(const QString &name)
{
  QSettings s("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services", QSettings::NativeFormat);

  if (s.value(name + "/Start", -1).toInt() != -1 || !s.value(name + "/ImagePath", "").toString().isEmpty())
    return true;

  return false;
}


void ServiceInstaller::error()
{
  qDebug() << "error()";
}


void ServiceInstaller::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
  qDebug() << "finished()" << exitCode << exitStatus;

  if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
    QSettings s("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\" + m_name, QSettings::NativeFormat);
    s.setValue("Parameters/Application", QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/schatd.exe"));
  }
}
