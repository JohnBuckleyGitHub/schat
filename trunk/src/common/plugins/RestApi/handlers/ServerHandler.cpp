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

  if (path == LS("/v1/server"))
    return server(request, response);

  else if (path == LS("/v1/server/uptime"))
    return uptime(request, response);

  return false;
}


/*!
 * Обработка запроса "/api/v1/server".
 */
bool ServerHandler::server(Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response)
{
  Tufao::Headers &headers = response->headers();
  FeedPtr feed = Ch::server()->feed(LS("server"));
  qint64 date = feed->head().date();

  if (m_cache.date != date) {
    m_cache.date = date;
    m_cache.etag = etag(date, "/v1/server");

    QVariantMap data = feed->feed();
    data.remove(LS("head"));
    m_cache.body = JSON::generate(data);
  }

  setLastModified(headers, date);
  setETag(headers, m_cache.etag);
  setNoCache(headers);

  if (!ifModified(request->headers(), m_cache.etag)) {
    response->writeHead(Tufao::HttpServerResponse::NOT_MODIFIED);
    response->end();
    return true;
  }

  response->writeHead(Tufao::HttpServerResponse::OK);
  if (request->method() != "HEAD") {
    setContentLength(headers, m_cache.body.size());
    response->end(m_cache.body);
  }
  else
    response->end();

  return true;
}


/*!
 * Обработка запроса "/api/v1/server/uptime".
 */
bool ServerHandler::uptime(Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response)
{
  Tufao::Headers &headers = response->headers();
  FeedPtr feed = Ch::server()->feed(LS("server"));

  setNoStore(headers);

  response->writeHead(Tufao::HttpServerResponse::OK);
  if (request->method() != "HEAD") {
    QByteArray body = JSON::generate(feed->get(LS("uptime")).json);
    setContentLength(headers, body.size());
    response->end(body);
  }
  else
    response->end();

  return true;
}
