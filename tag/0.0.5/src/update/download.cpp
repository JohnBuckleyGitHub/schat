/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
