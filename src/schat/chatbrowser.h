/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef CHATBROWSER_H_
#define CHATBROWSER_H_

#include <QTextBrowser>
#include <QTime>

class ChatBrowser : public QTextBrowser {
  Q_OBJECT
  
public:
  ChatBrowser(QWidget *parent = 0);
  void add(const QString &message);
  void newMessage(const QString &nick, const QString &message);
  void newParticipant(quint8 sex, const QString &nick);
  void participantLeft(quint8 sex, const QString &nick);
  
private:
  static inline QString currentTime() { return QTime::currentTime().toString("hh:mm:ss"); }
  void scroll();
  
};

#endif /*CHATBROWSER_H_*/
