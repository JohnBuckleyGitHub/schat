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
  inline QString server()                                                { return m_serverEdit->text(); } // TODO Использовать профиль
  inline void setServer(const QString &server)                           { m_serverEdit->setText(server); }
  void displayChoiceServer(bool display);
  
public slots:
  inline void msgNewMessage(const QString &nick, const QString &message) { browser->msgNewMessage(nick, message); }
  
private:
  void createActions();
  
  NetworkWidget *m_networkWidget;
  QAction *m_connectCreateAction;
  QHBoxLayout *m_networkLayout;
  QHBoxLayout *m_topLayout;
  QLabel *m_serverLabel;
  QLineEdit *m_serverEdit;
  QToolButton *m_connectCreateButton;
  QVBoxLayout *m_mainLayout;
  Settings *m_settings;
};

#endif /*MAINCHANNEL_H_*/
