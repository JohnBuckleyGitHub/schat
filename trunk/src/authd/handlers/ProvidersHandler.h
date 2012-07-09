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

#ifndef AUTHPROVIDERS_H_
#define AUTHPROVIDERS_H_

#include "HandlerCreator.h"

/*!
 * Ответ на запрос "/providers".
 *
 * Возвращаются JSON данные со списком доступных OAuth провайдеров.
 */
class ProvidersHandler : public HandlerCreator
{
public:
  ProvidersHandler() : HandlerCreator() {}
  bool serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent);

private:
  QByteArray m_cache; ///< Кэшированный список провайдеров.
};

#endif /* AUTHPROVIDERS_H_ */
