/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef DIRECTCHANNEL_H_
#define DIRECTCHANNEL_H_

#include <QLineEdit>
#include <QTextBrowser>
#include <QWidget>

class QAction;
class QHBoxLayout;
class QLabel;
class QToolButton;
class QVBoxLayout;

class DirectChannel : public QWidget {
  Q_OBJECT

public:
  DirectChannel(QWidget *parent = 0);
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

#endif /*DIRECTCHANNEL_H_*/
