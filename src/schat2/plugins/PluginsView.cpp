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

#include <QApplication>
#include <QFile>
#include <QTimer>
#include <QWebFrame>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "ChatPlugins.h"
#include "ChatSettings.h"
#include "plugins/PluginsView.h"
#include "sglobal.h"
#include "Translation.h"
#include "WebBridge.h"

PluginsView::PluginsView(QWidget *parent)
  : QWebView(parent)
{
  setAcceptDrops(false);
  page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

  connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(populateJavaScriptWindowObject()));

  QTimer::singleShot(0, this, SLOT(boot()));
}


/*!
 * Состояние плагина.
 *
 * \return - 0 - Состояние плагина не будет изменено после перезапуска.
 *         - 1 - Плагин будет включен после перезапуска чата.
 *         - 2 - Плагин будет отключен после перезапуска чата.
 */
int PluginsView::state(const QString &id)
{
  PluginItem *item = ChatCore::plugins()->plugin(id);
  if (!item)
    return 0;

  bool enabled = ChatCore::settings()->value(LS("Plugins/") + id).toBool();
  if (item->isLoaded() == enabled)
    return 0;

  if (enabled && !item->isLoaded())
    return 1;

  if (!enabled && item->isLoaded())
    return 2;

  return 0;
}


QVariantList PluginsView::list() const
{
  QVariantList plugins;
  foreach (PluginItem *item, ChatCore::plugins()->list()) {
    QVariantMap data;
    data[LS("id")]      = item->id();
    data[LS("icon")]    = LS("qrc") + item->icon();
    data[LS("title")]   = item->header().value(LS("Name"));
    data[LS("version")] = item->header().value(LS("Version"));
    data[LS("desc")]    = desc(item->header());
    data[LS("enabled")] = ChatCore::settings()->value(LS("Plugins/") + item->id()).toBool();
    plugins.append(data);
  }

  return plugins;
}


void PluginsView::enable(const QString &id, bool enable)
{
  ChatCore::settings()->setValue(LS("Plugins/") + id, enable);
}


void PluginsView::restart()
{
  ChatNotify::start(Notify::Restart);
}


void PluginsView::boot()
{
  QString file = QApplication::applicationDirPath() + LS("/styles/test/html/Plugins.html");
  if (QFile::exists(file))
    file = QUrl::fromLocalFile(file).toString();
  else
    file = LS("qrc:/html/Plugins.html");

  setUrl(QUrl(file));
}


void PluginsView::populateJavaScriptWindowObject()
{
  page()->mainFrame()->addToJavaScriptWindowObject(LS("SimpleChat"), WebBridge::i());
  page()->mainFrame()->addToJavaScriptWindowObject(LS("PluginsView"), this);
}


QString PluginsView::desc(const QVariantMap &data) const
{
  QString desc = data.value(LS("Desc/") + ChatCore::translation()->name()).toString();
  if (!desc.isEmpty())
    return desc;

  return data.value(LS("Desc")).toString();
}
