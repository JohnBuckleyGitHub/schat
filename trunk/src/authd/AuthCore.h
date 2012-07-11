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

#ifndef AUTHCORE_H_
#define AUTHCORE_H_

#include <QObject>
#include <QHash>

namespace Tufao {
  class HttpServer;
}

class AuthHandler;
class AuthState;
class HandlerCreator;
class NodeLog;
class OAuthData;
class QUrl;
class Settings;

class AuthCore : public QObject
{
  Q_OBJECT

public:
  AuthCore(QObject *parent = 0);
  ~AuthCore();
  inline const QHash<QString, OAuthData *>& providers() const { return m_providers; }
  inline static AuthCore *i()                                 { return m_self; }
  inline static AuthState *state()                            { return m_self->m_state; }
  inline static OAuthData *provider(const QString &name)      { return m_self->m_providers.value(name); }
  inline static Settings *settings()                          { return m_self->m_settings; }
  static QString root();

private slots:
  void start();

private:
  void add(const QUrl &url);
  void add(HandlerCreator *handler);
  void add(OAuthData *data);
  void openLog();

  AuthHandler *m_handler;                  ///< Основной обработчик запросов.
  AuthState *m_state;                      ///< Состояния авторизаций.
  NodeLog *m_log;                          ///< Журнал.
  QHash<QString, OAuthData *> m_providers; ///< Список доступных OAuth провайдеров.
  QList<Tufao::HttpServer *> m_servers;    ///< HTTP или HTTPS сервера ожидающие подключений.
  Settings *m_settings;                    ///< Настройки сервера.
  static AuthCore *m_self;                 ///< Указатель на себя.
};

#endif /* AUTHCORE_H_ */
