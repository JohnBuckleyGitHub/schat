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

#include "AuthCore.h"
#include "oauth2/OAuthData.h"
#include "Settings.h"
#include "sglobal.h"

OAuthData::OAuthData(const QByteArray &provider)
  : htmlName(provider)
  , name(provider)
  , provider(provider)
  , type("oauth2")
{
}


bool OAuthData::isValid() const
{
  if (provider.isEmpty() || id.isEmpty() || secret.isEmpty() || name.isEmpty() || htmlName.isEmpty())
    return false;

  return true;
}


bool OAuthData::read()
{
  Settings *settings = AuthCore::settings();
  id       = settings->value(provider + LS("/Id")).toByteArray();
  secret   = settings->value(provider + LS("/Secret")).toByteArray();
  redirect = settings->value(provider + LS("/Redirect")).toByteArray();

  qDebug() << toUrl();
  qDebug() << id << secret << redirect;

  return isValid();
}


QVariantMap OAuthData::toJSON() const
{
  QVariantMap data;
  data[LS("name")]     = name;
  data[LS("htmlName")] = htmlName;
  data[LS("url")]      = toUrl();
  data[LS("type")]     = type;
  return data;
}
