/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <QHash>
#include <QTcpServer>

#include "serversocket.h"
#include "profile.h"

class Server : public QTcpServer
{
  Q_OBJECT

public:
  Server(QObject *parent = 0);
  bool start();
  
  // FIXME добавить #define ...
  void setLocalProfile(Profile *p) { localProfile = p; }

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
  Profile *localProfile;
};

#endif /*SERVER_H_*/
