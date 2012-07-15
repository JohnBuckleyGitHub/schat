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

#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "AuthCore.h"
#include "AuthState.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "oauth2/MailRuAuth.h"
#include "oauth2/OAuthData.h"
#include "sglobal.h"

MailRuAuth::MailRuAuth(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
  : OAuthHandler(LS("mail_ru"), url, path, request, response, parent)
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


void MailRuAuth::dataReady()
{
  OAUTH_PREPARE_REPLY

  if (status != 200) {
    QByteArray error = JSON::parse(raw).toMap().value(LS("error")).toMap().value(LS("error_msg")).toByteArray();
    return setError("data_error: " + error);
  }

  QVariantList list = JSON::parse(raw).toList();
  if (list.isEmpty())
    return setError("invalid_data");

  QVariantMap data = list.first().toMap();
  QByteArray email = data.value(LS("email")).toByteArray();
  if (email.isEmpty())
    return setError("invalid_email");

  QByteArray id = SimpleID::encode(SimpleID::make("mail_ru:" + email, SimpleID::UserId));
  AuthCore::state()->add(new AuthStateData(m_state, "mail_ru", id, QByteArray(), data));

  log(NodeLog::InfoLevel, "Data is successfully received, id:" + id + ", email:" + email);
  deleteLater();
}


void MailRuAuth::tokenReady()
{
  OAUTH_PREPARE_REPLY

  QVariantMap data = JSON::parse(raw).toMap();
  if (status != 200)
    return setError("token_error: " + data.value(LS("error")).toByteArray());

  QByteArray token = data.value(LS("access_token")).toByteArray();
  log(NodeLog::InfoLevel, "Token is successfully received");

  QByteArray sign = QCryptographicHash::hash("app_id=" + m_provider->id + "method=users.getInfosecure=1session_key=" + token + m_provider->secret, QCryptographicHash::Md5).toHex();
  QNetworkRequest request(QUrl(LS("https://www.appsmail.ru/platform/api?method=users.getInfo&secure=1&app_id=" + m_provider->id + "&session_key=" + token + "&sig=" + sign)));
  QNetworkReply *reply = m_manager->get(request);
  connect(reply, SIGNAL(finished()), SLOT(dataReady()));
}


void MailRuAuth::getToken()
{
  QNetworkRequest request(QUrl(LS("https://connect.mail.ru/oauth/token")));
  request.setHeader(QNetworkRequest::ContentTypeHeader, LS("application/x-www-form-urlencoded"));

  QByteArray body = "client_id=" + m_provider->id;
  body += "&client_secret="      + m_provider->secret;
  body += "&grant_type=authorization_code";
  body += "&code="               + m_code;
  body += "&redirect_uri="       + m_provider->redirect;

  QNetworkReply *reply = m_manager->post(request, body);
  connect(reply, SIGNAL(finished()), SLOT(tokenReady()));
}


bool MailRuAuthCreator::serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
{
  if (path == LS("/oauth2/mail_ru")) {
    new MailRuAuth(url, path, request, response, parent);
    return true;
  }

  return false;
}
