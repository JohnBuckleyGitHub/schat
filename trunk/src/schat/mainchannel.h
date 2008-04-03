/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef MAINCHANNEL_H_
#define MAINCHANNEL_H_

#include <QLineEdit>
#include <QTextBrowser>
#include <QWidget>

class QAction;
class QHBoxLayout;
class QLabel;
class QToolButton;
class QVBoxLayout;

class MainChannel : public QWidget {
  Q_OBJECT

public:
  MainChannel(const QString &server, QWidget *parent = 0);
  QString server() { return serverEdit->text(); }
  void append(const QString &message);
  void displayChoiceServer(bool display);  
  void setServer(const QString &server) { serverEdit->setText(server); }
  
private:
  void createActions();
  void scroll();
  
  QAction *connectCreateAction;
  QToolButton *connectCreateButton;
  QHBoxLayout *topLayout;
  QLabel *serverLabel;
  QLineEdit *serverEdit;
  QTextBrowser chatText;
  QVBoxLayout *mainLayout;
};

#endif /*MAINCHANNEL_H_*/
