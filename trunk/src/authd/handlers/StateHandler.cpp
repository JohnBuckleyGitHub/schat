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

#include <QUrl>

#include "AuthCore.h"
#include "handlers/StateHandler.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Tufao/headers.h"
#include "Tufao/httpserverrequest.h"
#include "Tufao/httpserverresponse.h"

StateHandler::StateHandler(const QByteArray &state, const QByteArray &secret, Tufao::HttpServerResponse *response)
  : QObject(response)
  , m_secret(secret)
  , m_state(state)
  , m_response(response)
{
  connect(AuthCore::state(), SIGNAL(added(QByteArray, AuthStatePtr)), SLOT(added(QByteArray, AuthStatePtr)));
}


void StateHandler::serve(const QByteArray &secret, Tufao::HttpServerResponse *response, AuthStatePtr data)
{
  if (!data->error.isEmpty()) {
    response->writeHead(Tufao::HttpServerResponse::FORBIDDEN);
    response->end("{\"error\":\"" + data->error + "\"}");
    return;
  }

  response->writeHead(Tufao::HttpServerResponse::OK);

  QVariantMap json;
  json[LS("id")]       = data->id;
  json[LS("provider")] = data->provider;
  json[LS("raw")]      = data->raw;

  if (!secret.isEmpty() && SimpleID::encode(SimpleID::make(QUrl(AuthCore::baseUrl()).host().toUtf8() + secret, SimpleID::MessageId)) == data->state) {
    json[LS("cookie")] = data->cookie;
  }

  response->end(JSON::generate(json));
}


void StateHandler::added(const QByteArray &state, AuthStatePtr data)
{
  if (m_state != state)
    return;

  serve(m_secret, m_response, data);
}


bool StateHandlerCreator::serve(const QUrl &, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *)
{
  response->headers().replace("Content-Type", "application/json");

  if (path.startsWith(LS("/state/"))) {
    QByteArray state = path.mid(7, 34).toLatin1();
    if (SimpleID::typeOf(SimpleID::decode(state)) != SimpleID::MessageId) {
      response->writeHead(Tufao::HttpServerResponse::BAD_REQUEST);
      response->end("{\"error\":\"invalid_state\"}");
      return true;
    }

    QByteArray secret = request->headers().value("X-SChat-Secret");
    AuthStatePtr data = AuthCore::state()->get(state);
    if (!data) {
      new StateHandler(state, secret, response);
      return true;
    }

    StateHandler::serve(secret, response, data);
    return true;
  }

  return false;
}
