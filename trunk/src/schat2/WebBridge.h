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

#ifndef WEBBRIDGE_H_
#define WEBBRIDGE_H_

#include <QObject>
#include <QVariant>

#include "schat.h"

class SCHAT_CORE_EXPORT WebBridge : public QObject
{
  Q_OBJECT

public:
  WebBridge(QObject *parent = 0);
  inline static WebBridge *i() { return m_self; }
  Q_INVOKABLE QString channel(const QString &id);
  Q_INVOKABLE QString feed(const QString &name);
  Q_INVOKABLE void setTabPage(const QString &id, int page);
  static QVariantMap channel(const QByteArray &id);

private:
  static WebBridge *m_self; ///< Указатель на себя.
};

#endif /* WEBBRIDGE_H_ */
