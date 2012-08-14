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

#include "handlers/ServerHandler.h"
#include "sglobal.h"
#include "Tufao/httpserverrequest.h"
#include "Tufao/httpserverresponse.h"
#include "Tufao/headers.h"
#include "feeds/Feed.h"
#include "Ch.h"
#include "JSON.h"

ServerHandler::ServerHandler()
  : RestHandler()
  , m_date(0)
{
}


bool ServerHandler::serve(const QUrl &, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *)
{
  if (!path.startsWith(LS("/v1/server")))
    return false;

  const QByteArray method(request->method());
  Tufao::Headers &headers = response->headers();

  if (method != "GET" && method != "HEAD") {
    response->writeHead(Tufao::HttpServerResponse::METHOD_NOT_ALLOWED);
    headers.insert("Allow", "GET, HEAD");
    response->end();
    return true;
  }

  if (path == LS("/v1/server")) {
    server(request, response);
  }

  return false;
}


/*!
 * Обработка запроса "/api/v1/server".
 */
void ServerHandler::server(Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response)
{
  Tufao::Headers &headers = response->headers();
  FeedPtr feed = Ch::server()->feed(LS("server"));
  qint64 date = feed->head().date();

  if (m_date != date) {
    m_date = date;
    m_etag = etag(m_date, "/v1/server");

    QVariantMap data = feed->feed();
    data.remove(LS("head"));
    m_body = JSON::generate(data);
  }

  setLastModified(headers, m_date);
  setETag(headers, m_etag);
  setNoCache(headers);

  if (!ifModified(request->headers(), m_etag)) {
    response->writeHead(Tufao::HttpServerResponse::NOT_MODIFIED);
    response->end();
    return;
  }

  response->writeHead(Tufao::HttpServerResponse::OK);
  if (request->method() != "HEAD") {
    setContentLength(headers, m_body.size());
    response->end(m_body);
  }
  else
    response->end();
}
