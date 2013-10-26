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

#include <QUrl>

#include "oauth2/google/GoogleAuthData.h"
#include "sglobal.h"

GoogleAuthData::GoogleAuthData()
  : OAuthData("google")
{
  name = "Google";
  htmlName = "<span style='color:#0039b6'>G</span><span style='color:#c41200'>o</span><span style='color:#f7a70b'>o</span><span style='color:#0039b6'>g</span><span style='color:#30a72f'>l</span><span style='color:#c41200'>e</span>";
}


QByteArray GoogleAuthData::toUrl(const QByteArray &state) const
{
  QByteArray url = "https://accounts.google.com/o/oauth2/auth?scope=https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.email+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.profile";
  url += "&redirect_uri=" + QUrl::toPercentEncoding(redirect);
  url += "&response_type=code&client_id=" + id + "&approval_prompt=force";
  url += "&state=" + state;
  return url;
}
