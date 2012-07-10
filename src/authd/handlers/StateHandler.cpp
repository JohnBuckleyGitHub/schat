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

#include "AuthCore.h"
#include "handlers/StateHandler.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Tufao/Headers.h"
#include "Tufao/httpserverresponse.h"

StateHandler::StateHandler(const QByteArray &state, Tufao::HttpServerResponse *response)
  : QObject(response)
  , m_state(state)
  , m_response(response)
{
  connect(AuthCore::state(), SIGNAL(added(QByteArray, AuthStatePtr)), SLOT(added(QByteArray, AuthStatePtr)));
}


void StateHandler::serveOk(Tufao::HttpServerResponse *response, AuthStatePtr data)
{
  response->writeHead(Tufao::HttpServerResponse::OK);

  QVariantMap json;
  json[LS("id")]       = data->id;
  json[LS("provider")] = data->provider;
  json[LS("raw")]      = data->raw;
  response->end(JSON::generate(json));
}


void StateHandler::added(const QByteArray &state, AuthStatePtr data)
{
  if (m_state != state)
    return;

  serveOk(m_response, data);
}


bool StateHandlerCreator::serve(const QUrl &, const QString &path, Tufao::HttpServerRequest *, Tufao::HttpServerResponse *response, QObject *)
{
  response->headers().replace("Content-Type", "application/json");

  if (path.startsWith(LS("/state/"))) {
    QByteArray state = path.mid(7, 34).toLatin1();
    if (SimpleID::typeOf(SimpleID::decode(state)) != SimpleID::MessageId) {
      response->writeHead(Tufao::HttpServerResponse::BAD_REQUEST);
      response->end("{\"error\":\"invalid_state\"}");
      return true;
    }

    AuthStatePtr data = AuthCore::state()->get(state);
    if (!data) {
      new StateHandler(state, response);
      return true;
    }

    if (!data->error.isEmpty()) {
      response->writeHead(Tufao::HttpServerResponse::FORBIDDEN);
      response->end("{\"error\":\"" + data->error + "\"}");
      return true;
    }

    StateHandler::serveOk(response, data);
    return true;
  }

  return false;
}
