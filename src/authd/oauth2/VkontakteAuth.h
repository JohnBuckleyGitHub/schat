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

#ifndef VKONTAKTEAUTH_H_
#define VKONTAKTEAUTH_H_

#include "oauth2/OAuthHandler.h"

class VkontakteAuth : public OAuthHandler
{
  Q_OBJECT

public:
  VkontakteAuth(const QByteArray &state, const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent = 0);

private slots:
  void dataReady();
  void tokenReady();

private:
  void getToken();
};


class VkontakteAuthCreator : public HandlerCreator
{
public:
  VkontakteAuthCreator() : HandlerCreator() {}
  bool serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent);
};

#endif /* VKONTAKTEAUTH_H_ */
