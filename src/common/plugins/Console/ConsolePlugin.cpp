/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QMenu>
#include <QtPlugin>
#include <QTimer>

#include "ChatCore.h"
#include "ConsoleCmd.h"
#include "ConsolePlugin.h"
#include "ConsolePlugin_p.h"
#include "sglobal.h"
#include "Translation.h"
#include "ui/ConsoleTab.h"
#include "ui/TabWidget.h"
#include "ui/TabsToolBar.h"

ConsolePluginImpl::ConsolePluginImpl(QObject *parent)
  : ChatPlugin(parent)
  , m_console(0)
{
  new ConsoleCmd(this);

  ChatCore::translation()->addOther(LS("console"));

  QTimer::singleShot(0, this, SLOT(start()));
}


void ConsolePluginImpl::show()
{
  TabWidget *tabs = TabWidget::i();
  if (tabs->showPage("console") == -1)
    tabs->showPage(new ConsoleTab(tabs));
}


void ConsolePluginImpl::showMenu(QMenu *menu, QAction *separator)
{
  Q_UNUSED(separator)

  m_console->setText(tr("Console"));
  m_console->setChecked(TabWidget::isCurrent(TabWidget::page("console")));

  menu->addAction(m_console);
}


void ConsolePluginImpl::start()
{
  if (!TabWidget::i())
    return;

  m_console = new QAction(this);
  m_console->setIcon(QIcon(LS(":/images/Console/terminal.png")));
  m_console->setCheckable(true);

  connect(TabWidget::i()->toolBar(), SIGNAL(showMenu(QMenu*,QAction*)), SLOT(showMenu(QMenu*,QAction*)));
  connect(m_console, SIGNAL(triggered(bool)), SLOT(show()));
}


ChatPlugin *ConsolePlugin::create()
{
  m_plugin = new ConsolePluginImpl(this);
  return m_plugin;
}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(Console, ConsolePlugin);
#endif

