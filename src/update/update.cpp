/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
  m_queue.isEmpty();
}


/** [public slots]
 * 
 */
void Update::execute()
{
  if (!QDir().exists(m_targetPath))
    if (!QDir().mkdir(m_targetPath))
      qApp->exit(400);
  
  m_state = GettingUpdateXml;
  m_download->get(m_url);
}


/** [private slots]
 * 
 */
void Update::error()
{
  qApp->exit(32); // ошибка закачки
}


/** [private slots]
 * 
 */
void Update::saved(const QString &filename)
{
  if (m_state == GettingUpdateXml) {
    
    createQueue(filename);
    m_state = GettingUpdates;
    downloadNext();
  }
  else if (m_state == GettingUpdates) {
    if (verifyFile())
      downloadNext();
    else
      qApp->exit(32);
  }
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
void Update::createQueue(const QString &filename)
{
  if (!m_reader.readFile(filename)) {
    qApp->exit(401);
    return;
  }
  
  if (!m_reader.isUpdateAvailable()) {
    qApp->exit(100);
    return;
  }
  
  QList<FileInfo> list = m_reader.list();
  
  foreach (FileInfo fileInfo, list) {
    m_files << fileInfo.name;
    if (!verifyFile(fileInfo))
      m_queue.enqueue(fileInfo);
  }
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
void Update::finished()
{
  QString newName = m_appPath + "/schat-install.exe";
  
  if (QFile::exists(newName))
    QFile::remove(newName);
  
  if (!QFile::copy(m_appPath + "/schat-update.exe", newName)) {
    qApp->exit(402);
    return;
  }
  
  writeSettings();
     
  qApp->exit(0);
}


/** [private]
 * 
 */
void Update::writeSettings() const
{
  QSettings s(m_appPath + "/schat.conf", QSettings::IniFormat);
  s.beginGroup("Updates");
  s.setValue("ReadyToInstall", true);
  s.setValue("Files", m_files);
  s.setValue("LastDownloadedQtLevel", m_reader.qtLevel());
  s.setValue("LastDownloadedQtVersion", m_reader.qt());
  s.setValue("LastDownloadedCoreLevel", m_reader.coreLevel());
  s.setValue("LastDownloadedCoreVersion", m_reader.core());
}
