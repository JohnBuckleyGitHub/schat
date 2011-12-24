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

#ifndef NODEPLUGIN_H_
#define NODEPLUGIN_H_

#include <QObject>

#include "plugins/HookResult.h"
#include "plugins/NodeHooks.h"
#include "schat.h"

class Core;

class SCHAT_EXPORT NodePlugin : public QObject
{
  Q_OBJECT

public:
  NodePlugin(Core *core);
  virtual HookResult hook(const NodeHook & /*data*/) { return HookResult(); }
  virtual QList<NodeHook::Type> hooks() const { return QList<NodeHook::Type>(); }
  virtual void reload() {}

protected:
  Core *m_core;
};

#endif /* NODEPLUGIN_H_ */
