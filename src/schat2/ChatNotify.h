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

#ifndef CHATNOTIFY_H_
#define CHATNOTIFY_H_

#include <QObject>
#include <QVariant>

#include "schat.h"

class Notify
{
public:
  enum Actions {
    OpenChannel = 0x6F63 ///< "oc" Открытие канала, команда передаёт идентификатор канала.
  };

  Notify(int type, const QVariant &data = QVariant())
  : m_type(type)
  , m_data(data)
  {}

  inline const QVariant& data() const { return m_data; }
  inline int type() const { return m_type; }

private:
  int m_type;
  QVariant m_data;
};


class SCHAT_EXPORT ChatNotify : public QObject
{
  Q_OBJECT

public:
  ChatNotify(QObject *parent = 0);
  inline static ChatNotify *i() { return m_self; }
  inline static void start(const Notify &notify) { m_self->startNotify(notify); }

signals:
  void notify(const Notify &notify);

private:
  void startNotify(const Notify &notify);

  static ChatNotify *m_self; ///< Указатель на себя.
};

#endif /* CHATNOTIFY_H_ */
