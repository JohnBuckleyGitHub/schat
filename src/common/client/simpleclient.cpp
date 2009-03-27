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

#include "simpleclient.h"

/*!
 * \brief Приватный D-класс для класса SimpleClient.
 */
class SimpleClient::Private
{
public:
  Private(SimpleClient *parent)
  : q(parent),
  state(SimpleClient::UnconnectedState)
  {}

  QDataStream stream;          ///< Поток для чтения данный поступивших из сокета.
  QPointer<QTcpSocket> socket; ///< Сокет.
  QString host;                ///< Адрес сервера.
  quint16 port;                ///< Порт сервера.
  SimpleClient *q;             ///< Указатель на родительский объект.
  SimpleClient::State state;   ///< Состояние подключения.
};


/*!
 * Конструктор класса SimpleClient.
 */
SimpleClient::SimpleClient(QObject *parent)
  : QObject(parent), d(new Private(this))
{
  qDebug() << this;
}


/*!
 * Деструктор класса SimpleClient.
 */
SimpleClient::~SimpleClient()
{
  qDebug() << " ~ " << this;
}


/*!
 * Текущее состояние соединения.
 */
SimpleClient::State SimpleClient::state() const
{
  return d->state;
}


/*!
 * Подключение к серверу адрес, которого был задан предварительно.
 */
void SimpleClient::link()
{
  qDebug() << this << "connect()";

  createSocket();

  if (d->socket->state() == QTcpSocket::UnconnectedState) {
    d->socket->connectToHost(d->host, d->port);
  }
}


/*!
 * Подключение к серверу с указанным адресом.
 *
 * \param remote Пара содержащая адрес и порт сервера.
 */
void SimpleClient::link(QPair<QString, quint16> remote)
{
  qDebug() << this << "connect()";
  d->host = remote.first;
  d->port = remote.second;

  link();
}


/*!
 * Слот вызывается после успешной установки соединения.
 */
void SimpleClient::connected()
{
  qDebug() << this << "connected()";
}


/*!
 * Слот вызывается после разрыва соединения.
 */
void SimpleClient::disconnected()
{
  qDebug() << this << "disconnected()";
}


/*!
 * Слот вызывается, когда доступна новая порция данных для чтения.
 */
void SimpleClient::readyRead()
{
  qDebug() << this << "readyRead()";
}


/*!
 * Функция при необходимости создаёт сокет и устанавливает соединение сигнал/слот.
 *
 * \sa link().
 */
void SimpleClient::createSocket()
{
  if (!d->socket) {
    d->socket = new QTcpSocket(this);
    d->stream.setDevice(d->socket);
    connect(d->socket, SIGNAL(connected()),    SLOT(connected()));
    connect(d->socket, SIGNAL(readyRead()),    SLOT(readyRead()));
    connect(d->socket, SIGNAL(disconnected()), SLOT(disconnected()));
  }
}
