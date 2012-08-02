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

#ifndef SPELLCHECKERPLUGIN_P_H_
#define SPELLCHECKERPLUGIN_P_H_

#include <QDebug>

#include "SpellChecker.h"

#include "plugins/ChatPlugin.h"
#include "ui/tabs/SettingsTabHook.h"

class SpellChecker;

class SpellCheckerPluginImpl : public ChatPlugin
{
  Q_OBJECT

public:
  SpellCheckerPluginImpl(QObject *parent = 0);

public slots:
//  void settingsChanged(const QString &key, const QVariant &value);
  void start();

private:
  SpellChecker *m_spellchecker;

};

#endif /* SPELLCHECKERPLUGIN_P_H_ */
