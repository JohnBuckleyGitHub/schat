/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CHATPLUGIN_H_
#define CHATPLUGIN_H_

#include <QObject>

#include "schat.h"

class ChatCore;

class SCHAT_CORE_EXPORT ChatPlugin : public QObject
{
  Q_OBJECT

public:
  ChatPlugin(ChatCore *core);

public slots:
  virtual void notify(int /*notice*/, const QVariant & /*data*/) {}
  virtual void settingsChanged(const QString & /*key*/, const QVariant & /*value*/) {}

protected:
  ChatCore *m_core;
};

#endif /* CHATPLUGIN_H_ */
