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

#include "handlers/StateHandler.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Tufao/Headers.h"
#include "Tufao/httpserverresponse.h"

StateHandler::StateHandler(const QByteArray &state, Tufao::HttpServerResponse *response, QObject *parent)
  : QObject(parent)
  , m_state(state)
  , m_response(response)
{
}


bool StateHandlerCreator::serve(const QUrl &, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
{
  if (path.startsWith(LS("/state/"))) {
    QByteArray state = path.mid(7, 34).toLatin1();
    if (SimpleID::typeOf(SimpleID::decode(state)) != SimpleID::MessageId) {
      response->writeHead(Tufao::HttpServerResponse::BAD_REQUEST);
      response->headers().replace("Content-Type", "application/json");
      response->end("{\"error\":\"invalid_state\"}");
      return true;
    }

    return addHandler(request, new StateHandler(state, response, parent));
  }

  return false;
}
