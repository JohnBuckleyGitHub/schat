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

#include <QEvent>
#include <QLabel>
#include <QMovie>
#include <Qt>
#include <QTextDocument>

#include "widget/connectionstatus.h"
#include "widget/statusmenu.h"

/*!
 * Конструктор класса ConnectionStatus.
 */
ConnectionStatus::ConnectionStatus(StatusMenu *menu, QWidget *parent)
  : QStatusBar(parent),
  m_state(UnconnectedState)
{
  m_icon = new QLabel(this);
  m_icon->setVisible(false);

  m_movie = new QLabel(this);
  m_movie->setMovie(new QMovie(":/images/load.gif", QByteArray(), this));
  m_movie->setVisible(false);

  m_status = new QLabel(this);
  m_status->setWordWrap(true);
  m_status->setTextFormat(Qt::PlainText);

  m_userStatus = new StatusWidget(menu, this);

  addWidget(m_icon);
  addWidget(m_movie);
  addWidget(m_status, 1);
  addPermanentWidget(m_userStatus);

  updateStyleSheet();
  setState(UnconnectedState);
}


QString ConnectionStatus::echoText() const
{
  if (m_state == UnconnectedState)
    return tr("Connection lost");

  if (m_state == ConnectedState) {
    if (m_network.isEmpty())
      return tr("Successfully connected to server %1").arg(Qt::escape(m_server));
    else
      return tr("Successfully connected to network <b>%1</b> (%2)").arg(Qt::escape(m_network)).arg(Qt::escape(m_server));
  }

  return "";
}


void ConnectionStatus::setGender(bool male)
{
  m_userStatus->setGender(male);
}


/*!
 * Установка статуса подключения.
 */
void ConnectionStatus::setState(State state, const QString &server, const QString &network)
{
  m_state = state;
  m_network = network;
  m_server = server;

  if (state == UnconnectedState) {
    m_movie->movie()->setPaused(true);
    m_movie->setVisible(false);
    m_icon->setVisible(true);
    m_icon->setPixmap(QPixmap(":/images/network_disconnect.png"));
  }
  else if (state == ConnectingState) {
    m_movie->movie()->setPaused(false);
    m_movie->setVisible(true);
    m_icon->setVisible(false);
  }
  else if (state == ConnectedState) {
    m_movie->movie()->setPaused(true);
    m_movie->setVisible(false);
    m_icon->setVisible(true);
    m_icon->setPixmap(QPixmap(":/images/network_connect.png"));
  }

  retranslateUi();
}


void ConnectionStatus::setUserStatus(int status)
{
  m_userStatus->setStatus(static_cast<StatusMenu::Status>(status));
}


bool ConnectionStatus::event(QEvent *event)
{
  #if defined(Q_WS_WIN)
  if (event->type() == QEvent::ApplicationPaletteChange) {
    updateStyleSheet();
  }
  #endif

  return QStatusBar::event(event);
}


void ConnectionStatus::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QStatusBar::changeEvent(event);
}


void ConnectionStatus::retranslateUi()
{
  if (m_state == UnconnectedState) {
    m_status->setText(tr("No connection"));
  }
  else if (m_state == ConnectingState) {
    if (m_network.isEmpty())
      m_status->setText(tr("Connecting to server %1...").arg(m_server));
    else
      m_status->setText(tr("Connecting to network %1...").arg(m_network));
  }
  else if (m_state == ConnectedState) {
    if (m_network.isEmpty())
      m_status->setText(tr("Server %1").arg(m_server));
    else
      m_status->setText(tr("Network %1 (%2)").arg(m_network).arg(m_server));
  }
}


void ConnectionStatus::updateStyleSheet()
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
