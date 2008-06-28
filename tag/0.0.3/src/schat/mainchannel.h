/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef MAINCHANNEL_H_
#define MAINCHANNEL_H_

#include <QLineEdit>
#include <QWidget>

#include "abstracttab.h"

class NetworkWidget;
class QAction;
class QHBoxLayout;
class QLabel;
class QToolButton;
class QVBoxLayout;
class Settings;

class MainChannel : public AbstractTab {
  Q_OBJECT

public:
  MainChannel(Settings *settings, QWidget *parent = 0);
  void displayChoiceServer(bool display);
  
public slots:
  inline void msgNewMessage(const QString &nick, const QString &message) { browser->msgNewMessage(nick, message); }

private slots:
  void serverChanged();
  
private:
  void createActions();
  
  NetworkWidget *m_networkWidget;
  QAction *m_connectCreateAction;
  QHBoxLayout *m_networkLayout;
  QToolButton *m_connectCreateButton;
  QVBoxLayout *m_mainLayout;
  Settings *m_settings;
};

#endif /*MAINCHANNEL_H_*/
