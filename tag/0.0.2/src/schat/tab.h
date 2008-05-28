/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef TAB_H_
#define TAB_H_

#include <QWidget>

#include "abstracttab.h"

class Tab : public AbstractTab {
  Q_OBJECT

public:
  Tab(QWidget *parent = 0);
};

#endif /*TAB_H_*/
