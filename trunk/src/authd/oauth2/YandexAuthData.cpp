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

#include "oauth2/YandexAuthData.h"

YandexAuthData::YandexAuthData()
  : OAuthData("yandex")
{
  name = "Яндекс";
  htmlName = "<b style='color:#ff000a'>Я</b>ндекс";
}


QByteArray YandexAuthData::toUrl() const
{
  return "https://oauth.yandex.ru/authorize?response_type=code&client_id=" + id;
}
