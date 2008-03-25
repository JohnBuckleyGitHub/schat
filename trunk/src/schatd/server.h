/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <QTcpServer>
#include <QHash>
#include "serversocket.h"

class Server : public QTcpServer
{
  Q_OBJECT

public:
  Server(QObject *parent = 0);

public slots:
  void appendParticipant(const QString &p);
  void needParticipantList();
  void relayMessage(const QString &nick, const QString &message);
  
private slots:
  void connectionError(QAbstractSocket::SocketError socketError);
  void disconnected();

private:
  void removeConnection(ServerSocket *socket);
  void incomingConnection(int socketId);
  void participantLeft(const QString &nick);
  QHash<QString, ServerSocket *> peers;
};

#endif /*SERVER_H_*/
