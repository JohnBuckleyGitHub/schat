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

#ifndef HOOKRESULT_H_
#define HOOKRESULT_H_

class HookResult
{
  enum Status {
    Ok,
    Error
  };

public:
  HookResult()
  : m_matches(0)
  , m_status(Ok)
  {}

  HookResult(int matches)
  : m_matches(matches)
  , m_status(Ok)
  {}

  inline int matches() const { return m_matches; }
  inline Status status() const { return m_status; }
  inline void setMatches(int matches) { m_matches = matches; }
  inline void setStatus(Status status) { m_status = status; }

protected:
  int m_matches;
  Status m_status;
};

#endif /* HOOKRESULT_H_ */
