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

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "AuthCore.h"
#include "AuthHandler.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "oauth2/GoogleAuth.h"
#include "oauth2/OAuthData.h"
#include "sglobal.h"
#include "Tufao/httpserverresponse.h"
#include "AuthCore.h"
#include "AuthState.h"

GoogleAuth::GoogleAuth(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
  : OAuthHandler(url, path, request, response, parent)
  , m_reply(0)
{
  m_provider = AuthCore::provider(LS("google"));
  if (!m_provider) {
    AuthHandler::setError(response, Tufao::HttpServerResponse::INTERNAL_SERVER_ERROR);
    return;
  }

  if (url.hasQueryItem(LS("error")) || !url.hasQueryItem(LS("code"))) {
    serveError();
    return;
  }

  setState(url.queryItemValue(LS("state")).toLatin1());
  serveOk();

  m_manager = new QNetworkAccessManager(this);
  m_code = url.queryItemValue(LS("code")).toUtf8();
  getToken();
}


/*!
 * Слот вызывается, когда завершено получение информации о пользователе.
 */
void GoogleAuth::dataReady()
{
  m_reply = qobject_cast<QNetworkReply*>(sender());
  if (!m_reply)
    return;

  if (m_reply->error())
    return setError("network_error: " + m_reply->errorString().toUtf8());

  QVariantMap data = JSON::parse(m_reply->readAll()).toMap();
  m_reply->deleteLater();

  QByteArray email = data.value(LS("email")).toByteArray();
  if (email.isEmpty())
    return setError();

  QByteArray id = SimpleID::encode(SimpleID::make("google:" + email, SimpleID::UserId));
  AuthCore::state()->add(new AuthStateData(m_state, "google", id, QByteArray(), data));

  qDebug() << email;
}


/*!
 * Слот вызывается, когда завершено получение токена авторизации.
 */
void GoogleAuth::tokenReady()
{
  m_reply = qobject_cast<QNetworkReply*>(sender());
  if (!m_reply)
    return;

  if (m_reply->error())
    return setError("network_error: " + m_reply->errorString().toUtf8());

  QByteArray raw = m_reply->readAll();
  int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  m_reply->deleteLater();

  qDebug() << statusCode << raw;

  if (statusCode >= 500) {
    getToken();
    return;
  }

  QVariantMap data = JSON::parse(raw).toMap();
  QByteArray token = data.value(LS("access_token")).toByteArray();
  if (token.isEmpty())
    return setError("token_error: " + data.value(LS("error")).toByteArray());


  QNetworkRequest request(QUrl(LS("https://www.googleapis.com/oauth2/v1/userinfo")));
  request.setRawHeader("Authorization", "Bearer " + token);
  QNetworkReply *reply = m_manager->get(request);
  connect(reply, SIGNAL(readyRead()), SLOT(dataReady()));
}


/*!
 * Запрос обмена авторизационного кода на токен.
 */
void GoogleAuth::getToken()
{
  QNetworkRequest request(QUrl(LS("https://accounts.google.com/o/oauth2/token")));
  request.setHeader(QNetworkRequest::ContentTypeHeader, LS("application/x-www-form-urlencoded"));

  QByteArray body = "code=" + m_code;
  body += "&client_id="     + m_provider->id();
  body += "&client_secret=" + m_provider->secret();
  body += "&redirect_uri="  + m_provider->redirect();
  body += "&grant_type=authorization_code";

  QNetworkReply *reply = m_manager->post(request, body);
  connect(reply, SIGNAL(readyRead()), SLOT(tokenReady()));
}


/*!
 * Установка состояния ошибки.
 */
void GoogleAuth::setError(const QByteArray &error)
{
  if (m_reply) {
    m_reply->deleteLater();
    m_reply = 0;
  }

  qDebug() << "GoogleAuth::setError()" << error;
  AuthCore::state()->add(new AuthStateData(m_state, error));
}


bool GoogleAuthCreator::serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
{
  if (path == LS("/oauth2/google")) {
    new GoogleAuth(url, path, request, response, parent);
    return true;
  }

  return false;
}
