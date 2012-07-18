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
#include <QNetworkRequest>
#include <QNetworkReply>

#include "AuthCore.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "oauth2/FacebookAuth.h"
#include "oauth2/OAuthData.h"
#include "sglobal.h"
#include "AuthState.h"

FacebookAuth::FacebookAuth(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
  : OAuthHandler(LS("facebook"), url, path, request, response, parent)
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


void FacebookAuth::dataReady()
{
  OAUTH_PREPARE_REPLY
  OAUTH_BAD_STATUS

  QVariantMap data = JSON::parse(raw).toMap();
  QByteArray email = data.value(LS("email")).toByteArray();
  if (email.isEmpty())
    return setError("invalid_email");

  QByteArray id = SimpleID::encode(SimpleID::make("facebook:" + email, SimpleID::UserId));
  AuthCore::state()->add(new AuthStateData(m_state, "facebook", id, data));

  log(NodeLog::InfoLevel, "Data is successfully received, id:" + id + ", email:" + email);
  deleteLater();
}


void FacebookAuth::tokenReady()
{
  OAUTH_PREPARE_REPLY

  if (status != 200) {
    QVariantMap data = JSON::parse(raw).toMap();
    return setError("token_error: " + data.value(LS("error")).toMap().value(LS("message")).toByteArray());
  }

  QByteArray token = QUrl(LC('?') + raw).queryItemValue(LS("access_token")).toUtf8();
  log(NodeLog::InfoLevel, "Token is successfully received");

  QNetworkRequest request(QUrl(LS("https://graph.facebook.com/me?access_token=") + token));
  QNetworkReply *reply = m_manager->get(request);
  connect(reply, SIGNAL(finished()), SLOT(dataReady()));
}


void FacebookAuth::getToken()
{
  QNetworkRequest request(QUrl(LS("https://graph.facebook.com/oauth/access_token?client_id=") +
      m_provider->id +
      LS("&redirect_uri=")  + m_provider->redirect +
      LS("&client_secret=") + m_provider->secret +
      LS("&code="           + m_code)));

  QNetworkReply *reply = m_manager->get(request);
  connect(reply, SIGNAL(finished()), SLOT(tokenReady()));
}


bool FacebookAuthCreator::serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
{
  if (path == LS("/oauth2/facebook")) {
    new FacebookAuth(url, path, request, response, parent);
    return true;
  }

  return false;
}
