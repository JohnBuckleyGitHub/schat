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
#include <QNetworkRequest>
#include <QNetworkReply>

#include "JSON.h"
#include "NodeLog.h"
#include "oauth2/FacebookAuth.h"
#include "oauth2/OAuthData.h"
#include "sglobal.h"

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


void FacebookAuth::tokenReady()
{
  m_reply = qobject_cast<QNetworkReply*>(sender());
  if (!m_reply)
    return;

  if (m_reply->error())
    return setError("network_error: " + m_reply->errorString().toUtf8());

  QByteArray raw = m_reply->readAll();
  int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  m_reply->deleteLater();
  m_reply = 0;

  if (statusCode != 200) {
    QVariantMap data = JSON::parse(raw).toMap();
    return setError("token_error: " + data.value(LS("error")).toMap().value(LS("message")).toByteArray());
  }

  QByteArray token = QUrl(LC('?') + raw).queryItemValue(LS("access_token")).toUtf8();
  log(NodeLog::InfoLevel, "Token is successfully received");
}


void FacebookAuth::getToken()
{
  QNetworkRequest request(QUrl(LS("https://graph.facebook.com/oauth/access_token?client_id=") +
      m_provider->id +
      LS("&redirect_uri=")  + m_provider->redirect +
      LS("&client_secret=") + m_provider->secret +
      LS("&code="           + m_code)));

  QNetworkReply *reply = m_manager->get(request);
  connect(reply, SIGNAL(readyRead()), SLOT(tokenReady()));
}


bool FacebookAuthCreator::serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
{
  if (path == LS("/oauth2/facebook")) {
    new FacebookAuth(url, path, request, response, parent);
    return true;
  }

  return false;
}
