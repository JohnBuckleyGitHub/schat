/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#ifndef ABSTRACTPEER_H_
#define ABSTRACTPEER_H_

#include <QObject>

/*!
 * \brief Абстрактный базовый каркас для клиентского и серверного подключения.
 */
class AbstractPeer : public QObject
{
  Q_OBJECT

public:
  AbstractPeer(QObject *parent = 0);
  ~AbstractPeer();
  inline qint64 rx() const { return m_rx; }
  inline qint64 tx() const { return m_tx; }

protected:
  qint64 m_rx; ///< Счётчик полученных (receive) байт.
  qint64 m_tx; ///< Счётчик отправленных (transmit) байт.
};

#endif /* ABSTRACTPEER_H_ */
