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

#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "StateCache.h"
#include "ui/tabs/AbstractTab.h"
#include "ui/tabs/ChannelBaseTab.h"
#include "ui/tabs/ServerTab.h"
#include "ui/TabWidget.h"

StateCache::StateCache(QObject *parent)
  : QObject(parent)
  , m_settings(ChatCore::settings())
  , m_key(LS("PinnedTabs"))
{
  m_settings->setDefault(m_key, QStringList());
  connect(m_settings, SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(m_settings, SIGNAL(synced()), SLOT(synced()));

  QTimer::singleShot(0, this, SIGNAL(start()));
}


void StateCache::pinned(AbstractTab *tab)
{
  QString id = encode(tab->id());
  if (!m_tabs.contains(id)) {
    m_tabs.append(id);
    m_settings->setValue(m_key, m_tabs);
  }
}


void StateCache::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == m_key)
    m_tabs = value.toStringList();
}


void StateCache::start()
{
  TabWidget *tabs = TabWidget::i();
  if (tabs) {
    connect(tabs, SIGNAL(pinned(AbstractTab*)), SLOT(pinned(AbstractTab*)));
    connect(tabs, SIGNAL(unpinned(AbstractTab*)), SLOT(unpinned(AbstractTab*)));
  }
}


void StateCache::synced()
{
  m_tabs = m_settings->value(m_key).toStringList();

  TabWidget *tabs = TabWidget::i();
  if (!tabs)
    return;

  foreach (const QString &text, m_tabs) {
    QByteArray id = decode(text);
    if (Channel::isCompatibleId(id)) {
      ChannelBaseTab *tab = tabs->channelTab(id, false, false);
      if (tab && !(tab->options() & AbstractTab::Pinned))
        tab->pin();
      else if (!tab)
        join(id);
    }
  }
}


void StateCache::unpinned(AbstractTab *tab)
{
  QString id = encode(tab->id());
  if (m_tabs.contains(id)) {
    m_tabs.removeAll(id);
    m_settings->setValue(m_key, m_tabs);
  }
}


QByteArray StateCache::decode(const QString &id) const
{
  if (id.size() == 34)
    return SimpleID::decode(id);

  return id.toUtf8();
}


QString StateCache::encode(const QByteArray &id) const
{
  if (SimpleID::typeOf(id) != SimpleID::InvalidId)
    return SimpleID::encode(id);

  return id;
}


void StateCache::join(const QByteArray &id)
{
  TabWidget *tabs = TabWidget::i();

  if (SimpleID::typeOf(id) == SimpleID::ServerId) {
    ServerTab *serverTab = tabs->serverTab();
    if (tabs->indexOf(serverTab) == -1) {
      tabs->addTab(serverTab, QString());
      serverTab->setOnline();
      serverTab->setText(ChatClient::serverName());
    }

    serverTab->pin();
    return;
  }

  ChannelBaseTab *tab = TabWidget::i()->channelTab(id, true, false);
  if (tab)
    tab->pin();

  if (!tab || SimpleID::typeOf(id) == SimpleID::ChannelId)
    ChatClient::channels()->join(id);
}
