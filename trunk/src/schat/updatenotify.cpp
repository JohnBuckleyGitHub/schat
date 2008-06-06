/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>

#include "updatenotify.h"


/** [public]
 * 
 */
UpdateNotify::UpdateNotify(QObject *parent)
  : QObject(parent)
{

}


/** [public slots]
 * 
 */
void UpdateNotify::execute()
{
  qDebug() << "void UpdateNotify::execute()";
  
}
