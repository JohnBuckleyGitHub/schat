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
#include "update.h"


/** [public]
 * 
 */
Update::Update(const QUrl &url, QObject *parent)
  : QObject(parent)
{
  m_url        = url;
  m_appPath    = qApp->applicationDirPath();
  m_targetPath = m_appPath + "/updates";
  m_download   = new Download(m_targetPath, this);
  m_urlPath    = QFileInfo(m_url.toString()).path();
  
  connect(m_download, SIGNAL(saved(const QString &)), SLOT(saved(const QString &)));
  connect(m_download, SIGNAL(error()), SLOT(error()));
}


/** [public slots]
 * 
 */
void Update::execute()
{
  if (!QDir().exists(m_targetPath))
    if (!QDir().mkdir(m_targetPath))
      error(400);
  
  m_state = GettingUpdateXml;
  m_download->get(m_url);
}


/** [private slots]
 * Слот вызывается при успешном сохранении файла.
 * Инициатор: `Download::saveToDisk(const QString &, QIODevice *)`
 */
void Update::saved(const QString &filename)
{
  if (m_state == GettingUpdateXml) {
    
    if (!createQueue(filename))
      return;
      
    m_state = GettingUpdates;
    downloadNext();
  }
  else if (m_state == GettingUpdates) {
    if (verifyFile())
      downloadNext();
    else
      error(405);
  }
}


/** [private]
 * 
 */
bool Update::createQueue(const QString &filename)
{
  if (!m_reader.readFile(filename)) {
    error(401);
    return false;
  }
  
  if (!m_reader.isUpdateAvailable()) {
    error(400);
    return false;
  }
  
  QList<FileInfo> list = m_reader.list();
  
  foreach (FileInfo fileInfo, list) {
    m_files << fileInfo.name;
    if (!verifyFile(fileInfo))
      m_queue.enqueue(fileInfo);
  }
  
  return true;
}


/** [private]
 * 
 */
bool Update::verifyFile(const FileInfo &fileInfo)
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

  hash.addData(file.readAll());
  result = hash.result();
  
  if (result.toHex() != fileInfo.md5)
    return false;
   
  return true;
}


/** [private]
 * 
 */
void Update::downloadNext()
{
  if (!m_queue.isEmpty()) {
    currentFile = m_queue.dequeue();
    m_download->get(QUrl(m_urlPath + "/win32/" + currentFile.name));
  }
  else
    finished();
}


/** [private]
 * 
 */
void Update::error(int err)
{
  writeSettings(true);
  qApp->exit(err);
}


/** [private]
 * 
 */
void Update::finished()
{
  writeSettings();
     
  qApp->exit(0);
}


/** [private]
 * 
 */
void Update::writeSettings(bool err) const
{
  QSettings s(m_appPath + "/schat.conf", QSettings::IniFormat);
  s.beginGroup("Updates");
  
  if (!err) { 
    s.setValue("ReadyToInstall", true);
    s.setValue("Files", m_files);
  }
  else {
    s.setValue("ReadyToInstall", false);
    return;
  }
    
  s.setValue("LastDownloadedQtLevel", m_reader.qtLevel());
  s.setValue("LastDownloadedQtVersion", m_reader.qt());
  s.setValue("LastDownloadedCoreLevel", m_reader.coreLevel());
  s.setValue("LastDownloadedCoreVersion", m_reader.core());
}
