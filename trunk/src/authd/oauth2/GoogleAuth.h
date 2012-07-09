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

#ifndef GOOGLEAUTH_H_
#define GOOGLEAUTH_H_

#include "oauth2/OAuthHandler.h"

class OAuthData;
class QNetworkAccessManager;
class QNetworkReply;

class GoogleAuth : public OAuthHandler
{
  Q_OBJECT

public:
  GoogleAuth(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent = 0);

private slots:
  void dataReady();
  void tokenReady();

private:
  void setError();

  OAuthData *m_provider;            ///< Информация о провайдере.
  QNetworkAccessManager *m_manager; ///< Менеджер доступа к сети.
  QNetworkReply *m_reply;           ///< Текущий завершённый HTTP запрос.
};


class GoogleAuthCreator : public HandlerCreator
{
public:
  GoogleAuthCreator() : HandlerCreator() {}
  bool serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent);
};


#endif /* GOOGLEAUTH_H_ */
