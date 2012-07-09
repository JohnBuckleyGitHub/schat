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

#include "HandlerCreator.h"
#include "Tufao/httpserverrequest.h"

bool HandlerCreator::serve(const QUrl &, const QString &, Tufao::HttpServerRequest *, Tufao::HttpServerResponse *, QObject *)
{
  return false;
}


/*!
 * Обвёртка для автоматического удаления обработчика запроса.
 *
 * \param request HTTP запрос.
 * \param handler Обработчик запроса.
 */
bool HandlerCreator::addHandler(Tufao::HttpServerRequest *request, QObject *handler)
{
  QObject::connect(request, SIGNAL(close()), handler, SLOT(deleteLater()));
  return true;
}
