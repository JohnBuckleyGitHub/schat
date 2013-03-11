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

#include "ChannelsCmd.h"
#include "ChannelsMenuImpl.h"
#include "ChannelsPlugin.h"
#include "ChannelsPlugin_p.h"
#include "ChannelsSettings.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientFeeds.h"
#include "feeds/FeedStrings.h"
#include "hooks/ChatViewHooks.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Tr.h"
#include "Translation.h"
#include "ui/ListTab.h"
#include "ui/tabs/ChatView.h"
#include "ui/TabsToolBar.h"
#include "ui/TabWidget.h"

class ChannelsTr : public Tr
{
  Q_DECLARE_TR_FUNCTIONS(ChannelsTr)

public:
  ChannelsTr() : Tr() { m_prefix = LS("channels_"); }

protected:
  QString valueImpl(const QString &key) const
  {
    if      (key == LS("join"))           return tr("Join");
    else if (key == LS("channel_name"))   return tr("Channel name");
    else if (key == LS("list"))           return tr("Channels");
    else if (key == LS("no_title"))       return tr("No title");
    else if (key == LS("title"))          return tr("Channel title");
    else if (key == LS("options"))        return tr("Options");
    else if (key == LS("visibility"))     return tr("Visibility in the channel list");
    else if (key == LS("default"))        return tr("Default");
    else if (key == LS("always_visible")) return tr("Always visible");
    else if (key == LS("hidden"))         return tr("Hidden");
    else if (key == LS("pin"))            return tr("Pin to channel list");
    else if (key == LS("permissions"))    return tr("Permissions");
    else if (key == LS("owner"))          return tr("Owner");
    else if (key == LS("readonly"))       return tr("Read only");
    else if (key == LS("logging"))        return tr("Logging messages");
    return QString();
  }
};


ChannelsPluginImpl::ChannelsPluginImpl(QObject *parent)
  : ChatPlugin(parent)
  , m_list(0)
{
  m_tr = new ChannelsTr();
  new ChannelsCmd(this);
  new ChannelsMenuImpl(this);

  connect(ChatClient::i(), SIGNAL(ready()), SLOT(ready()));
  connect(ChatClient::channels(), SIGNAL(channel(QByteArray)), SLOT(channel(QByteArray)));

  ChatCore::translation()->addOther(LS("channels"));
  ChatCore::settings()->setDefault(SETTINGS_CHANNELS_IGNORING, false);

  connect(ChatViewHooks::i(), SIGNAL(initHook(ChatView*)), SLOT(init(ChatView*)));
  connect(ChatViewHooks::i(), SIGNAL(loadFinishedHook(ChatView*)), SLOT(loadFinished(ChatView*)));

  QTimer::singleShot(0, this, SLOT(start()));
}


ChannelsPluginImpl::~ChannelsPluginImpl()
{
  delete m_tr;
}


/*!
 * Игнорирование пользователя.
 */
void ChannelsPluginImpl::ignore(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return;

  ClientFeeds::post(ChatClient::id(), LS("acl/head/other/") + SimpleID::encode(id), Acl::Read, Feed::Share | Feed::Broadcast);
}


/*!
 * Показ списка каналов.
 */
void ChannelsPluginImpl::show()
{
  TabWidget::i()->tab(LIST_TAB);
}


/*!
 * Отмена игнорирования пользователя.
 */
void ChannelsPluginImpl::unignore(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return;

  ClientFeeds::del(ChatClient::id(), LS("acl/head/other/") + SimpleID::encode(id), Feed::Share | Feed::Broadcast);
}


void ChannelsPluginImpl::channel(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::ChannelId)
    return;

  getAcl(ChatClient::channels()->get(id));
}


/*!
 * Инициализация JavaScript.
 */
void ChannelsPluginImpl::init(ChatView *view)
{
  if (SimpleID::typeOf(view->id()) != SimpleID::ChannelId)
    return;

  view->addJS(LS("qrc:/js/Channels/bootstrap-dropdown.js"));
  view->addJS(LS("qrc:/js/Channels/Channels.js"));
}


/*!
 * Инициализация CSS.
 */
void ChannelsPluginImpl::loadFinished(ChatView *view)
{
  if (SimpleID::typeOf(view->id()) != SimpleID::ChannelId)
    return;

  view->addCSS(LS("qrc:/css/Channels/bootstrap-dropdown.css"));
  view->addCSS(LS("qrc:/css/Channels/Channels.css"));
}


void ChannelsPluginImpl::ready()
{
  getAcl(ChatClient::channel());
  getAcl(ChatClient::server());
}


void ChannelsPluginImpl::showMenu(QMenu *menu, QAction *separator)
{
  Q_UNUSED(separator)

  m_list->setText(tr("Channels"));
  m_list->setChecked(TabWidget::isCurrent(TabWidget::i()->tab(LIST_TAB, TabWidget::NoOptions)));

  menu->addAction(m_list);
}


void ChannelsPluginImpl::start()
{
  TabWidget *tabs = TabWidget::i();
  if (!tabs)
    return;

  m_list = new QAction(this);
  m_list->setIcon(QIcon(LS(":/images/Channels/list.png")));
  m_list->setCheckable(true);

  tabs->add(new ListTabCreator());

  connect(tabs->toolBar(), SIGNAL(showMenu(QMenu*,QAction*)), SLOT(showMenu(QMenu*,QAction*)));
  connect(m_list, SIGNAL(triggered(bool)), SLOT(list()));
}


/*!
 * Загрузка фида \b acl если он не был получен.
 */
void ChannelsPluginImpl::getAcl(ClientChannel channel)
{
  if (!channel)
    return;

  FeedPtr feed = channel->feed(FEED_NAME_ACL, false);
  if (!feed || channel->type() == SimpleID::ServerId)
    ClientFeeds::request(channel, FEED_METHOD_GET, FEED_NAME_ACL);
}


ChatPlugin *ChannelsPlugin::create()
{
  m_plugin = new ChannelsPluginImpl(this);
  return m_plugin;
}


QWidget *ChannelsPlugin::settings(QWidget *parent)
{
  return new ChannelsSettings(parent);
}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(Channels, ChannelsPlugin);
#endif
