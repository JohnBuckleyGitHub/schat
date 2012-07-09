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

#include <QFile>

#include "AuthCore.h"
#include "net/SimpleID.h"
#include "oauth2/OAuthHandler.h"
#include "sglobal.h"
#include "Tufao/headers.h"
#include "Tufao/httpserverrequest.h"
#include "Tufao/httpserverresponse.h"

OAuthHandler::OAuthHandler(const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent)
  : QObject(parent)
  , m_path(path)
  , m_url(url)
  , m_request(request)
  , m_response(response)
{
}


/*!
 * Получение содержимого файла, если файл находится в WWW директории сервера будет использоваться он
 * в противном случае будет использоваться файл из ресурсов. Это позволяет переопределить стандартные страницы.
 */
QByteArray OAuthHandler::page(const QString &name)
{
  QByteArray out;
  QFile file(AuthCore::root() + LC('/') + name);
  if (file.exists() && file.open(QIODevice::ReadOnly))
    out = file.readAll();

  if (out.isEmpty()) {
    file.setFileName(LS(":/www/") + name);
    if (file.open(QIODevice::ReadOnly))
      out = file.readAll();
  }

  return out;
}


/*!
 * Отображение страницы с ошибкой, если пользователь отменил авторизацию.
 */
void OAuthHandler::serveError()
{
  m_response->writeHead(Tufao::HttpServerResponse::OK);
  m_response->headers().replace("Content-Type", "text/html");

  QByteArray data = page(LS("error.html"));
  QByteArray lang = m_request->headers().value("Accept-Language").left(2);
  if (lang == "ru")
    data.replace("${ERROR_TEXT}", "<b>Авторизация была отменена вами</b>");
  else
    data.replace("${ERROR_TEXT}", "<b>Authorization has been canceled by you</b>");

  m_response->end(data);
}



void OAuthHandler::serveOk()
{
  m_response->writeHead(Tufao::HttpServerResponse::OK);
  m_response->headers().replace("Content-Type", "text/html");
  QByteArray data = page(LS("result.html"));
  m_response->end(data);
}


/*!
 * Установка или генерация идентификатора состояния.
 */
void OAuthHandler::setState(const QByteArray &state)
{
  if (SimpleID::typeOf(SimpleID::decode(state)) == SimpleID::MessageId)
    m_state = state;
  else
    m_state = SimpleID::encode(SimpleID::randomId(SimpleID::MessageId));
}
