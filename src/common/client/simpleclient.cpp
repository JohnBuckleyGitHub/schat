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

#include "protocol4.h"
#include "simpleclient.h"
#include "usertools.h"

/*!
 * \brief Приватный D-класс для класса SimpleClient.
 */
class SimpleClient::Private
{
public:
  Private(SimpleClient *parent)
  : uniqueId(getUniqueId()),
  gender(protocol::Male),
  q(parent),
  state(SimpleClient::UnconnectedState)
  {}

  const QByteArray getUniqueId() const;

  const QByteArray uniqueId;   ///< Уникальный идентификатор, используется для анонимных пользователей, вместо пароля.
  protocol::Gender gender;     ///< Пол пользователя.
  QByteArray password;         ///< SHA1-хеш пароля для зарегистрированного пользователя.
  QDataStream stream;          ///< Поток для чтения данный поступивших из сокета.
  QPointer<QTcpSocket> socket; ///< Сокет.
  QString fullName;            ///< Полное имя пользователя, игнорируются для зарегистрированных.
  QString host;                ///< Адрес сервера.
  QString nick;                ///< Ник пользователя.
  QString userName;            ///< Логин зарегистрированного пользователя.
  quint16 port;                ///< Порт сервера.
  SimpleClient *q;             ///< Указатель на родительский объект.
  SimpleClient::State state;   ///< Состояние подключения.
};


/*!
 * Получение уникального идентификатора на основе mac-адреса первого сетевого адаптера.
 *
 * \return SHA1 хеш mac-адреса, либо пустой массив в случае ошибки.
 */
const QByteArray SimpleClient::Private::getUniqueId() const
{
  QList<QNetworkInterface> all = QNetworkInterface::allInterfaces();
  foreach (QNetworkInterface iface, all) {
    QString hw = iface.hardwareAddress();
    if (!hw.isEmpty())
      return QCryptographicHash::hash(hw.toLatin1(), QCryptographicHash::Sha1);
  }

  return QByteArray();
}


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
 * Установка полного имени пользователя.
 */
void SimpleClient::setFullName(const QString &fullName)
{
  d->fullName = UserTools::fullName(fullName);
}


/*!
 * Установка пола пользователя.
 */
void SimpleClient::setGender(quint8 gender)
{
  d->gender = static_cast<protocol::Gender>(gender);
}


/*!
 * Установка ника пользователя.
 */
void SimpleClient::setNick(const QString &nick)
{
  d->nick = UserTools::nick(nick);
}


/*!
 * Установка пароля пользователя.
 */
void SimpleClient::setPassword(const QString &password)
{
  if (password.isEmpty())
    d->password = QByteArray();
  else
    d->password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1);
}


/*!
 * Установка имени пользователя.
 */
void SimpleClient::setUserName(const QString &userName)
{
  d->userName = userName;
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
