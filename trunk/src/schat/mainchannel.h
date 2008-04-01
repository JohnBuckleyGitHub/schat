/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef MAINCHANNEL_H_
#define MAINCHANNEL_H_

#include <QTextBrowser>
#include <QWidget>

class QHBoxLayout;
class QLabel;
class QLineEdit;
class QVBoxLayout;

class MainChannel : public QWidget {
  Q_OBJECT

public:
  MainChannel(QWidget *parent = 0);
  void append(const QString &message);
  void displayChoiceServer(bool display);  
  
private:
  void scroll();
  
  QHBoxLayout *topLayout;
  QLabel *serverLabel;
  QLineEdit *serverEdit;
  QTextBrowser chatText;
  QVBoxLayout *mainLayout;
};

#endif /*MAINCHANNEL_H_*/
