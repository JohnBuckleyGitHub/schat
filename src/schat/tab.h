/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef TAB_H_
#define TAB_H_

#include <QWidget>
#include <QTextBrowser>

class Tab : public QWidget {
  Q_OBJECT

public:
  Tab(QWidget *parent = 0);
  void append(const QString &message);
  
private:
  QTextBrowser chatText;
  void scroll();
};

#endif /*TAB_H_*/
