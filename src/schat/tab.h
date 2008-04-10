/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef TAB_H_
#define TAB_H_

#include <QTextBrowser>
#include <QWidget>

#include "chatbrowser.h"

class QVBoxLayout;

class Tab : public QWidget {
  Q_OBJECT

public:
  Tab(QWidget *parent = 0);
  ~Tab();
  inline void append(const QString &message) { chatBrowser->add(message); }
  
public slots:
  inline void newMessage(const QString &nick, const QString &message) { chatBrowser->newMessage(nick, message); }
  inline void newParticipant(quint8 sex, const QString &nick)         { chatBrowser->newParticipant(sex, nick); }
  inline void participantLeft(quint8 sex, const QString &nick)        { chatBrowser->participantLeft(sex, nick); }
  
private:
  ChatBrowser *chatBrowser;
  QVBoxLayout *mainLayout;
};

#endif /*TAB_H_*/
