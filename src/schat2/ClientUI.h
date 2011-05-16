/*
 * ClientUI.h
 *
 *  Created on: 17.02.2011
 *      Author: IMPOMEZIA
 */

#ifndef CLIENTUI_H_
#define CLIENTUI_H_

#include <QMainWindow>

class ChatCore;
class QPushButton;
class SimpleClient;
class QTabWidget;
class QLineEdit;
class QToolButton;
class QTextBrowser;
class StatusBar;
class QFrame;
class TabWidget;
class WelcomeTab;

class ClientUI : public QMainWindow
{
  Q_OBJECT

public:
  ClientUI(QWidget *parent = 0);

private slots:
  void send();

private:
  ChatCore *m_core;
  QFrame *m_central;
  QLineEdit *m_send;
  QLineEdit *m_url;
  StatusBar *m_statusBar;
  TabWidget *m_tabs;
};

#endif /* CLIENTUI_H_ */
