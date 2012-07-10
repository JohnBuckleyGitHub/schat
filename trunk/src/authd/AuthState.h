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

#ifndef AUTHSTATE_H_
#define AUTHSTATE_H_

#include <QHash>
#include <QObject>
#include <QSharedPointer>
#include <QVariant>

class AuthStateData
{
public:
  AuthStateData(const QByteArray &state, const QByteArray &error);
  AuthStateData(const QByteArray &state, const QByteArray &provider, const QByteArray &id, const QByteArray &token, const QVariantMap &raw);

  QByteArray error;    ///< Текстовый код ошибки.
  QByteArray id;       ///< Глобальный идентификатор пользователя.
  QByteArray provider; ///< Кодовое имя OAuth2 провайдера.
  QByteArray state;    ///< Идентификатор состояния.
  QByteArray token;    ///< Авторизационный токен пользователя.
  QVariantMap raw;     ///< Сырые не обработанные данные пользователя.
};

typedef QSharedPointer<AuthStateData> AuthStatePtr;


class AuthState : public QObject
{
  Q_OBJECT

public:
  AuthState(QObject *parent = 0);
  inline AuthStatePtr get(const QByteArray &state) const  { return m_states.value(state); }
  void add(AuthStateData *data);

signals:
  void added(const QByteArray &state, AuthStatePtr data);

private:
  QHash<QByteArray, AuthStatePtr> m_states; ///< Таблица состояний.
};

#endif /* AUTHSTATE_H_ */
