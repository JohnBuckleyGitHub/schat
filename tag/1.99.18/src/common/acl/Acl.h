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

#ifndef ACL_H_
#define ACL_H_

#include <QHash>
#include <QVariant>

#include "acl/Groups.h"
#include "schat.h"

class Channel;

class SCHAT_EXPORT Acl
{
public:
  /// Результирующий список прав.
  enum ResultAcl {
    Read  = 04,
    Write = 02,
    Edit  = 01
  };

  /// Общий список прав, подобный UNIX правам доступа: владелец/группа/остальные.
  enum CommonAcl {
    UsersRead   = 0400, ///< владельцы имеют право на чтение.
    UsersWrite  = 0200, ///< владельцы имеют право на запись.
    UsersEdit   = 0100, ///< владельцы имеют право на редактирование свойств.
    GroupsRead  = 040,  ///< группы имеют право на чтение.
    GroupsWrite = 020,  ///< группы имеют право на запись.
    GroupsEdit  = 010,  ///< группы имеют право на редактирование свойств.
    OthersRead  = 04,   ///< остальные имеют право на чтение.
    OthersWrite = 02,   ///< остальные имеют право на запись.
    OthersEdit  = 01    ///< остальные имеют право на редактирование свойств.
  };

  Acl(int acl = 0766);

  bool can(Channel *channel, ResultAcl acl) const;
  inline const Groups& groups() const            { return m_groups; }
  inline const QList<QByteArray>& owners() const { return m_owners; }
  inline Groups& groups()                        { return m_groups; }
  inline int mask() const                        { return m_mask; }
  inline void remove(const QByteArray &owner)    { m_owners.removeAll(owner); }
  inline void setMask(int mask)                  { m_mask = mask; }
  int match(Channel *channel) const;
  QVariantMap get(Channel *channel);
  QVariantMap save();
  void add(const QByteArray &owner);
  void load(const QVariantMap &json);

private:
  Groups m_groups;                 ///< Группы.
  int m_mask;                      ///< Общая маска прав доступа Acl:CommonAcl.
  int m_math;                      ///< Права доступа для конкретного пользователя.
  QHash<QByteArray, int> m_others; ///< Специальные права доступа для выбранных пользователей.
  QList<QByteArray> m_owners;      ///< Идентификаторы владельцев.
};

#endif /* ACL_H_ */
