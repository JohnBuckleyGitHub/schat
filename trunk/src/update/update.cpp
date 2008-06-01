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
  m_url = url;
  m_appPath = qApp->applicationDirPath();
  m_updatesPath = m_appPath + "/updates";
  m_download = new Download(this);
  m_download->setBasePath(m_updatesPath);
  
  connect(m_download, SIGNAL(saved(const QString &)), SLOT(saved(const QString &)));
  connect(m_download, SIGNAL(error()), SLOT(error()));
}


/** [public slots]
 * 
 */
void Update::execute()
{
  if (!QDir().exists(m_updatesPath))
    if (!QDir().mkdir(m_updatesPath))
      qApp->exit(64); // не удалось создать папку для обновлений
  
  m_state = GettingUpdateXml;
  m_download->get(m_url);
  m_reader.setPath(QFileInfo(m_url.toString()).path());
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
    
    if (!m_reader.readFile(filename)) {
      qApp->exit(16); // не удалось прочитать update.xml
      return;
    }
    
    if (!m_reader.isUpdateAvailable()) {
      qApp->exit(8); // нет доступных обновлений
      return;
    }
    
    m_state = GettingUpdates;
    m_queue = m_reader.queue();
    
    foreach (QString url, m_queue)
      m_files << QFileInfo(url).fileName();
    
    m_download->get(QUrl(m_queue.dequeue()));
  }
  else if (m_state == GettingUpdates) {
    if (m_queue.isEmpty()) {
      QString newName = m_appPath + "/schat-install.exe";
      
      if (QFile::exists(newName))
        QFile::remove(newName);
      
      if (!QFile::copy(m_appPath + "/schat-update.exe", newName)) {
        qApp->exit(4); // Ошибка создания копии
        return;
      }
      
//      QProcess::startDetached(m_appPath + "/schat-install.exe", QStringList() << "-install", m_appPath);
         
      qApp->exit(0); // обновления успешно скачаны

    }
    else
      m_download->get(QUrl(m_queue.dequeue()));
  }
}
