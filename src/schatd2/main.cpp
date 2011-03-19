#include <QtGui>
#include <QApplication>

#include "Core.h"
#include "debugstream.h"
#include "net/PacketReader.h"
#include "net/PacketWriter.h"
#include "net/SimpleSocket.h"
#include "net/TransportReader.h"
#include "net/TransportWriter.h"
#include "protov4.h"
#include "Worker.h"
#include "WorkerThread.h"
#include "version.h"

#if defined(Q_WS_WIN32)
#include "qt_windows.h"
#endif


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  app.setApplicationName(SCHAT_NAME);
  app.setApplicationVersion(SCHAT_VERSION);

  Worker::setDefaultSslConf(app.applicationDirPath() + "/server.crt", app.applicationDirPath() + "/server.key");

  Core *core = new Core;
  core->start();

  protov4 w;
  w.show();
  int result = app.exec();
  core->quit();

  delete core;

  return result;
}
