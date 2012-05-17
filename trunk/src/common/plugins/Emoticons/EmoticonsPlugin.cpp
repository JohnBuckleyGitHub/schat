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

#include <QDebug>

#include <QtPlugin>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "EmoticonsExtension.h"
#include "EmoticonsPlugin.h"
#include "EmoticonsPlugin_p.h"
#include "Extensions.h"
#include "sglobal.h"
#include "Emoticons.h"

EmoticonsPluginImpl::EmoticonsPluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  m_emoticons = new Emoticons(this);

  ChatCore::settings()->setDefault(LS("Emoticons"), QStringList(LS("kolobok")));
  ChatCore::extensions()->addFactory(new EmoticonsExtensionFactory());

  connect(ChatCore::extensions(), SIGNAL(loaded()), SLOT(loaded()));
  connect(ChatCore::extensions(), SIGNAL(installed(QString)), SLOT(installed(QString)));
}


void EmoticonsPluginImpl::installed(const QString &key)
{
  if (!key.startsWith(LS("emoticons/")))
    return;

  m_emoticons->load(ChatCore::extensions()->get(key));
}


void EmoticonsPluginImpl::loaded()
{
  QStringList emoticons = ChatCore::settings()->value(LS("Emoticons")).toStringList();
  foreach (const QString &name, emoticons) {
    ChatCore::extensions()->install(LS("emoticons/") + name);
  }
}


ChatPlugin *EmoticonsPlugin::create()
{
  if (!ChatCore::config().contains(LS("EXTENSIONS")))
    return 0;

  m_plugin = new EmoticonsPluginImpl(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(Emoticons, EmoticonsPlugin);
