/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef MAINCHANNEL_H_
#define MAINCHANNEL_H_

#include <QLineEdit>
#include <QWidget>

#include "abstracttab.h"

class QAction;
class QHBoxLayout;
class QLabel;
class QToolButton;
class QVBoxLayout;

class MainChannel : public AbstractTab {
  Q_OBJECT

public:
  MainChannel(const QString &server, QWidget *parent = 0);
  inline QString server()                                                { return serverEdit->text(); } // TODO Использовать профиль
  inline void setServer(const QString &server)                           { serverEdit->setText(server); }
  void displayChoiceServer(bool display);
  
public slots:
  inline void msgNewMessage(const QString &nick, const QString &message) { browser->msgNewMessage(nick, message); }
  
private:
  void createActions();
  
  QAction *connectCreateAction;
  QHBoxLayout *topLayout;
  QLabel *serverLabel;
  QLineEdit *serverEdit;
  QToolButton *connectCreateButton;
  QVBoxLayout *mainLayout;
};

#endif /*MAINCHANNEL_H_*/
