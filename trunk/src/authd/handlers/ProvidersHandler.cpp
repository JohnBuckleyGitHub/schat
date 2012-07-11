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

#include <QStringList>

#include "AuthCore.h"
#include "handlers/ProvidersHandler.h"
#include "JSON.h"
#include "oauth2/OAuthData.h"
#include "Settings.h"
#include "sglobal.h"
#include "Tufao/headers.h"
#include "Tufao/httpserverresponse.h"

bool ProvidersHandler::serve(const QUrl &, const QString &path, Tufao::HttpServerRequest *, Tufao::HttpServerResponse *response, QObject *)
{
  if (path == LS("/providers")) {
    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().replace("Content-Type", "application/json");

    if (m_cache.isEmpty()) {
      QVariantMap list;
      const QHash<QString, OAuthData *> &providers = AuthCore::i()->providers();

      QHashIterator<QString, OAuthData *> i(providers);
      while (i.hasNext()) {
        i.next();
        list[i.key()] = i.value()->toJSON();
      }

      QStringList order = AuthCore::settings()->value(LS("Order")).toStringList();
      QMutableStringListIterator j(order);
      while (j.hasNext()) {
        QString &name = j.next();
        if (!providers.contains(name))
          j.remove();
      }

      QVariantMap data;
      data[LS("providers")] = list;
      data[LS("order")]     = order;
      m_cache = JSON::generate(data);
    }

    response->end(m_cache);
    return true;
  }

  return false;
}
