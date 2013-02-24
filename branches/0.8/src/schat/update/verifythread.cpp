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

#include <QCryptographicHash>
#include <QFile>

#include "verifythread.h"

/*!
 * Конструктор класса VerifyThread.
 *
 * \param files      Список файлов которые необходимо проверить.
 * \param targetPath Папка в которой находятся файлы.
 * \param url        Адрес файлов, исключая имя файла.
 * \param parent     Указатель на родительский объект.
 */
VerifyThread::VerifyThread(const QList<FileInfo> &files, const QString &targetPath, const QString &url, QObject *parent)
  : QThread(parent), m_files(files), m_targetPath(targetPath), m_url(url)
{
  connect(this, SIGNAL(finished()), SLOT(deleteLater()));
}


/*!
 * Основной метод потока.
 */
void VerifyThread::run()
{
  QStringList out;
  qint64 size = 0;

  foreach (FileInfo file, m_files) {
    if (!verifyFile(file)) {
      out << (m_url + file.name);
      size += file.size;
    }
  }

  emit finished(out, size);
}

/*!
 * Проверка локального файла.
 * Наличие, размер, контрольная сумма.
 *
 * \param fileInfo Структура содержащая информацию о файле.
 * \return \a true в случае успешной проверки файла, иначе \a false;
 */
bool VerifyThread::verifyFile(const FileInfo &fileInfo) const
{
  QString fileName = m_targetPath + '/' + fileInfo.name;
  QFile file(fileName);

  if (!file.exists())
    return false;

  if (file.size() != fileInfo.size)
    return false;

  QCryptographicHash hash(QCryptographicHash::Md5);
  QByteArray result;

  if(!file.open(QIODevice::ReadOnly))
    return false;

  while (file.bytesAvailable() > 65536) {
    hash.addData(file.read(65536));
  }
  hash.addData(file.readAll());
  result = hash.result();

  if (result.toHex() != fileInfo.md5)
    return false;

  return true;
}
