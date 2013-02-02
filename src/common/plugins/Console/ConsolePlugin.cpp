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

#include <QApplication>
#include <QMenu>
#include <QTimer>
#include <QtPlugin>

#include "ChatCore.h"
#include "ConsoleCmd.h"
#include "ConsolePlugin.h"
#include "ConsolePlugin_p.h"
#include "sglobal.h"
#include "Tr.h"
#include "Translation.h"
#include "ui/ConsoleTab.h"
#include "ui/TabsToolBar.h"
#include "ui/TabWidget.h"

class ConsoleTr : public Tr
{
  Q_DECLARE_TR_FUNCTIONS(ConsoleTr)

public:
  ConsoleTr() : Tr() { m_prefix = LS("console_"); }

protected:
  QString valueImpl(const QString &key) const
  {
    if (key == LS("bad_server"))                return tr("This server does not support remote management.");
    else if (key == LS("password"))             return tr("Password:");
    else if (key == LS("login"))                return tr("Login");
    else if (key == LS("empty_password"))       return tr("Password cannot be empty");
    else if (key == LS("incorect_password"))    return tr("You entered an incorrect password");
    else if (key == LS("change_password"))      return tr("Change password");
    else if (key == LS("new_password"))         return tr("New password:");
    else if (key == LS("confirm_new_password")) return tr("Confirm new password:");
    else if (key == LS("save"))                 return tr("Save");
    else if (key == LS("cancel"))               return tr("Cancel");
    else if (key == LS("password_mismatch"))    return tr("The passwords you entered don't match");
    else if (key == LS("password_short"))       return tr("Password is too short");
    else if (key == LS("home"))                 return tr("Home");
    else if (key == LS("logout"))               return tr("Logout");
    else if (key == LS("server_name"))          return tr("Server Name");
    return QString();
  }
};

ConsolePluginImpl::ConsolePluginImpl(QObject *parent)
  : ChatPlugin(parent)
  , m_console(0)
{
  m_tr = new ConsoleTr();
  new ConsoleCmd(this);

  ChatCore::translation()->addOther(LS("console"));

  QTimer::singleShot(0, this, SLOT(start()));
}


ConsolePluginImpl::~ConsolePluginImpl()
{
  delete m_tr;
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

