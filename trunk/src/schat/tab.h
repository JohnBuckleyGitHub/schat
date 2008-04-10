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
  inline void append(const QString &message)                             { chatBrowser->add(message); }
  
public slots:
  inline void msgNewMessage(const QString &nick, const QString &message) { chatBrowser->msgNewMessage(nick, message); }
  inline void msgNewParticipant(quint8 sex, const QString &nick)         { chatBrowser->msgNewParticipant(sex, nick); }
  inline void msgParticipantLeft(quint8 sex, const QString &nick)        { chatBrowser->msgParticipantLeft(sex, nick); }
  
private:
  ChatBrowser *chatBrowser;
  QVBoxLayout *mainLayout;
};

#endif /*TAB_H_*/
