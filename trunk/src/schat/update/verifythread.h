/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#ifndef VERIFYTHREAD_H_
#define VERIFYTHREAD_H_

#include <QThread>

#include "updatexmlreader.h"

/*!
 * \brief Поток выполняющий проверку скачанных файлов.
 */
class VerifyThread : public QThread
{
  Q_OBJECT

public:
  VerifyThread(const QList<FileInfo> &files, const QString &targetPath, const QString &url, QObject *parent = 0);

signals:
  void finished(const QStringList &urls, qint64 size);

protected:
  void run();

private:
  bool verifyFile(const FileInfo &fileInfo) const;

  QList<FileInfo> m_files;
  QString m_targetPath;
  QString m_url;
};

#endif /* VERIFYTHREAD_H_ */
