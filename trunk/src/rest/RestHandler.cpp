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

#include <QCryptographicHash>

#include "DateTime.h"
#include "net/SimpleID.h"
#include "RestHandler.h"
#include "Tufao/headers.h"

bool RestHandler::serve(const QUrl &, const QString &, Tufao::HttpServerRequest *, Tufao::HttpServerResponse *, QObject *)
{
  return false;
}


bool RestHandler::ifModified(const Tufao::Headers &headers, const QByteArray &etag)
{
  QByteArray tag = headers.value("If-None-Match");
  if (tag.isEmpty() || tag != etag)
    return true;

  return false;
}


/*!
 * Формирование ETag строки на основе даты и соли.
 *
 * Дата приводится к строковому виду и хешируется вместе с солью с помощью SHA1, затем кодируется с помощью Base64, из результата удаляется дополняющий символ.
 */
QByteArray RestHandler::etag(qint64 date, const QByteArray &salt)
{
  return '"' + QCryptographicHash::hash(QByteArray::number(date) + salt, QCryptographicHash::Sha1).toBase64().replace('=', QByteArray()) + '"';
}


void RestHandler::setContentLength(Tufao::Headers &headers, qint64 size)
{
  headers.insert("Content-Length", QByteArray::number(size));
}


void RestHandler::setETag(Tufao::Headers &headers, const QByteArray &etag)
{
  headers.insert("ETag", etag);
}


void RestHandler::setLastModified(Tufao::Headers &headers, qint64 date)
{
  headers.insert("Last-Modified", Tufao::Headers::fromDateTime(DateTime::toDateTime(date)));
}
