/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef DIRECTCHANNELSERVER_H_
#define DIRECTCHANNELSERVER_H_

#include <QTextBrowser>
#include <QWidget>
#include <QPointer>

#include "chatbrowser.h"
#include "serversocket.h"

class QVBoxLayout;

class DirectChannelServer : public QWidget {
  Q_OBJECT

public:
  enum ConnectionState {
    Disconnected,
    Connected,
  };
  
  DirectChannelServer(Profile *p, ServerSocket *s, QWidget *parent = 0);
  inline void append(const QString &message) { chatBrowser->add(message); }
  void sendText(const QString &text);
  
private slots:
  void removeConnection();
  
private:
  ChatBrowser *chatBrowser;
  ConnectionState state;
  Profile *profile;
  QPointer<ServerSocket> socket;
  QVBoxLayout *mainLayout;
};

#endif /*DIRECTCHANNELSERVER_H_*/
