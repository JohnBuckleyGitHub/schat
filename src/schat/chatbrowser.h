/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef CHATBROWSER_H_
#define CHATBROWSER_H_

#include <QTextBrowser>
#include <QTime>

#include "channellog.h"

class ChatBrowser : public QTextBrowser {
  Q_OBJECT
  
public:
  ChatBrowser(QWidget *parent = 0);
  inline QString channel()                       { return m_channelLog->channel(); }
  inline void setChannel(const QString &channel) { m_channelLog->setChannel(channel); }
  static inline QString currentTime()            { return QTime::currentTime().toString("hh:mm:ss"); }
  void add(const QString &message);
  void msg(const QString &text);
  void msgChangedNick(quint16 sex, const QString &oldNick, const QString &newNick);
  void msgChangedProfile(quint16 sex, const QString &nick);
  void msgDisconnect();
  void msgNewParticipant(quint8 sex, const QString &nick);
  void msgParticipantLeft(quint8 sex, const QString &nick, const QString &bye);
  void msgReadyForUse(const QString &addr);
  void msgReadyForUse(const QString &network, const QString &addr);
  void scroll();
  
protected:
  void contextMenuEvent(QContextMenuEvent *event);
  
public slots:
  void msgNewMessage(const QString &nick, const QString &message);
  
private:
  ChannelLog *m_channelLog;
};

#endif /*CHATBROWSER_H_*/
