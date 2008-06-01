/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "update.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QStringList arguments = app.arguments();
  arguments.takeFirst();

//  if (arguments.isEmpty())
//    return 1;

  Update update;
  QTimer::singleShot(0, &update, SLOT(execute()));
      
  app.exec();
}
