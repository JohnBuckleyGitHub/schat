/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 * Copyright © 2012 Alexey Ivanov <alexey.ivanes@gmail.com>
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
#include <QtPlugin>
#include <QTimer>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "SpellCheckerPlugin.h"
#include "SpellCheckerPlugin_p.h"
#include "SpellCheckerPage.h"
#include "sglobal.h"

SpellCheckerPluginImpl::SpellCheckerPluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  QTimer::singleShot(0, this, SLOT(start()));
}


void SpellCheckerPluginImpl::start()
{
  QStringList defaultDicts;
  defaultDicts.append("en_US");
  defaultDicts.append(QLocale().name().toUtf8().constData());

  ChatCore::settings()->setLocalDefault(LS("SpellChecker/EnabledDicts"), defaultDicts);

  SpellChecker::instance();
  SettingsTabHook::add(new SpellCheckerPageCreator(this));

}

ChatPlugin *SpellCheckerPlugin::create()
{
  m_plugin = new SpellCheckerPluginImpl(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(SpellChecker, SpellCheckerPlugin);
