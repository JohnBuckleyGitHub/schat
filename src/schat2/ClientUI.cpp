/*
 * ClientUI.cpp
 *
 *  Created on: 17.02.2011
 *      Author: IMPOMEZIA
 */

#include <QtGui>

#include "ClientUI.h"

#include "qtwin/qtwin.h"
#include "net/SimpleClient.h"
#include "ui/tabs/ChannelTab.h"
#include "ui/StatusBar.h"
#include "ui/TabWidget.h"
#include "User.h"

#define SCHAT_RANDOM_CLIENT_ID
#include <QUuid>

ClientUI::ClientUI(QWidget *parent)
  : QMainWindow(parent),
    m_client(0)
{
  if (QtWin::isCompositionEnabled()) {
    QtWin::extendFrameIntoClientArea(this);
  }

  m_client = new SimpleClient(new User("IMPOMEZIA"), 0, this);
  #if defined(SCHAT_RANDOM_CLIENT_ID)
  m_client->user()->setNick(QUuid::createUuid().toString());
  #endif

  m_central = new QFrame(this);

  m_tabs = new TabWidget(m_client, this);
  m_statusBar = new StatusBar(m_client, this);

  m_url = new QLineEdit("schat://192.168.1.33:6999", this);
  m_statusBar->setUrl(m_url->text());
  m_channelTab = new ChannelTab(QByteArray(), m_tabs);

  m_tabs->addTab(m_channelTab, tr("Welcome"));
  m_send = new QLineEdit(this);

  setStatusBar(m_statusBar);

  QGridLayout *mainLay = new QGridLayout(m_central);
  mainLay->addWidget(m_url, 0, 0);
  mainLay->addWidget(m_tabs, 1, 0);
  mainLay->addWidget(m_send, 2, 0);
  setCentralWidget(m_central);

  connect(m_send, SIGNAL(returnPressed()), SLOT(send()));
  connect(m_url, SIGNAL(textChanged(const QString &)), m_statusBar, SLOT(setUrl(const QString &)));
}


void ClientUI::send()
{
  if (m_send->text().isEmpty())
    return;

  m_client->sendMessage(0, QByteArray(), m_send->text());
  m_send->clear();
}
