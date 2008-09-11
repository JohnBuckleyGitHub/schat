/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.com)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <QtGui>

#include "daemonui.h"
#include "version.h"

/*!
 * \brief Конструктор класса DaemonUi.
 */
DaemonUi::DaemonUi(QWidget *parent)
  : QDialog(parent)
{
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

  createActions();
  createTray();

  setWindowTitle(tr("IMPOMEZIA Simple Chat Daemon UI"));
}


void DaemonUi::createActions()
{
  m_quitAllAction = new QAction(QIcon(":/images/quit.png"), tr("Выход с &остановкой сервера"), this);
  m_quitAction = new QAction(QIcon(":/images/logout.png"), tr("&Выход"), this);
}


void DaemonUi::createTray()
{
  m_menu = new QMenu(this);
  m_menu->addAction(m_quitAllAction);
  m_menu->addAction(m_quitAction);

  m_tray = new QSystemTrayIcon(this);
  m_tray->setIcon(QIcon(":/images/logo16-gray.png"));
  m_tray->setToolTip(tr("IMPOMEZIA Simple Chat Daemon UI %1").arg(SCHAT_VERSION));
  m_tray->setContextMenu(m_menu);
  m_tray->show();
}
