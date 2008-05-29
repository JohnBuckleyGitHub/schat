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
}


/** [public slots]
 * 
 */
void Update::execute()
{
  m_state = GettingUpdateXml;
  m_download->get(QUrl(DefaultUpdateXmlUrl));  
}


/** [private slots]
 * 
 */
void Update::saved(const QString &filename)
{
  if (m_state == GettingUpdateXml) {
    m_state = ReadingUpdateXml;
    
    if (!m_reader.readFile(filename)) {
      QCoreApplication::instance()->exit(16);
      return;
    }
    
      qDebug() << "READ OK";
  }
}
