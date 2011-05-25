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
#include "ui/SendWidget.h"
#include "ui/InputWidget.h"

#define SCHAT_RANDOM_CLIENT_ID
#include <QUuid>

ClientUI::ClientUI(QWidget *parent)
  : QMainWindow(parent)
{
  if (QtWin::isCompositionEnabled()) {
    QtWin::extendFrameIntoClientArea(this);
  }

  m_core = new ChatCore(this);
  m_central = new QWidget(this);

  m_tabs = new TabWidget(this);
  m_statusBar = new StatusBar(m_core->client(), this);

  m_url = new QLineEdit("schat://192.168.1.33:7667", this);
  m_statusBar->setUrl(m_url->text());

  m_send = new SendWidget(this);

  setStatusBar(m_statusBar);

  QVBoxLayout *mainLay = new QVBoxLayout(m_central);
  mainLay->addWidget(m_url);
  mainLay->addWidget(m_tabs);
  mainLay->addWidget(m_send);
  mainLay->setStretchFactor(m_tabs, 999);
  mainLay->setStretchFactor(m_send, 1);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);
  setCentralWidget(m_central);

  connect(m_send->input(), SIGNAL(send(const QString &)), SLOT(send(const QString &)));
  connect(m_url, SIGNAL(textChanged(const QString &)), m_statusBar, SLOT(setUrl(const QString &)));
}


void ClientUI::send(const QString &text)
{
  if (text.isEmpty())
    return;

  MessageData data(QByteArray(), m_tabs->currentId(), text);
  m_tabs->messageAdapter()->send(data);
}
