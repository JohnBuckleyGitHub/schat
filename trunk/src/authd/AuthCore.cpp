/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QDir>
#include <QCoreApplication>
#include <QTimer>
#include <QUrl>

#include "AuthCore.h"
#include "AuthHandler.h"
#include "AuthState.h"
#include "HandlerRoute.h"
#include "handlers.h"
#include "NodeLog.h"
#include "oauth2/facebook/FacebookAuthData.h"
#include "oauth2/github/GithubAuthData.h"
#include "oauth2/google/GoogleAuthData.h"
#include "oauth2/live/LiveAuthData.h"
#include "oauth2/mail_ru/MailRuAuthData.h"
#include "oauth2/OAuthData.h"
#include "oauth2/odnoklassniki/OdnoklassnikiAuthData.h"
#include "oauth2/vkontakte/VkontakteAuthData.h"
#include "oauth2/yandex/YandexAuthData.h"
#include "Path.h"
#include "Settings.h"
#include "sglobal.h"
#include "Storage.h"
#include "Tufao/httpserver.h"
#include "version.h"

AuthCore *AuthCore::m_self = 0;

AuthCore::AuthCore(QObject *parent)
  : QObject(parent)
{
  m_self = this;
  Path::init();

  m_log = new NodeLog();

  m_settings = new Settings(defaultConf(), this);
  m_settings->setDefault(LS("Listen"),   QStringList(LS("http://0.0.0.0:7668")));
  m_settings->setDefault(LS("Root"),     defaultRoot());
  m_settings->setDefault(LS("Order"),    QStringList() << LS("facebook") << LS("vkontakte") << LS("google") << LS("live") << LS("github") << LS("yandex") << LS("odnoklassniki") << LS("mail_ru"));
  m_settings->setDefault(LS("LogLevel"), 2);
  m_settings->setDefault(LS("BaseUrl"),  QString());

  openLog();

  m_handler = new AuthHandler(this);
  m_handler->setRoot(QDir::cleanPath(m_settings->value(LS("Root")).toString()));

  m_state = new AuthState(this);

  add(new ProvidersHandler());
  add(new StateHandlerCreator());
  add(new FacebookAuthCreator());
  add(new GoogleAuthCreator());
  add(new YandexAuthCreator());
  add(new MailRuAuthCreator());
  add(new VkontakteAuthCreator());
  add(new OdnoklassnikiAuthCreator());
  add(new LiveAuthCreator());
  add(new GithubAuthCreator());

  QTimer::singleShot(0, this, SLOT(start()));
}


AuthCore::~AuthCore()
{
  delete m_log;
  qDeleteAll(m_providers);
}


/*!
 * Возвращает корневую директорию для файлов доступных по HTTP.
 */
QString AuthCore::root()
{
  return m_self->m_handler->root();
}


void AuthCore::version()
{
  QTextStream out(stdout);
  out << LS("Simple Chat Auth Daemon, version ") << SCHAT_VERSION;
  if (SCHAT_REVISION)
    out << LC('.') << SCHAT_REVISION;

  out << endl << endl;
  out << QString(SCHAT_COPYRIGHT).replace("©", LS("(C)")) << LS(", https://schat.me") << endl;
}


void AuthCore::start()
{
  m_baseUrl = m_settings->value(LS("BaseUrl")).toByteArray();
  if (m_baseUrl.isEmpty()) {
    SCHAT_LOG_FATAL("Configuration option \"BaseUrl\" is not set")
    QCoreApplication::exit(1);
    return;
  }

  add(new FacebookAuthData());
  add(new GoogleAuthData());
  add(new YandexAuthData());
  add(new VkontakteAuthData());
  add(new MailRuAuthData());
  add(new OdnoklassnikiAuthData());
  add(new LiveAuthData());
  add(new GithubAuthData());

  if (m_providers.isEmpty()) {
    SCHAT_LOG_FATAL("Providers list is empty")
    QCoreApplication::exit(2);
    return;
  }

  QStringList listen = m_settings->value(LS("Listen")).toStringList();
  foreach (const QString &url, listen) {
    add(QUrl(url));
  }

  if (m_servers.isEmpty()) {
    SCHAT_LOG_FATAL("Failed to open ports for incoming connections")
    QCoreApplication::exit(3);
    return;
  }

  SCHAT_LOG_INFO("Server successfully started")
}


QString AuthCore::defaultConf() const
{
# if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
  if (!Path::isPortable())
    return LS("/etc/schatd2/") + Path::app() + LS(".conf");
# endif

  return Path::data() + LC('/') + Path::app() + LS(".conf");
}


QString AuthCore::defaultRoot() const
{
# if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
  if (!Path::isPortable())
    return LS("/usr/share/schatd2/www");
# endif

  return Path::data(Path::SystemScope) + LS("/www");
}


/*!
 * Добавление адреса, на котором сервер будет принимать подключения.
 */
void AuthCore::add(const QUrl &url)
{
  Tufao::HttpServer *server = new Tufao::HttpServer(this);
  connect(server, SIGNAL(requestReady(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)), m_handler, SLOT(handleRequest(Tufao::HttpServerRequest*,Tufao::HttpServerResponse*)));

  if (server->listen(QHostAddress(url.host()), url.port())) {
    SCHAT_LOG_INFO("Added url:" << url.toString())
    m_servers.append(server);
  }
  else {
    SCHAT_LOG_ERROR("Failed to open port for incoming connections")
    server->deleteLater();
  }
}


void AuthCore::add(HandlerCreator *handler)
{
  HandlerRoute::routes.append(handler);
}


void AuthCore::add(OAuthData *data)
{
  const QByteArray &name = data->provider;

  if (!m_providers.contains(name) && data->read()) {
    m_providers[name] = data;
    SCHAT_LOG_INFO("Added authorization provider:" << name)
  }
  else
    delete data;
}


void AuthCore::openLog()
{
  QString path = Path::cache();
# if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
  if (!Path::isPortable())
    path = LS("/var/log/schatd2");
# endif

  m_log->open(path + LC('/') + Path::app() + LS(".log"), static_cast<NodeLog::Level>(m_settings->value(LS("LogLevel")).toInt()));
}
