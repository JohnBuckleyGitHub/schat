/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include <QVBoxLayout>

#include "ChatCore.h"
#include "qtwin/qtwin.h"
#include "ui/ChatWindow.h"
#include "ui/SendWidget.h"
#include "ui/StatusBar.h"
#include "ui/TabWidget.h"

ChatWindow::ChatWindow(QWidget *parent)
  : QMainWindow(parent)
{
  if (QtWin::isCompositionEnabled()) {
    QtWin::extendFrameIntoClientArea(this);
  }

  m_core = new ChatCore(this);
  m_central = new QWidget(this);

  m_tabs = new TabWidget(this);
  m_statusBar = new StatusBar(m_core->client(), this);

  m_send = new SendWidget(this);

  setStatusBar(m_statusBar);

  QVBoxLayout *mainLay = new QVBoxLayout(m_central);
  mainLay->addWidget(m_tabs);
  mainLay->addWidget(m_send);
  mainLay->setStretchFactor(m_tabs, 999);
  mainLay->setStretchFactor(m_send, 1);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);
  setCentralWidget(m_central);

  connect(m_send, SIGNAL(send(const QString &)), SLOT(send(const QString &)));
}


void ChatWindow::send(const QString &text)
{
  m_core->send(m_tabs->currentId(), text);
}
