/*
 * ClientUI.h
 *
 *  Created on: 17.02.2011
 *      Author: IMPOMEZIA
 */

#ifndef CLIENTUI_H_
#define CLIENTUI_H_

#include <QMainWindow>

class QPushButton;
class SimpleClient;
class QTabWidget;
class QLineEdit;
class QToolButton;
class QTextBrowser;
class StatusBar;
class QFrame;
class TabWidget;
class ChannelTab;

class ClientUI : public QMainWindow
{
  Q_OBJECT

public:
  ClientUI(QWidget *parent = 0);

private slots:
  void send();

private:
  QLineEdit *m_url;
  QLineEdit *m_send;
  QFrame *m_central;
  SimpleClient *m_client;
  StatusBar *m_statusBar;
  TabWidget *m_tabs;
  ChannelTab *m_channelTab;
};

#endif /* CLIENTUI_H_ */
