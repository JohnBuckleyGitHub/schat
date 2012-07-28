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

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "AuthCore.h"
#include "AuthCore.h"
#include "AuthHandler.h"
#include "AuthState.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "oauth2/GoogleAuth.h"
#include "oauth2/OAuthData.h"
#include "sglobal.h"
#include "Tufao/httpserverresponse.h"

GoogleAuth::GoogleAuth(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
  : OAuthHandler(LS("google"), url.queryItemValue(LS("state")).toLatin1(), url, path, request, response, parent)
{
}


/*!
 * Слот вызывается, когда завершено получение информации о пользователе.
 */
void GoogleAuth::dataReady()
{
  OAUTH_PREPARE_REPLY
  OAUTH_BAD_STATUS

  QVariantMap data = JSON::parse(raw).toMap();
  QByteArray uid = data.value(LS("id")).toByteArray();
  if (uid.isEmpty())
    return setError("invalid_uid");

  User user;
  user.name     = data.value(LS("name")).toString();
  user.email    = data.value(LS("email")).toString();
  user.link     = data.value(LS("link")).toString();
  user.birthday = data.value(LS("birthday")).toString();

  QByteArray id = SimpleID::encode(SimpleID::make("google:" + uid, SimpleID::UserId));
  AuthCore::state()->add(new AuthStateData(m_state, "google", id, data, user));

  log(NodeLog::InfoLevel, "Data is successfully received, id:" + id + ", uid:" + uid);
  deleteLater();
}


/*!
 * Слот вызывается, когда завершено получение токена авторизации.
 */
void GoogleAuth::tokenReady()
{
  m_reply = qobject_cast<QNetworkReply*>(sender());
  if (!m_reply)
    return;

  if (m_reply->error() != 0 && m_reply->error() != QNetworkReply::UnknownContentError)
    return setError("network_error: " + m_reply->errorString().toUtf8());

  QByteArray raw = m_reply->readAll();
  int status = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  m_reply->deleteLater();
  m_reply = 0;

  if (status >= 500) {
    log(NodeLog::WarnLevel, "Bad status code: " + QByteArray::number(status));
    getToken();
    return;
  }

  QVariantMap data = JSON::parse(raw).toMap();
  QByteArray token = data.value(LS("access_token")).toByteArray();
  if (token.isEmpty())
    return setError("token_error: " + data.value(LS("error")).toByteArray());

  log(NodeLog::InfoLevel, "Token is successfully received");

  QNetworkRequest request(QUrl(LS("https://www.googleapis.com/oauth2/v1/userinfo")));
  request.setRawHeader("Authorization", "Bearer " + token);
  QNetworkReply *reply = m_manager->get(request);
  connect(reply, SIGNAL(finished()), SLOT(dataReady()));
}


/*!
 * Запрос обмена авторизационного кода на токен.
 */
void GoogleAuth::getToken()
{
  QNetworkRequest request(QUrl(LS("https://accounts.google.com/o/oauth2/token")));
  request.setHeader(QNetworkRequest::ContentTypeHeader, LS("application/x-www-form-urlencoded"));

  QByteArray body = "code=" + m_code;
  body += "&client_id="     + m_provider->id;
  body += "&client_secret=" + m_provider->secret;
  body += "&redirect_uri="  + m_provider->redirect;
  body += "&grant_type=authorization_code";

  QNetworkReply *reply = m_manager->post(request, body);
  connect(reply, SIGNAL(finished()), SLOT(tokenReady()));
}


bool GoogleAuthCreator::serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
{
  if (path == LS("/oauth2/google")) {
    new GoogleAuth(url, path, request, response, parent);
    return true;
  }

  return false;
}
