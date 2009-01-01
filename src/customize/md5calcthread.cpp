/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 - 2009 IMPOMEZIA <schat@impomezia.com>
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
Md5CalcThread::Md5CalcThread(const QMap<ProgressPage::Nsi, FileInfoLite> &files, QObject *parent)
  : QThread(parent), m_files(files)
{
  connect(this, SIGNAL(finished()), SLOT(deleteLater()));
}

/*!
 * Основной метод потока.
 */
void Md5CalcThread::run()
{
  bool noError = false;

  if (m_files.contains(ProgressPage::Core))
    noError = calc(ProgressPage::Core);

  if (m_files.contains(ProgressPage::Runtime) && noError)
    noError = calc(ProgressPage::Runtime);

  emit done(!noError);
}


/*!
 * Подсчёт контрольной суммы файла
 * \todo Эта функция для подсчёта md5 суммы загружает файл целиком в память, это не оптимально.
 *
 * \param type Тип файла
 * \return \a true в случае успеха.
 */
bool Md5CalcThread::calc(ProgressPage::Nsi type)
{
  FileInfoLite info = m_files.value(type);

  QFile file(info.name);

  if (!file.exists())
    return false;

  QCryptographicHash hash(QCryptographicHash::Md5);
  QByteArray result;

  if(!file.open(QIODevice::ReadOnly))
    return false;

  hash.addData(file.readAll());
  result = hash.result();
  emit done(type, result);

  return true;
}
