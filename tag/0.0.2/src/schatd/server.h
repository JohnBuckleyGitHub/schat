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
  
  #ifdef SCHAT_CLIENT
  void setLocalProfile(Profile *p) { localProfile = p; }
  #endif
  
signals:
  void incomingDirectConnection(const QString &p, ServerSocket *socket);

public slots:
  void appendParticipant(const QString &p);
  void clientSendNewProfile(quint16 sex, const QString &nick, const QString &name);
  void relayMessage(const QString &channel, const QString &nick, const QString &message);
  void relayParticipantList(ServerSocket *socket);
  
  #ifdef SCHAT_CLIENT
  void appendDirectParticipant(const QString &p);
  #endif
  
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
  
  #ifdef SCHAT_CLIENT
  void removeDirectConnection(ServerSocket *socket);
  Profile *localProfile;
  QHash<QString, ServerSocket *> directPeers;
  #endif
};

#endif /*SERVER_H_*/
