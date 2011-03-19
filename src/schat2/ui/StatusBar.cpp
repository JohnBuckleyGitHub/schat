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

#include "debugstream.h"
#include "net/SimpleClient.h"
#include "QProgressIndicator/QProgressIndicator.h"
#include "ui/StatusBar.h"

StatusBar::StatusBar(SimpleClient *client, QWidget *parent)
  : QStatusBar(parent),
    m_clientState(SimpleClient::ClientOffline),
    m_client(client)
{
  m_progress = new QProgressIndicator(this);
  m_progress->setAnimationDelay(100);
  m_progress->setMaximumSize(16, 16);
  m_progress->startAnimation();

  m_icon = new QLabel(this);
  m_secure = new QLabel(this);
  m_label = new QLabel(this);

  addWidget(m_progress);
  addWidget(m_icon);
  addWidget(m_secure);
  addWidget(m_label, 1);

  connect(m_client, SIGNAL(clientStateChanged(int)), SLOT(clientStateChanged(int)));

  updateStyleSheet();
  clientStateChanged(m_clientState);
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


void StatusBar::mouseReleaseEvent(QMouseEvent *event)
{
  bool context = event->button() == Qt::RightButton;
  if (event->button() != Qt::LeftButton && !context) {
    QStatusBar::mouseReleaseEvent(event);
    return;
  }

  QMenu menu;
  QAction *mainAction = 0;
  int mainActionState = -1;

  if (context || (event->button() == Qt::LeftButton && QApplication::widgetAt(event->globalPos()) == m_icon) || (event->button() == Qt::LeftButton && QApplication::widgetAt(event->globalPos()) == m_progress)) {
    if (m_clientState == SimpleClient::ClientOnline) {
      mainAction = menu.addAction(QIcon(":/images/plug-disconnect.png"), tr("Disconnect"));
      mainActionState = 1;
    }
    else if (m_clientState == SimpleClient::ClientOffline || m_clientState == SimpleClient::ClientError) {
      mainAction = menu.addAction(QIcon(":/images/plug.png"), tr("Connect"));
      mainActionState = 2;
    }
    else if (m_clientState == SimpleClient::ClientConnecting) {
      mainAction = menu.addAction(QIcon(":/images/plug-disconnect.png"), tr("Abort"));
      mainActionState = 1;
    }
  }

  if (menu.actions().size() == 0)
    return;

  QAction *action = menu.exec(event->globalPos());
  if (action) {
    if (action == mainAction){
      if (mainActionState == 1)
        m_client->leave();
      else if (mainActionState == 2)
        m_client->openUrl(m_url);
    }
  }

  if (mainAction)
    delete mainAction;
}


void StatusBar::clientStateChanged(int state)
{
  m_clientState = state;

  if (m_clientState == SimpleClient::ClientConnecting) {
    m_icon->setVisible(false);
    m_progress->setVisible(true);
    m_progress->startAnimation();
  }
  else {
    m_icon->setVisible(true);
    m_progress->setVisible(false);
    m_progress->stopAnimation();
  }

  if (m_clientState != SimpleClient::ClientOnline)
    m_secure->setVisible(false);

  if (m_clientState == SimpleClient::ClientOffline) {
    m_icon->setPixmap(QPixmap(":/images/offline.png"));
  }
  else if (m_clientState == SimpleClient::ClientOnline) {
    m_icon->setPixmap(QPixmap(":/images/online.png"));

    #if !defined(SCHAT_NO_SSL)
    if (m_client->isEncrypted()) {
      m_secure->setPixmap(QPixmap(":/images/secure.png"));
      m_secure->setVisible(true);
    }
    #endif
  }
  else if (m_clientState == SimpleClient::ClientError) {
    m_icon->setPixmap(QPixmap(":/images/network-error.png"));
  }

  retranslateUi();
}


void StatusBar::retranslateUi()
{
  m_secure->setToolTip(tr("Encrypted connection"));

  if (m_clientState == SimpleClient::ClientOffline) {
    m_label->setText(tr("No connection"));
    m_icon->setToolTip(tr("Unconnected"));
  }
  else if (m_clientState == SimpleClient::ClientConnecting) {
    m_label->setText(tr("Connecting..."));
  }
  else if (m_clientState == SimpleClient::ClientOnline) {
    m_label->setText(m_client->url().host());
    m_icon->setToolTip(tr("Connected"));
  }
  else if (m_clientState == SimpleClient::ClientError) {
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

