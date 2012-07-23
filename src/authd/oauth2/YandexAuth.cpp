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
#include <QNetworkReply>

#include "AuthCore.h"
#include "AuthState.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "oauth2/OAuthData.h"
#include "oauth2/YandexAuth.h"
#include "sglobal.h"

YandexAuth::YandexAuth(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
  : OAuthHandler(LS("yandex"), url, path, request, response, parent)
{
  if (!m_provider)
    return;

  if (url.hasQueryItem(LS("error")) || !url.hasQueryItem(LS("code"))) {
    serveError();
    return;
  }

  setState(url.queryItemValue(LS("state")).toLatin1());
  serveOk();

  m_manager = new QNetworkAccessManager(this);
  m_code = url.queryItemValue(LS("code")).toUtf8();
  log(NodeLog::InfoLevel, "Start receiving token, code:" + m_code);
  getToken();
}


void YandexAuth::dataReady()
{
  OAUTH_PREPARE_REPLY;
  OAUTH_BAD_STATUS

  QVariantMap data = JSON::parse(raw).toMap();
  QByteArray email = data.value(LS("default_email")).toByteArray();
  if (email.isEmpty())
    return setError("invalid_email");

  QByteArray id = SimpleID::encode(SimpleID::make("yandex:" + email, SimpleID::UserId));
  AuthCore::state()->add(new AuthStateData(m_state, "yandex", id, data));

  log(NodeLog::InfoLevel, "Data is successfully received, id:" + id + ", email:" + email);
}


void YandexAuth::tokenReady()
{
  OAUTH_PREPARE_REPLY
  Q_UNUSED(status)

  QVariantMap data = JSON::parse(raw).toMap();
  QByteArray token = data.value(LS("access_token")).toByteArray();
  if (token.isEmpty())
    return setError("token_error: " + data.value(LS("error")).toByteArray());

  log(NodeLog::InfoLevel, "Token is successfully received");

  QNetworkRequest request(QUrl(LS("https://login.yandex.ru/info?format=json")));
  request.setRawHeader("Authorization", "OAuth " + token);
  QNetworkReply *reply = m_manager->get(request);
  connect(reply, SIGNAL(finished()), SLOT(dataReady()));
}


void YandexAuth::getToken()
{
  QNetworkRequest request(QUrl(LS("https://oauth.yandex.ru/token")));
  request.setHeader(QNetworkRequest::ContentTypeHeader, LS("application/x-www-form-urlencoded"));

  QByteArray body = "grant_type=authorization_code";
  body += "&code="          + m_code;
  body += "&client_id="     + m_provider->id;
  body += "&client_secret=" + m_provider->secret;

  QNetworkReply *reply = m_manager->post(request, body);
  connect(reply, SIGNAL(finished()), SLOT(tokenReady()));
}


bool YandexAuthCreator::serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
{
  if (path == LS("/oauth2/yandex")) {
    new YandexAuth(url, path, request, response, parent);
    return true;
  }

  return false;
}
