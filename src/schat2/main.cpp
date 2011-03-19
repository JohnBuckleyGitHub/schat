#include "debugstream.h"

#include "net/PacketWriter.h"
#include "net/TransportWriter.h"
#include "net/TransportReader.h"
#include "net/PacketReader.h"
#include "net/SimpleSocket.h"
#include "version.h"
#include "ClientUI.h"

#include <QtGui>
#include <QApplication>

#if defined(Q_WS_WIN32)
#include "qt_windows.h"
#endif

void connect(QObject *parent)
{
  QCryptographicHash hash(QCryptographicHash::Sha1);
  hash.addData("IMPOMEZIA");
  QByteArray data = hash.result();

  SCHAT_DEBUG_STREAM(data.toHex())

  QByteArray packetBuffer;
  packetBuffer.reserve(8192);
  QDataStream packetStream(&packetBuffer, QIODevice::ReadWrite);
  QList<QByteArray> list;

  list.clear();
  PacketWriter writer(&packetStream, 4);
  writer.put("IMPOMEZIA");
  list.append(writer.data());

  SimpleSocket *socket = new SimpleSocket(0, parent);
  socket->connectToHost("192.168.1.33", 7666);
  socket->waitForConnected();

  #if defined(SCHAT_BENCHMARK)
  QTime t;
  t.start();
  for (int i = 0; i < SCHAT_BENCHMARK; ++i) {
  #endif

  socket->send(list);
//  socket->send(list);
//  socket->send(list);

  #if defined(SCHAT_BENCHMARK)
  }
  qDebug() << t.elapsed() << "ms";
  qDebug() << "tx bytes:" << socket->tx() << "rx bytes:" << socket->rx();
  #endif

  SCHAT_DEBUG_STREAM("tx bytes:" << socket->tx() << "rx bytes:" << socket->rx())
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(SCHAT_NAME);
    a.setApplicationVersion(SCHAT_VERSION);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    #if defined(SCHAT_BENCHMARK) && defined(Q_WS_WIN32)
    Sleep(5000);
    #endif


    ClientUI w;
    #if defined(SCHAT_BENCHMARK)
    connect(&w);
    #endif
    w.show();
    return a.exec();
}
