/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef ABSTRACTPROTOCOLHANDLER_H_
#define ABSTRACTPROTOCOLHANDLER_H_

#include <boost/shared_ptr.hpp>

#include "connection.h"

/*!
 * \brief Базовый класс для обработки специфичный протоколов.
 */
class AbstractProtocolHandler {

public:
  /// Тип протокола.
  enum Type {
    Basic,              ///< Базовый протокол.
    SimpleClientServer, ///< Протокол обмена между клиентом и сервером.
    SimpleServerServer  ///< Протокол обмена между серверами.
  };

  AbstractProtocolHandler(Connection &connection, Type type = Basic);
  virtual ~AbstractProtocolHandler();
  inline Type type() const { return m_type; }
  virtual void append(quint16 opcode, const QByteArray &data);

protected:
  Connection &m_connection;

private:
  const Type m_type;
};


#endif /* ABSTRACTPROTOCOLHANDLER_H_ */
