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

#ifndef OAUTHHANDLER_H_
#define OAUTHHANDLER_H_

#include <QUrl>

#include "HandlerCreator.h"

class OAuthData;
class QNetworkAccessManager;
class QNetworkReply;

class OAuthHandler : public QObject
{
  Q_OBJECT

public:
  OAuthHandler(const QString &provider, const QUrl &url, const QString &path, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, QObject *parent = 0);
  static QByteArray page(const QString &name);

protected:
  virtual void setError(const QByteArray &error = "unknown_error");
  void log(const QByteArray &text);
  void serveError();
  void serveOk();
  void setState(const QByteArray &state);

  const QString &m_path;                 ///< Относительный путь.
  const QUrl &m_url;                     ///< Полный URL адрес запроса.
  OAuthData *m_provider;                 ///< Информация о провайдере.
  QByteArray m_code;                     ///< Авторизационный код.
  QByteArray m_state;                    ///< Base32 кодированный идентификатор состояния.
  QNetworkAccessManager *m_manager;      ///< Менеджер доступа к сети.
  QNetworkReply *m_reply;                ///< Текущий завершённый HTTP запрос.
  Tufao::HttpServerRequest *m_request;   ///< HTTP запрос.
  Tufao::HttpServerResponse *m_response; ///< HTTP ответ.
};

#endif /* OAUTHHANDLER_H_ */
