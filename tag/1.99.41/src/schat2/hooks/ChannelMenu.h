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

#ifndef CHANNELMENU_H_
#define CHANNELMENU_H_

#include "actions/MenuBuilder.h"
#include "Channel.h"

namespace Hooks
{

class SCHAT_CORE_EXPORT ChannelMenu : public QObject
{
  Q_OBJECT

public:
  ChannelMenu(QObject *parent = 0);
  ~ChannelMenu();
  inline static void add(ChannelMenu *hook)                   { if (!m_self->m_hooks.contains(hook)) m_self->m_hooks.append(hook); }
  inline static void bind(QMenu *menu, ClientChannel channel) { m_self->bindImpl(menu, channel); }
  inline static void remove(ChannelMenu *hook)                { m_self->m_hooks.removeAll(hook); }
  static void bind(QMenu *menu, const QByteArray &id);
  static void bind(QMenu *menu, const QUrl &url);

protected slots:
  void cleanup();
  void triggered(QAction *action);

protected:
  virtual bool triggerImpl(QAction *action);
  virtual void bindImpl(QMenu *menu, ClientChannel channel);
  virtual void cleanupImpl();

  QList<ChannelMenu*> m_hooks; ///< Хуки.
  static ChannelMenu *m_self;  ///< Указатель на себя.
};

} // namespace Hooks

#endif /* CHANNELMENU_H_ */
