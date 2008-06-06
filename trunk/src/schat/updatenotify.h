/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef UPDATENOTIFY_H_
#define UPDATENOTIFY_H_

#include <QObject>

class UpdateNotify : public QObject {
  Q_OBJECT
  
public:
  UpdateNotify(QObject *parent = 0);
  
public slots:
  void execute();

};

#endif /*UPDATENOTIFY_H_*/
