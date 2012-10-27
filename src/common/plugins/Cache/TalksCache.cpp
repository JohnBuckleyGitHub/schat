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

#include <QTimer>
#include <QMenu>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "TalksCache.h"
#include "ui/ChatIcons.h"
#include "ui/TabsToolBar.h"
#include "ui/TabWidget.h"

TalksCache::TalksCache(QObject *parent)
  : QObject(parent)
  , m_settings(ChatCore::settings())
  , m_clear(0)
  , m_menu(0)
{
  m_settings->setDefault(LS("RecentTalks"),    QStringList());
  m_settings->setDefault(LS("MaxRecentTalks"), 20);

  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
  connect(m_settings, SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(m_settings, SIGNAL(synced()), SLOT(synced()));

  QTimer::singleShot(0, this, SLOT(start()));
}


void TalksCache::notify(const Notify &notify)
{
  if (notify.type() == Notify::ChannelTabClosed) {
    QByteArray id = notify.data().toByteArray();
    m_channels.removeAll(id);
    m_channels.prepend(id);
    m_settings->setValue(LS("RecentTalks"), save());
  }
  else if (notify.type() == Notify::Language && m_menu) {
    m_menu->setTitle(tr("Recent"));
    m_clear->setText(tr("Clear"));
  }
}


void TalksCache::settingsChanged(const QString &key, const QVariant &value)
{
  Q_UNUSED(value)
  if (key == LS("RecentTalks") || key == LS("MaxRecentTalks"))
    m_channels = channels();
}


/*!
 * Формирование меню со списком недавних разговоров.
 */
void TalksCache::showMenu(QMenu *menu, QAction *separator)
{
  if (m_channels.isEmpty())
    return;

  m_menu->clear();

  ClientChannels *channels = ChatClient::channels();
  TabWidget *tabs = TabWidget::i();

  for (int i = 0; i < m_channels.size(); ++i) {
    ClientChannel channel = channels->get(m_channels.at(i));
    if (!channel)
      continue;

    if (tabs->channelTab(channel->id(), false, false))
      continue;

    QAction *action = m_menu->addAction(ChatIcons::icon(channel), channel->name());
    action->setData(channel->id());
  }

  if (m_menu->isEmpty())
    return;

  m_menu->addSeparator();
  m_menu->addAction(m_clear);
  menu->insertMenu(separator, m_menu);
}


void TalksCache::start()
{
  if (!TabWidget::i())
    return;

  TabsToolBar *toolBar = TabWidget::i()->toolBar();
  m_menu = new QMenu(tr("Recent"), toolBar);
  m_menu->setIcon(QIcon(LS(":/images/Cache/clock.png")));
  m_clear = new QAction(this);
  m_clear->setText(tr("Clear"));
  m_clear->setIcon(SCHAT_ICON(EditClear));

  connect(m_menu, SIGNAL(triggered(QAction*)), SLOT(triggered(QAction*)));
  connect(toolBar, SIGNAL(showMenu(QMenu*,QAction*)), SLOT(showMenu(QMenu*,QAction*)));
}


void TalksCache::synced()
{
  m_channels = channels();
  ClientChannels *channels = ChatClient::channels();
  QList<QByteArray> unsynced;

  foreach (const QByteArray &id, m_channels) {
    ClientChannel channel = channels->get(id);
    if (!channel || !channel->isSynced())
      unsynced.append(id);
  }

  channels->info(unsynced);
}


void TalksCache::triggered(QAction *action)
{
  if (action == m_clear) {
    m_channels.clear();
    m_settings->setValue(LS("RecentTalks"), QStringList());
    return;
  }

  QByteArray id = action->data().toByteArray();
  TabWidget::i()->channelTab(id);

  if (SimpleID::typeOf(id) == SimpleID::ChannelId)
    ChatClient::channels()->join(id);
}


/*!
 * Получение списка каналов.
 */
QList<QByteArray> TalksCache::channels() const
{
  QStringList recents = m_settings->value(LS("RecentTalks")).toStringList().mid(0, m_settings->value(LS("MaxRecentTalks")).toInt());
  int type = 0;
  QList<QByteArray> list;

  foreach (const QString &text, recents) {
    QByteArray id = SimpleID::decode(text);
    type = SimpleID::typeOf(id);

    if ((type == SimpleID::ChannelId || type == SimpleID::UserId) && !list.contains(id))
      list.append(id);
  }

  return list;
}


/*!
 * Сохранение списка каналов.
 */
QStringList TalksCache::save() const
{
  QStringList recent;
  foreach (const QByteArray &id, m_channels)
    recent.append(SimpleID::encode(id));

  return recent;
}
