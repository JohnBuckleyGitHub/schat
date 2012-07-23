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
#include <QCryptographicHash>

#include "AuthCore.h"
#include "AuthHandler.h"
#include "AuthState.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "oauth2/OAuthData.h"
#include "oauth2/OdnoklassnikiAuth.h"
#include "sglobal.h"
#include "Tufao/httpserverresponse.h"

OdnoklassnikiAuth::OdnoklassnikiAuth(const QByteArray &state, const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
  : OAuthHandler(LS("odnoklassniki"), state, url, path, request, response, parent)
{
}


void OdnoklassnikiAuth::dataReady()
{
  OAUTH_PREPARE_REPLY
  OAUTH_BAD_STATUS

  QVariantMap data = JSON::parse(raw).toMap();
  if (data.contains(LS("error_code")))
    return setError("data_error: " + data.value(LS("error_msg")).toByteArray());

  QByteArray uid = data.value(LS("uid")).toByteArray();
  QByteArray id = SimpleID::encode(SimpleID::make("odnoklassniki:" + uid, SimpleID::UserId));
  AuthCore::state()->add(new AuthStateData(m_state, "odnoklassniki", id, data));

  log(NodeLog::InfoLevel, "Data is successfully received, id:" + id + ", uid:" + uid);
  deleteLater();
}


void OdnoklassnikiAuth::tokenReady()
{
  OAUTH_PREPARE_REPLY
  OAUTH_BAD_STATUS

  QVariantMap data = JSON::parse(raw).toMap();
  if (data.contains(LS("error")))
    return setError("token_error: " + data.value(LS("error")).toByteArray());

  QByteArray token = data.value(LS("access_token")).toByteArray();
  log(NodeLog::InfoLevel, "Token is successfully received");

  QByteArray sign = QCryptographicHash::hash("application_key=" + m_provider->publicKey + "method=users.getCurrentUser"
      + QCryptographicHash::hash(token + m_provider->secret, QCryptographicHash::Md5).toHex(), QCryptographicHash::Md5).toHex();

  QNetworkRequest request(QUrl(LS("http://api.odnoklassniki.ru/fb.do?method=users.getCurrentUser&access_token=" + token + "&application_key=" + m_provider->publicKey + "&sig=" + sign)));
  QNetworkReply *reply = m_manager->get(request);
  connect(reply, SIGNAL(finished()), SLOT(dataReady()));
}


void OdnoklassnikiAuth::getToken()
{
  QNetworkRequest request(QUrl(LS("https://api.odnoklassniki.ru/oauth/token.do")));
  request.setHeader(QNetworkRequest::ContentTypeHeader, LS("application/x-www-form-urlencoded"));

  QByteArray body = "code=" + m_code +
      "&redirect_uri=" + m_provider->redirect + '/' + m_state +
      "&grant_type=authorization_code&client_id=" + m_provider->id +
      "&client_secret=" + m_provider->secret;

  QNetworkReply *reply = m_manager->post(request, body);
  connect(reply, SIGNAL(finished()), SLOT(tokenReady()));
}


bool OdnoklassnikiAuthCreator::serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
{
  if (path == LS("/oauth2/odnoklassniki")) {
    AuthHandler::setError(response, Tufao::HttpServerResponse::BAD_REQUEST);
  }
  else if (path.startsWith(LS("/oauth2/odnoklassniki/"))) {
    QByteArray state = path.mid(22, 34).toLatin1();
    if (SimpleID::typeOf(SimpleID::decode(state)) != SimpleID::MessageId) {
      AuthHandler::setError(response, Tufao::HttpServerResponse::BAD_REQUEST);
      return true;
    }

    new OdnoklassnikiAuth(state, url, path, request, response, parent);
    return true;
  }

  return false;
}
