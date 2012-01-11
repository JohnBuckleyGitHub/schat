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

#ifndef NODEPOOL_H_
#define NODEPOOL_H_

#include <QStringList>
#include <QThread>

#include "schat.h"

class SCHAT_EXPORT NodePool : public QThread
{
  Q_OBJECT

public:
  NodePool(const QStringList &listen, QObject *core, QObject *parent = 0);

protected:
  void run();

private slots:
  void newConnection(int socketDescriptor);

private:
  QObject *m_core;      ///< Указатель на ядро чата.
  QStringList m_listen; ///< Список пар адресов и портов на которых сервер будет принимать подключения.
};

#endif /* NODEPOOL_H_ */
