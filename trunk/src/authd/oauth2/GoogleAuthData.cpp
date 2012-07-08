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

#include <QDebug>

#include <QUrl>

#include "AuthCore.h"
#include "oauth2/GoogleAuthData.h"
#include "Settings.h"
#include "sglobal.h"

GoogleAuthData::GoogleAuthData()
  : OAuthData(LS("google"))
{
}


bool GoogleAuthData::read()
{
  Settings *settings = AuthCore::settings();
  m_id       = settings->value(LS("Google/Id")).toByteArray();
  m_secret   = settings->value(LS("Google/Secret")).toByteArray();
  m_redirect = settings->value(LS("Google/Redirect")).toByteArray();

  qDebug() << toUrl();
  qDebug() << m_id << m_secret << m_redirect;

  return isValid();
}


QByteArray GoogleAuthData::toUrl() const
{
  QByteArray url = "https://accounts.google.com/o/oauth2/auth?scope=https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.email+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.profile";
  url += "&redirect_uri=" + QUrl::toPercentEncoding(m_redirect);
  url += "&response_type=code&client_id=" + m_id + "&approval_prompt=force";
  return url;
}
