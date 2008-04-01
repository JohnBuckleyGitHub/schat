/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef MAINCHANNEL_H_
#define MAINCHANNEL_H_

#include <QTextBrowser>
#include <QWidget>
#include <QLineEdit>

class QHBoxLayout;
class QLabel;
class QVBoxLayout;

class MainChannel : public QWidget {
  Q_OBJECT

public:
  MainChannel(const QString &server, QWidget *parent = 0);
  void append(const QString &message);
  void displayChoiceServer(bool display);  
  void setServer(const QString &server) { serverEdit->setText(server); }
  QString server() { return serverEdit->text(); }
  
private:
  void scroll();
  
  QHBoxLayout *topLayout;
  QLabel *serverLabel;
  QLineEdit *serverEdit;
  QTextBrowser chatText;
  QVBoxLayout *mainLayout;
};

#endif /*MAINCHANNEL_H_*/
