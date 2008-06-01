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
Update::Update(QObject *parent)
  : QObject(parent)
{
  m_updatesPath = qApp->applicationDirPath() + "/updates";
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
  m_download->get(QUrl(DefaultUpdateXmlUrl));
  m_reader.setPath(QFileInfo(DefaultUpdateXmlUrl).path());
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
    m_download->get(QUrl(m_queue.dequeue()));
  }
  else if (m_state == GettingUpdates) {
    if (m_queue.isEmpty()) {
      qApp->exit(0); // обновления успешно скачаны
    }
    else
      m_download->get(QUrl(m_queue.dequeue()));
  }
}
