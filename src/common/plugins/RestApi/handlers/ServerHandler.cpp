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


bool ServerHandler::serve()
{
  if (!m_path.startsWith(LS("/v1/server")))
    return false;

  if (m_path == LS("/v1/server"))
    return server();

  else if (m_path == LS("/v1/server/uptime"))
    return uptime();

  return false;
}


/*!
 * Обработка запроса "/api/v1/server".
 */
bool ServerHandler::server()
{
  if (!ifMethodAllowed())
    return true;

  FeedPtr feed = Ch::server()->feed(LS("server"));
  qint64 date  = feed->head().date();

  if (m_cache.date != date) {
    m_cache.date = date;
    m_cache.etag = etag(date, "/v1/server");

    QVariantMap data = feed->feed();
    data.remove(LS("head"));
    m_cache.body = JSON::generate(data);
  }

  setLastModified(date);
  setETag(m_cache.etag);
  setNoCache();

  if (!ifModified(m_cache.etag)) {
    m_response->writeHead(Tufao::HttpServerResponse::NOT_MODIFIED);
    m_response->end();
    return true;
  }

  m_response->writeHead(Tufao::HttpServerResponse::OK);
  if (m_request->method() != "HEAD") {
    setContentLength(m_cache.body.size());
    m_response->end(m_cache.body);
  }
  else
    m_response->end();

  return true;
}


/*!
 * Обработка запроса "/api/v1/server/uptime".
 */
bool ServerHandler::uptime()
{
  if (!ifMethodAllowed())
    return true;

  FeedPtr feed = Ch::server()->feed(LS("server"));

  setNoStore();

  m_response->writeHead(Tufao::HttpServerResponse::OK);
  if (m_request->method() != "HEAD") {
    QByteArray body = JSON::generate(feed->get(LS("uptime")).json);
    setContentLength(body.size());
    m_response->end(body);
  }
  else
    m_response->end();

  return true;
}
