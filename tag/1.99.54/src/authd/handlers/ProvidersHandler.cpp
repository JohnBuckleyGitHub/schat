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
#include <QUrl>

#if QT_VERSION >= 0x050000
# include <QUrlQuery>
#endif

#include "AuthCore.h"
#include "handlers/ProvidersHandler.h"
#include "JSON.h"
#include "oauth2/OAuthData.h"
#include "Settings.h"
#include "sglobal.h"
#include "Tufao/headers.h"
#include "Tufao/httpserverresponse.h"

bool ProvidersHandler::serve(const QUrl &url, const QString &path, Tufao::HttpServerRequest *, Tufao::HttpServerResponse *response, QObject *)
{
  if (path == LS("/providers")) {
    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().replace("Content-Type", "application/json");

    QByteArray body;
    QVariantMap list;
    const QMap<QString, OAuthData *> &providers = AuthCore::i()->providers();

    QMapIterator<QString, OAuthData *> i(providers);
    while (i.hasNext()) {
      i.next();
#     if QT_VERSION >= 0x050000
      list[i.key()] = i.value()->toJSON(QUrlQuery(url).queryItemValue(LS("state")).toLatin1());
#     else
      list[i.key()] = i.value()->toJSON(url.queryItemValue(LS("state")).toLatin1());
#     endif
    }

    if (m_order.isEmpty()) {
      m_order = AuthCore::settings()->value(LS("Order")).toStringList();
      QMutableStringListIterator j(m_order);
      while (j.hasNext()) {
        QString &name = j.next();
        if (!providers.contains(name))
          j.remove();
      }
    }

    QVariantMap data;
    data[LS("providers")] = list;
    data[LS("order")]     = m_order;
    body = JSON::generate(data);

    response->end(body);
    return true;
  }

  return false;
}
