/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef CHATBROWSER_H_
#define CHATBROWSER_H_

#include <QTextBrowser>
#include <QWidget>

class ChatBrowser : public QTextBrowser {
  Q_OBJECT
  
public:
  ChatBrowser(QWidget *parent = 0);
  void add(const QString &message);
  
private:
  void scroll();
};

#endif /*CHATBROWSER_H_*/
