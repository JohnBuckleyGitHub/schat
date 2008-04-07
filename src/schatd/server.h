/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <QHash>
#include <QTcpServer>

#include "serversocket.h"

class Server : public QTcpServer
{
  Q_OBJECT

public:
  Server(QObject *parent = 0);
  bool start();
  
  // FIXME добавить #define ...
  void setLocalFullName(const QString &name) { localFullName = name; }
  void setLocalNick(const QString &nick) { localNick = nick; }
  void setLocalSex(quint8 s) { localSex = s; }

public slots:
  void appendParticipant(const QString &p);
  void relayMessage(const QString &channel, const QString &nick, const QString &message);
  void relayParticipantList(ServerSocket *socket);
  
private slots:
  void connectionError(QAbstractSocket::SocketError socketError);
  void disconnected();

private:
  void incomingConnection(int socketId);
  void participantLeft(const QString &nick);
  void readSettings();
  void removeConnection(ServerSocket *socket);
  
  QHash<QString, ServerSocket *> peers;
  QString listenAddress;
  quint16 listenPort;
  
  // FIXME добавить #define ...
  QString localFullName;
  QString localNick; 
  quint8 localSex;
};

#endif /*SERVER_H_*/
