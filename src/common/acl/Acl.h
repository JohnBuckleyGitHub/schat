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

#ifndef ACL_H_
#define ACL_H_

#include "User.h"

class SCHAT_EXPORT Acl
{
public:
  /// Результирующий список прав.
  enum ResultAcl {
    Read  = 04,
    Write = 02,
    Edit  = 1
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

  Acl(int acl = 0666);

  inline const Groups& groups() const { return m_groups; }
  inline Groups& groups() { return m_groups; }
  inline int acl() const { return m_acl; }
  inline void setAcl(int acl) { m_acl = acl; }
  int match(ClientUser user) const;

private:
  Groups m_groups; ///< Группы.
  int m_acl;       ///< Права доступа Acl:CommonAcl.
};

#endif /* ACL_H_ */
