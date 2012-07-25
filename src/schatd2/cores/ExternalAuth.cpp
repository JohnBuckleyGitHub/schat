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

#include "Account.h"
#include "Ch.h"
#include "cores/Core.h"
#include "cores/ExternalAuth.h"
#include "events.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "sglobal.h"
#include "Storage.h"

QHash<QByteArray, QVariantMap> ExternalAuthTask::m_cache;

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
  m_timer = new QBasicTimer();
  m_timer->start(20000, this);

  QTimer::singleShot(0, this, SLOT(start()));
}


ExternalAuthTask::~ExternalAuthTask()
{
  if (m_timer->isActive())
    m_timer->stop();

  delete m_timer;
}


void ExternalAuthTask::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == m_timer->timerId()) {
    m_timer->stop();
    deleteLater();
  }

  QObject::timerEvent(event);
}


/*!
 * Получение данных об авторизации от внешнего сервера.
 */
void ExternalAuthTask::ready()
{
  int error = m_reply->error();
  if (error == QNetworkReply::ContentOperationNotPermittedError)
    return setError(Notice::Forbidden);

  if (error)
    return setError(Notice::BadGateway);

  QByteArray raw = m_reply->readAll();
  QVariantMap data = JSON::parse(raw).toMap();

  if (data.isEmpty())
    return setError(Notice::BadGateway);

  m_reply->deleteLater();

  qDebug() << raw;

  done(data);
}


void ExternalAuthTask::sslErrors()
{
  m_reply->ignoreSslErrors();
}


void ExternalAuthTask::start()
{
  QVariantMap data = m_cache.value(m_data.id + m_data.cookie);
  if (!data.isEmpty())
    return done(data);

  m_manager = new QNetworkAccessManager(this);
  QNetworkRequest request(QUrl(Storage::authServer() + LS("/state/") + SimpleID::encode(m_data.id)));
  request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
  request.setRawHeader("X-SChat-Cookie", SimpleID::encode(m_data.cookie));

  m_reply = m_manager->get(request);
  connect(m_reply, SIGNAL(finished()), SLOT(ready()));
  connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors()));
}


/*!
 * Авторизация пользователя, если пользователь не существует, будет создан новый.
 */
AuthResult ExternalAuthTask::auth(const QVariantMap &data)
{
  QByteArray id = SimpleID::decode(data.value(LS("id")).toByteArray());
  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return AuthResult(Notice::Forbidden, m_data.id);

  AuthResult result = AnonymousAuth::isCollision(id, m_data.nick, m_data.id, Storage::nickOverride());
  if (result.action == AuthResult::Reject) {
    if (result.status == Notice::NickAlreadyUse)
      m_cache[m_data.id + m_data.cookie] = data;

    return result;
  }

  ChatChannel channel = Ch::channel(id, SimpleID::UserId);
  bool created = false;

  if (!channel) {
    channel = ChatChannel(new ServerChannel(id, m_data.nick));
    created = true;

    channel->setAccount();
    channel->account()->provider = data.value(LS("provider")).toString();
    channel->account()->groups += LS("registered");
    channel->setName(m_data.nick);
    channel->gender().setRaw(m_data.gender);
  }

  if (channel->status().value() == Status::Offline)
    channel->status().set(m_data.status);

  if (!channel->isValid())
    return AuthResult(Notice::BadRequest, m_data.id);

  Core::add(channel);
  Ch::newUserChannel(channel, m_data, m_host, created, m_socket);

  SCHAT_LOG_INFO_STR("EXTERNAL AUTH: " + channel->name().toUtf8() + '@' + m_host.toUtf8() + '/' + SimpleID::encode(channel->id()) + ", " + m_data.host.toUtf8());
  m_cache.remove(m_data.id + m_data.cookie);
  return AuthResult(id, m_data.id);
}


void ExternalAuthTask::done(const QVariantMap &data)
{
  AuthResult result = auth(data);
  if (result.action == AuthResult::Reject) {
    Core::i()->reject(result, m_socket);
  }
  else if (result.action == AuthResult::Accept) {
    Core::i()->accept(result, m_host);
  }

  m_timer->stop();
  deleteLater();
}


/*!
 * Отклонение авторизации пользователя.
 */
void ExternalAuthTask::setError(int errorCode)
{
  m_reply->deleteLater();
  Core::i()->reject(AuthResult(errorCode, m_data.id), m_socket);
  m_timer->stop();
  deleteLater();
}
