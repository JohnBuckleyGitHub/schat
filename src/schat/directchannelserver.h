/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef DIRECTCHANNELSERVER_H_
#define DIRECTCHANNELSERVER_H_

#include <QTextBrowser>
#include <QWidget>

#include "chatbrowser.h"
#include "serversocket.h"

class QVBoxLayout;

class DirectChannelServer : public QWidget {
  Q_OBJECT

public:
  DirectChannelServer(ServerSocket *s, QWidget *parent = 0);
  inline void append(const QString &message) { chatBrowser->add(message); }
  
private:
  ChatBrowser *chatBrowser;
  QVBoxLayout *mainLayout;
};

#endif /*DIRECTCHANNELSERVER_H_*/
