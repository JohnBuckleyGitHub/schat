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

#ifndef RESTHANDLER_H_
#define RESTHANDLER_H_

#include <QString>

#include "schat.h"

namespace Tufao {
  class HttpServerRequest;
  class HttpServerResponse;
  struct Headers;
}

class QUrl;
class QObject;

struct RestReplyCache
{
  RestReplyCache()
  : date(0)
  {}

  QByteArray body; ///< Тело ответа.
  QByteArray etag; ///< ETag заголовок.
  qint64 date;     ///< Дата последнего обновления.
};


class SCHAT_REST_EXPORT RestHandler
{
public:
  RestHandler() {}
  virtual ~RestHandler() {}
  virtual bool serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent);

  static bool ifModified(const Tufao::Headers &headers, const QByteArray &etag);
  static QByteArray etag(qint64 date, const QByteArray &salt);
  static void setContentLength(Tufao::Headers &headers, qint64 size);
  static void setETag(Tufao::Headers &headers, const QByteArray &etag);
  static void setLastModified(Tufao::Headers &headers, qint64 date);
  static void setNoCache(Tufao::Headers &headers);
  static void setNoStore(Tufao::Headers &headers);
};

#endif /* RESTHANDLER_H_ */
