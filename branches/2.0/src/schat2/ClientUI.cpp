/*
 * ClientUI.cpp
 *
 *  Created on: 17.02.2011
 *      Author: IMPOMEZIA
 */

#include <QtGui>

#include "ClientUI.h"

#include "ChatCore.h"
#include "MessageAdapter.h"
#include "net/packets/message.h"
#include "net/SimpleClient.h"
#include "qtwin/qtwin.h"
#include "ui/StatusBar.h"
#include "ui/tabs/WelcomeTab.h"
#include "ui/TabWidget.h"
#include "User.h"

#define SCHAT_RANDOM_CLIENT_ID
#include <QUuid>

ClientUI::ClientUI(QWidget *parent)
  : QMainWindow(parent)
{
  if (QtWin::isCompositionEnabled()) {
    QtWin::extendFrameIntoClientArea(this);
  }

  m_core = new ChatCore(this);
  m_central = new QFrame(this);

  m_tabs = new TabWidget(this);
  m_statusBar = new StatusBar(m_core->client(), this);

  m_url = new QLineEdit("schat://192.168.1.33:7667", this);
  m_statusBar->setUrl(m_url->text());

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

  MessageData data(QByteArray(), m_tabs->currentId(), m_send->text());
  m_tabs->messageAdapter()->send(data);
//  m_client->sendMessage(0, QByteArray(), m_send->text());
//  m_client->parser()->parse(m_tabs->currentId(), m_send->text());
  m_send->clear();
}
