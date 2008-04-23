/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef DIRECTCHANNELSERVER_H_
#define DIRECTCHANNELSERVER_H_

#include <QWidget>
#include <QPointer>

#include "abstracttab.h"
#include "serversocket.h"

class QVBoxLayout;

class DirectChannelServer : public AbstractTab {
  Q_OBJECT

public:
  enum ConnectionState {
    Disconnected,
    Connected,
  };
  
  DirectChannelServer(Profile *p, ServerSocket *s, QWidget *parent = 0);
  virtual ~DirectChannelServer();
  void changeSocket(ServerSocket *s);
  void sendText(const QString &text);
  
private slots:
  void removeConnection();
  
private:
  void initSocket();
  
  ConnectionState state;
  Profile *profile;
  QPointer<ServerSocket> socket;
  QVBoxLayout *mainLayout;
};

#endif /*DIRECTCHANNELSERVER_H_*/
