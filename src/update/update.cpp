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
  m_download = new Download(this);
  m_download->setBasePath(QCoreApplication::instance()->applicationDirPath());
  
  connect(m_download, SIGNAL(saved(const QString &)), SLOT(saved(const QString &)));
  connect(m_download, SIGNAL(error()), SLOT(error()));
}


/** [public slots]
 * 
 */
void Update::execute()
{
  m_state = GettingUpdateXml;
  m_download->get(QUrl(DefaultUpdateXmlUrl));
  m_reader.setPath(QFileInfo(DefaultUpdateXmlUrl).path());
}


/** [private slots]
 * 
 */
void Update::error()
{
  QCoreApplication::instance()->exit(32); // ошибка закачки
}


/** [private slots]
 * 
 */
void Update::saved(const QString &filename)
{
  if (m_state == GettingUpdateXml) {
    
    if (!m_reader.readFile(filename)) {
      QCoreApplication::instance()->exit(16); // не удалось прочитать update.xml
      return;
    }
    
    if (!m_reader.isUpdateAvailable()) {
      QCoreApplication::instance()->exit(8); // нет доступных обновлений
      return;
    }
    
    m_state = GettingUpdates;
    m_queue = m_reader.queue();
    m_download->get(QUrl(m_queue.dequeue()));
  }
  else if (m_state == GettingUpdates) {
    if (!m_queue.isEmpty()) {
      m_download->get(QUrl(m_queue.dequeue()));
    }
  }
}
