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

#ifndef AUTHBRIDGE_H_
#define AUTHBRIDGE_H_

#include <QObject>
#include <QVariant>

#include "schat.h"

class AuthClient;

/*!
 * Мост между C++ и JavaScript кодом для поддержки авторизации.
 */
class SCHAT_CORE_EXPORT AuthBridge : public QObject
{
  Q_OBJECT

public:
  AuthBridge(QObject *parent = 0);
  Q_INVOKABLE bool anonymous() const;
  Q_INVOKABLE QVariantMap providers() const { return m_providers; }
  Q_INVOKABLE void cancel();
  Q_INVOKABLE void open();
  void start(const QString &url);

signals:
  void providersReady();

private slots:
  void providersReady(const QVariantMap &data);

private:
  AuthClient *m_client;    ///< HTTP клиент для авторизации.
  QVariantMap m_providers; ///< Провайдеры авторизации.
};

#endif /* AUTHBRIDGE_H_ */
