/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.com)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOCALSERVICE_H_
#define LOCALSERVICE_H_

#include <QLocalSocket>

/*!
 * \brief Класс, обслуживающий локальные соединения.
 */
class LocalService : public QObject
{
  Q_OBJECT

public:
  LocalService(QLocalSocket *socket, QObject *parent = 0);
  ~LocalService();

private slots:
  void readyRead();

private:
  bool send(quint16 opcode);
  void unknownOpcode();

  QDataStream m_stream;
  QLocalSocket *m_socket;
  quint16 m_nextBlockSize;
  quint16 m_opcode;
};

#endif /*LOCALSERVICE_H_*/
