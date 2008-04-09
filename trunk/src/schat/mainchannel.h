/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef MAINCHANNEL_H_
#define MAINCHANNEL_H_

#include <QLineEdit>
#include <QTextBrowser>
#include <QWidget>

#include "chatbrowser.h"

class QAction;
class QHBoxLayout;
class QLabel;
class QToolButton;
class QVBoxLayout;

class MainChannel : public QWidget {
  Q_OBJECT

public:
  MainChannel(const QString &server, QWidget *parent = 0);  
  inline QString server()                      { return serverEdit->text(); }
  inline void append(const QString &message)   { chatBrowser.add(message); }
  inline void setServer(const QString &server) { serverEdit->setText(server); }
  void displayChoiceServer(bool display);
  
private:
  void createActions();
  
  ChatBrowser chatBrowser;
  QAction *connectCreateAction;
  QHBoxLayout *topLayout;
  QLabel *serverLabel;
  QLineEdit *serverEdit;
  QToolButton *connectCreateButton;
  QVBoxLayout *mainLayout;
};

#endif /*MAINCHANNEL_H_*/
