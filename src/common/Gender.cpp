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

#include "Gender.h"

quint8 Gender::color() const
{
  if (m_gender >= Unknown)
    return Default;

  quint8 out = m_gender;
  if (value() == Female)
    out -= Female;

  if (out > Thief)
    return Default;

  return out;
}


quint8 Gender::value() const
{
  if (m_gender >= Unknown && m_gender <= Bot)
    return m_gender;

  if (m_gender > Bot)
    return Unknown;

  if (m_gender < Female)
    return Male;

  return Female;
}


/*!
 * Установка пола.
 */
void Gender::set(Genders gender)
{
  if (gender >= Unknown)
    m_gender = gender;
  else
    m_gender = gender + color();
}


void Gender::setColor(Colors color)
{
  if (m_gender >= Unknown)
    return;

  m_gender = value() + color;
}


void Gender::setColor(quint8 color)
{
  if (color > Thief)
    color = Default;

  setColor(static_cast<Colors>(color));
}


QString Gender::colorToString(quint8 color)
{
  switch (color) {
    case Black:
      return "black";

    case Gray:
      return "gray";

    case Green:
      return "green";

    case Red:
      return "red";

    case White:
      return "white";

    case Yellow:
      return "yellow";

    case Medical:
      return "medical";

    case Nude:
      return "nude";

    case Thief:
      return "thief";

    default:
      return "default";
      break;
  }
}


Gender& Gender::operator=(int other)
{
  m_gender = other;
  return *this;
}
