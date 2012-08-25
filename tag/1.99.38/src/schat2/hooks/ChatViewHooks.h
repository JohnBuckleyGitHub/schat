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

#ifndef CHATVIEWHOOKS_H_
#define CHATVIEWHOOKS_H_

#include <QObject>

#include "schat.h"

class ChatView;

class SCHAT_CORE_EXPORT ChatViewHooks : public QObject
{
  Q_OBJECT

public:
  ChatViewHooks(QObject *parent = 0);
  ~ChatViewHooks();
  inline const QList<ChatView *>& views() const   { return m_views; }
  inline static ChatViewHooks *i()                { return m_self; }
  inline static void add(ChatView *view)          { m_self->addImpl(view); }
  inline static void add(ChatViewHooks *hook)     { if (!m_self->m_hooks.contains(hook)) m_self->m_hooks.append(hook); }
  inline static void init(ChatView *view)         { m_self->initImpl(view); }
  inline static void loadFinished(ChatView *view) { m_self->loadFinishedImpl(view); }
  inline static void remove(ChatView *view)       { m_self->removeImpl(view); }
  inline static void remove(ChatViewHooks *hook)  { m_self->m_hooks.removeAll(hook); }

signals:
  void addHook(ChatView *view);
  void initHook(ChatView *view);
  void loadFinishedHook(ChatView *view);
  void removeHook(ChatView *view);

protected:
  virtual void addImpl(ChatView *view);
  virtual void initImpl(ChatView *view);
  virtual void loadFinishedImpl(ChatView *view);
  virtual void removeImpl(ChatView *view);

  QList<ChatView *> m_views;     ///< Список окон.

private:
  QList<ChatViewHooks*> m_hooks; ///< Хуки.
  static ChatViewHooks *m_self;  ///< Указатель на себя.
};

#endif /* CHATVIEWHOOKS_H_ */
