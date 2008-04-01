/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef TAB_H_
#define TAB_H_

#include <QTextBrowser>
#include <QWidget>

class QVBoxLayout;

class Tab : public QWidget {
  Q_OBJECT

public:
  Tab(QWidget *parent = 0);
  void append(const QString &message);
  
private:
  void scroll();
  
  QTextBrowser chatText;
  QVBoxLayout *mainLayout;
};

#endif /*TAB_H_*/
