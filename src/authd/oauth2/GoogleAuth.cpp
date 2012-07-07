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

#include "oauth2/GoogleAuth.h"
#include "sglobal.h"

GoogleAuth::GoogleAuth(const QUrl &url, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
  : QObject(parent)
  , m_state(GetAccessToken)
{
  m_manager = new QNetworkAccessManager(this);
//  QNetworkRequest request(m_url);

  qDebug() << "GoogleAuth()" << url.queryItemValue(LS("code"));

  QNetworkRequest r(QUrl(LS("https://accounts.google.com/o/oauth2/token")));
  r.setHeader(QNetworkRequest::ContentTypeHeader, LS("application/x-www-form-urlencoded"));
  QByteArray body = "code=" + url.queryItemValue(LS("code")).toUtf8();
  body += "&client_id=649867065580.apps.googleusercontent.com";
  body += "&client_secret=_v35lXd85eThn3TmNF0cFgRc";
  body += "&redirect_uri=http://localhost:7668/oauth2/google";
  body += "&grant_type=authorization_code";

  QNetworkReply *reply = m_manager->post(r, body);
  connect(reply, SIGNAL(readyRead()), SLOT(tokenReady()));
}


void GoogleAuth::tokenReady()
{
  QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
  qDebug() << "GoogleAuth::tokenReady()" << reply->error() << reply->readAll();
}


bool GoogleAuthCreator::serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
{
  if (path == LS("/oauth2/google")) {
    new GoogleAuth(url, request, response, parent);
    return true;
  }

  return false;
}
