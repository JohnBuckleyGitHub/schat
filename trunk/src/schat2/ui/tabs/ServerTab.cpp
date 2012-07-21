/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QFile>
#include <QVBoxLayout>
#include <QTextDocument>

#include "ChatAlerts.h"
#include "ChatCore.h"
#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "hooks/ChannelMenu.h"
#include "messages/ServiceMessage.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/ServerTab.h"
#include "ui/TabWidget.h"
#include "WebBridge.h"

ServerTab::ServerTab(TabWidget *parent)
  : AbstractTab(QByteArray(), LS("server"), parent)
{
  m_options |= CanSendMessage;

  QString file = QApplication::applicationDirPath() + "/styles/test/html/Server.html";
  if (QFile::exists(file))
    file = QUrl::fromLocalFile(file).toString();
  else
    file = "qrc:/html/Server.html";

  m_chatView = new ChatView(QByteArray(), file, this);

  m_deleteOnClose = false;

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_chatView);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  setIcon(SCHAT_ICON(Globe));

  connect(ChatClient::i(), SIGNAL(online()), SLOT(online()));
  connect(ChatAlerts::i(), SIGNAL(alert(const Alert &)), SLOT(alert(const Alert &)));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
  connect(ChatClient::io(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));

  retranslateUi();
}


bool ServerTab::bindMenu(QMenu *menu)
{
  Hooks::ChannelMenu::bind(menu, ChatClient::server());
  return true;
}


void ServerTab::changeEvent(QEvent *event)
{
  AbstractTab::changeEvent(event);

  if (event->type() == QEvent::LanguageChange) {
    WebBridge::i()->retranslate();
    ChatNotify::start(Notify::Language);
  }
}


void ServerTab::alert(const Alert &alert)
{
  if (alert.type() == LS("online"))
    m_chatView->add(ServiceMessage::connected());
  else if (alert.type() == LS("offline"))
    m_chatView->add(ServiceMessage::connectionLost());
}


void ServerTab::notify(const Notify &notify)
{
  int type = notify.type();
  if (type == Notify::ShowID) {
    if (notify.data().toByteArray() == id())
      m_chatView->add(ServiceMessage::showId(notify.data().toByteArray()));
  }
  else if (type == Notify::OpenChannel || type == Notify::OpenInfo) {
    if (notify.data() != id())
      return;

    if (m_tabs->indexOf(this) == -1) {
      m_tabs->addTab(this, QString());
      setOnline();
      retranslateUi();
    }

    m_tabs->setCurrentIndex(m_tabs->indexOf(this));

    if (type == Notify::OpenInfo)
      chatView()->evaluateJavaScript(LS("Pages.setPage(1);"));
    else
      chatView()->evaluateJavaScript(LS("Pages.setPage(0);"));
  }
  if (notify.type() == Notify::ServerRenamed) {
    retranslateUi();
  }
}


void ServerTab::online()
{
  setId(ChatClient::serverId());
  m_chatView->setId(id());
}


/*!
 * Обработка изменения состояния клиента.
 */
void ServerTab::clientStateChanged(int state)
{
  if (state == ChatClient::WaitAuth) {
    ServiceMessage message(tr("Server %1 requires authorization").arg(LS("<b>") + Qt::escape(ChatClient::serverName()) + LS("</b>")));

    message.data()[LS("Type")]  = LS("info");
    message.data()[LS("Extra")] = LS("orange-text");
    chatView()->add(message);
    chatView()->evaluateJavaScript(LS("AuthDialog.show()"));

    if (m_tabs->indexOf(this) == -1) {
      m_tabs->addTab(this, QString());
      setOnline();
    }

    m_tabs->setCurrentIndex(m_tabs->indexOf(this));
  }

  retranslateUi();
}


void ServerTab::retranslateUi()
{
  QString name = ChatClient::serverName();
  if (name.isEmpty())
    name = tr("Network");

  setText(name);
}
