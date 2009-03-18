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

#include <QtCore>
#include <QtNetwork>

#include "chatdaemon.h"
#include "chatserver.h"

/*!
 * \brief Конструктор класса ChatDaemon.
 * \param parent Указатель на родительский объект.
 */
ChatDaemon::ChatDaemon(QObject *parent)
  : QObject(parent)
{
  qDebug() << this;
}


ChatDaemon::~ChatDaemon()
{
  qDebug() << "~" << this;
}


/*!
 * Запуск сервера.
 *
 * Функция читает настройки сервера, устанавливает параметры логирования каналов
 * и производит запуск сервера \a m_server.
 * Данные о попытке запуска заносятся в лог файл.
 * \return \a true в случае успешного запуска, и \a false при возникновении любой ошибки.
 */
bool ChatDaemon::start()
{
  qDebug() << this << "start()";
  ChatServer server("0.0.0.0", 7777, 2);
  server.run();

  return true;
}
