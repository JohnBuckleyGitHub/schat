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

#ifndef STATEHANDLER_H_
#define STATEHANDLER_H_

#include "HandlerCreator.h"

class StateHandler : public QObject
{
  Q_OBJECT

public:
  StateHandler(const QByteArray &state, Tufao::HttpServerResponse *response, QObject *parent = 0);

private:
  QByteArray m_state;                    ///< Идентификатор состояния.
  Tufao::HttpServerResponse *m_response; ///< Ответ на запрос.
};


class StateHandlerCreator : public HandlerCreator
{
public:
  StateHandlerCreator() : HandlerCreator() {}
  bool serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent);
};

#endif /* STATEHANDLER_H_ */
