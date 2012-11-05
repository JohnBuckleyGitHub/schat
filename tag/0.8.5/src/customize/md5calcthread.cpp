/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#include "md5calcthread.h"

/*!
 * Конструктор класса Md5CalcThread.
 */
Md5CalcThread::Md5CalcThread(const QMap<ProgressPage::Nsi, FileInfoLite> &files, const QString &pfxFile, const QString &pfxPassword, QObject *parent)
  : QThread(parent), m_files(files), m_pfxFile(QDir::toNativeSeparators(pfxFile)), m_pfxPassword(pfxPassword)
{
  m_pfx = (!m_pfxFile.isEmpty() && !m_pfxPassword.isEmpty());
  connect(this, SIGNAL(finished()), SLOT(deleteLater()));
}

/*!
 * Основной метод потока.
 */
void Md5CalcThread::run()
{
  if (m_pfx) {
    foreach (FileInfoLite info, m_files) {
      QProcess process;
      process.start(QString("SignTool.exe sign /f \"%1\" /p %2 /t http://timestamp.comodoca.com/authenticode \"%3\"")
          .arg(m_pfxFile)
          .arg(m_pfxPassword)
          .arg(info.name));
      process.waitForStarted(10000);
      process.waitForFinished();
    }
  }

  int errors = 0;

  if (m_files.contains(ProgressPage::Core))
    errors += calc(ProgressPage::Core);

  if (m_files.contains(ProgressPage::Runtime) && errors == 0)
    errors += calc(ProgressPage::Runtime);

  emit done(errors != 0);
}


/*!
 * Подсчёт контрольной суммы файла.
 *
 * \param type Тип файла
 * \return \a true в случае успеха.
 */
int Md5CalcThread::calc(ProgressPage::Nsi type)
{
  FileInfoLite info = m_files.value(type);

  QFile file(info.name);

  if (!file.exists())
    return 1;

  QCryptographicHash hash(QCryptographicHash::Md5);
  QByteArray result;

  if(!file.open(QIODevice::ReadOnly))
    return 1;

  while (file.bytesAvailable() > 65536) {
    hash.addData(file.read(65536));
  }
  hash.addData(file.readAll());

  result = hash.result();
  emit done(type, result, m_pfx ? QFileInfo(info.name).size() : 0);

  return 0;
}
