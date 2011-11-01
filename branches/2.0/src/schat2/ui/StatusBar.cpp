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

#include <QAction>
#include <QApplication>
#include <QEvent>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QWidgetAction>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "messages/MessageAdapter.h"
#include "NetworkManager.h"
#include "QProgressIndicator/QProgressIndicator.h"
#include "ui/network/NetworkWidget.h"
#include "ui/StatusBar.h"
#include "ui/StatusWidget.h"
#include "ui/UserUtils.h"

StatusBar::StatusBar(QWidget *parent)
  : QStatusBar(parent)
  , m_client(ChatCore::i()->client())
{
  m_progress = new QProgressIndicator(this);
  m_progress->setAnimationDelay(100);
  m_progress->setMaximumSize(16, 16);
  m_progress->startAnimation();

  m_icon = new QLabel(this);
  m_login = new QLabel(this);
  m_secure = new QLabel(this);
  m_label = new QLabel(this);

  m_login->setPixmap(QPixmap(":/images/key.png"));

  m_url = new NetworkWidget(this);
  m_url->setMinimumWidth(m_url->width() * 2);
  m_urlAction = new QWidgetAction(this);
  m_urlAction->setDefaultWidget(m_url);

  m_status = new StatusWidget(this);

  addWidget(m_progress);
  addWidget(m_icon);
  addWidget(m_login);
  addWidget(m_secure);
  addWidget(m_label, 1);
  addPermanentWidget(m_status);

  connect(m_client, SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));
  connect(ChatCore::i()->adapter(), SIGNAL(loggedIn(const QString&)), SLOT(loggedIn(const QString&)));

  updateStyleSheet();
  clientStateChanged(SimpleClient::ClientOffline);
}


bool StatusBar::event(QEvent *event)
{
  #if defined(Q_WS_WIN)
  if (event->type() == QEvent::ApplicationPaletteChange) {
    updateStyleSheet();
  }
  #endif

  return QStatusBar::event(event);
}


void StatusBar::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QStatusBar::changeEvent(event);
}


/*!
 * Показ меню.
 */
void StatusBar::mouseReleaseEvent(QMouseEvent *event)
{
  if (childAt(event->pos()) == m_icon || event->button() == Qt::RightButton)
    menu(event->globalPos());
  else
    QStatusBar::mouseReleaseEvent(event);
}


void StatusBar::clientStateChanged(int state)
{
  if (state == SimpleClient::ClientConnecting) {
    m_icon->setVisible(false);
    m_progress->setVisible(true);
    m_progress->startAnimation();
  }
  else {
    m_icon->setVisible(true);
    m_progress->setVisible(false);
    m_progress->stopAnimation();
  }

  if (state != SimpleClient::ClientOnline) {
    m_login->setVisible(false);
    m_secure->setVisible(false);
  }

  if (state == SimpleClient::ClientOffline) {
    m_icon->setPixmap(QPixmap(":/images/offline.png"));
  }
  else if (state == SimpleClient::ClientOnline) {
    m_icon->setPixmap(QPixmap(":/images/online.png"));

    if (!UserUtils::user()->account().isEmpty())
      loggedIn(UserUtils::user()->account());

    #if !defined(SCHAT_NO_SSL)
    if (m_client->isEncrypted()) {
      m_secure->setPixmap(QPixmap(":/images/secure.png"));
      m_secure->setVisible(true);
    }
    #endif
  }
  else if (state == SimpleClient::ClientError) {
    m_icon->setPixmap(QPixmap(":/images/network-error.png"));
  }

  retranslateUi();
}


void StatusBar::loggedIn(const QString &name)
{
  m_login->setToolTip(name);
  m_login->setVisible(true);
}


void StatusBar::menuTriggered(QAction *action)
{
  if (action == m_connect)
    m_url->open();
}


void StatusBar::menu(const QPoint &point)
{
  QMenu menu(this);
  if (m_client->clientState() == SimpleClient::ClientOffline)
    menu.addAction(m_urlAction);

  QAction *action = m_url->connectAction();
  m_connect = menu.addAction(action->icon(), action->text());

  connect(&menu, SIGNAL(triggered(QAction*)), SLOT(menuTriggered(QAction*)));
  menu.exec(point);
}


void StatusBar::retranslateUi()
{
  m_secure->setToolTip(tr("Encrypted connection"));
  int state = m_client->clientState();

  if (state == SimpleClient::ClientOffline) {
    m_label->setText(tr("No connection"));
    m_icon->setToolTip(tr("No connection"));
  }
  else if (state == SimpleClient::ClientConnecting) {
    m_label->setText(tr("Connecting..."));
  }
  else if (state == SimpleClient::ClientOnline) {
    m_label->setText(NetworkManager::currentServerName());
    m_icon->setToolTip(tr("Connected"));
  }
  else if (state == SimpleClient::ClientError) {
    m_label->setText(tr("Error"));
    m_icon->setToolTip(tr("Error"));
  }
}


void StatusBar::updateStyleSheet()
{
  #if defined(Q_OS_MAC)
  setStyleSheet("QStatusBar { background: qlineargradient(x1: 1, y1: 0, x2: 1, y2: 1, stop: 0 #ededed, stop: 1 #c8c8c8); } QStatusBar::item { border-width: 0; }");
  #else
    #if defined(Q_WS_WIN)
    setStyleSheet(QString("QStatusBar { background-color: %1; } QStatusBar::item { border-width: 0; }").arg(parentWidget()->palette().color(QPalette::Window).name()));
    #else
    setStyleSheet("QStatusBar::item { border-width: 0; }");
    #endif
  #endif
}

