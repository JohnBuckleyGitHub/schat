/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>

#include "download.h"


/** [public]
 * 
 */
Download::Download(const QString &targetPath, QObject *parent)
  : QObject(parent)
{
  m_targetPath = targetPath;
  connect(&m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
}


/** [public]
 * 
 */
void Download::get(const QUrl &url)
{
  QNetworkRequest request(url);
  m_manager.get(request);
}


/** [private slots]
 * 
 */
void Download::downloadFinished(QNetworkReply *reply)
{
  QUrl url = reply->url();
  if (reply->error())
    emit error();
  else
    saveToDisk(saveFileName(url), reply);

  reply->deleteLater();
}


/** [private]
 * 
 */
bool Download::saveToDisk(const QString &filename, QIODevice *data)
{
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly))
    return false;

  file.write(data->readAll());
  file.close();
  
  emit saved(filename);

  return true;
}


/** [private]
 * 
 */
QString Download::saveFileName(const QUrl &url)
{
  QString path = url.path();
  QString basename = QFileInfo(path).fileName();

  if (basename.isEmpty())
    return basename;
  
  basename = m_targetPath + '/' + basename;

  if (QFile::exists(basename))
    QFile::remove(basename);

  return basename;
}
