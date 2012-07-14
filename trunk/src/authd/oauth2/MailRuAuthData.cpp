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

#include "oauth2/MailRuAuthData.h"

MailRuAuthData::MailRuAuthData()
  : OAuthData("mail_ru")
{
  name = "Mail.Ru";
  htmlName = "<span style='color:#f7a600'>@</span>mail<span style='color:#f7a600'>.ru</span>";
}


/*!
 * \todo Отсутствует реализация создания адреса для провайдера mail_ru.
 */
QByteArray MailRuAuthData::toUrl(const QByteArray &state) const
{
  Q_UNUSED(state)
  return QByteArray();
}
