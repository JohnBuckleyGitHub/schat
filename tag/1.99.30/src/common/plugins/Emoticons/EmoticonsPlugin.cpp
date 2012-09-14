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

#include <QtPlugin>
#include <QDesktopServices>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "ChatSettings.h"
#include "Emoticons.h"
#include "EmoticonsExtension.h"
#include "EmoticonsFilter.h"
#include "EmoticonsPlugin.h"
#include "EmoticonsPlugin_p.h"
#include "Extensions.h"
#include "net/SimpleID.h"
#include "sglobal.h"

EmoticonsPluginImpl::EmoticonsPluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  m_emoticons = new Emoticons(this);

  ChatCore::settings()->setDefault(LS("Emoticons"), QStringList(LS("kolobok")));
  ChatCore::extensions()->addFactory(new EmoticonsExtensionFactory());

  TokenFilter::add(LS("channel"), new EmoticonsFilter(m_emoticons));
  TokenFilter::add(LS("input"), new EmoticonsInputFilter());
  QDesktopServices::setUrlHandler(LS("emoticon"), this, "openUrl");

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


void EmoticonsPluginImpl::openUrl(const QUrl &url)
{
  ChatNotify::start(Notify::InsertText, QChar(QChar::Nbsp) + QString(SimpleID::fromBase32(url.path().toLatin1())) + QChar(QChar::Nbsp));
}


ChatPlugin *EmoticonsPlugin::create()
{
  if (!ChatCore::config().contains(LS("EXTENSIONS")))
    return 0;

  m_plugin = new EmoticonsPluginImpl(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(Emoticons, EmoticonsPlugin);