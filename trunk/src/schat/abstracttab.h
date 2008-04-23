/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef ABSTRACTTAB_H_
#define ABSTRACTTAB_H_

#include <QIcon>
#include <QWidget>

#include "chatbrowser.h"

class QTextBrowser;

class AbstractTab : public QWidget {
  Q_OBJECT

public:
  enum Type {
    Unknown,
    Main,
    Private,
    Direct,
    DirectServer
  };
  
  AbstractTab(QWidget *parent = 0);
  
  bool notice;
  ChatBrowser *browser;
  QIcon icon;
  Type type;
};

#endif /*ABSTRACTTAB_H_*/
