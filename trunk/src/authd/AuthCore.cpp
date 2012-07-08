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

#include <QDebug>

#include <QTimer>
#include <QUrl>
#include <QCoreApplication>

#include "AuthCore.h"
#include "AuthHandler.h"
#include "HandlerRoute.h"
#include "oauth2/GoogleAuth.h"
#include "Path.h"
#include "Settings.h"
#include "sglobal.h"
#include "Storage.h"
#include "Tufao/httpserver.h"

AuthCore *AuthCore::m_self = 0;

AuthCore::AuthCore(QObject *parent)
  : QObject(parent)
{
  m_self = this;
  Path::init();

  m_settings = new Settings(Storage::etcPath() + LC('/') + Path::app() + LS(".conf"), this);
  m_settings->setDefault(LS("Listen"), QStringList("http://0.0.0.0:7668"));

  m_handler = new AuthHandler(this);
  m_handler->setRoot(QCoreApplication::applicationDirPath());

  HandlerRoute::routes.append(new GoogleAuthCreator());
  QTimer::singleShot(0, this, SLOT(start()));
}


void AuthCore::start()
{
  QStringList listen = m_settings->value(LS("Listen")).toStringList();
  foreach (const QString &url, listen) {
    add(QUrl(url));
  }
}


void AuthCore::add(const QUrl &url)
{
  qDebug() << url.scheme() << url.host() << url.port();
  Tufao::HttpServer *server = new Tufao::HttpServer(this);
  connect(server, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)), m_handler, SLOT(handleRequest(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));

  if (server->listen(QHostAddress(url.host()), url.port()))
    m_servers.append(server);
  else
    server->deleteLater();
}
