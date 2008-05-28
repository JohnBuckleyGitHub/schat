/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>
#include "downloadmanager.h"

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  QStringList arguments = app.arguments();
  arguments.takeFirst();

  if (arguments.isEmpty())
    return 1;

  DownloadManager manager;
  manager.append(arguments);

  QObject::connect(&manager, SIGNAL(finished()), &app, SLOT(quit()));
  app.exec();
}
