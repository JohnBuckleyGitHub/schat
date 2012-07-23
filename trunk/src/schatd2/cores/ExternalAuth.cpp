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
#include <QNetworkReply>
#include <QTimer>

#include "cores/Core.h"
#include "cores/ExternalAuth.h"
#include "events.h"
#include "net/SimpleID.h"
#include "Storage.h"
#include "sglobal.h"

ExternalAuth::ExternalAuth(Core *core)
  : AnonymousAuth(core)
{
}


AuthResult ExternalAuth::auth(const AuthRequest &data)
{
  if (SimpleID::typeOf(data.cookie) != SimpleID::CookieId)
    return AuthResult(Notice::BadRequest, data.id);

  new ExternalAuthTask(data, m_core->packetsEvent()->address.toString(), m_core);
  return AuthResult(AuthResult::Pending);
}


int ExternalAuth::type() const
{
  return AuthRequest::External;
}


ExternalAuthTask::ExternalAuthTask(const AuthRequest &data, const QString &host, QObject *parent)
  : QObject(parent)
  , m_data(data)
  , m_host(host)
  , m_socket(Core::socket())
{
  m_manager = new QNetworkAccessManager(this);
  QTimer::singleShot(0, this, SLOT(start()));
}


void ExternalAuthTask::ready()
{
  if (m_reply->error())
    return setError(Notice::BadGateway);

  QByteArray raw = m_reply->readAll();
  QVariantMap data = JSON::parse(raw).toMap();

  if (data.isEmpty())
    return setError(Notice::BadGateway);

  m_reply->deleteLater();

  qDebug() << raw;
}


void ExternalAuthTask::sslErrors()
{
  m_reply->ignoreSslErrors();
}


void ExternalAuthTask::start()
{
  QNetworkRequest request(QUrl(Storage::authServer() + LS("/state/") + SimpleID::encode(m_data.id)));
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
  request.setRawHeader("X-SChat-Cookie", SimpleID::encode(m_data.cookie));

  m_reply = m_manager->get(request);
  connect(m_reply, SIGNAL(finished()), SLOT(ready()));
  connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors()));
}


/*!
 * Отклонение авторизации пользователя.
 */
void ExternalAuthTask::setError(int errorCode)
{
  m_reply->deleteLater();
  Core::i()->reject(AuthResult(errorCode, m_data.id), m_socket);
}
