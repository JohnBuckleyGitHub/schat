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

#ifndef PACKETWRITER_H_
#define PACKETWRITER_H_

#include <QByteArray>
#include <QDataStream>

#include "JSON.h"
#include "net/Protocol.h"
#include "net/SimpleID.h"

/*!
 * Класс выполняющий запись виртуального пакета.
 */
class PacketWriter
{
public:
  /*!
   * Создание виртуального пакета и запись заголовка.
   *
   * \param stream   output stream.
   * \param type     Тип пакета.
   */
  inline PacketWriter(QDataStream *stream, quint16 type)
    : m_stream(stream)
    , m_option(Protocol::BasicHeader)
  {
    m_device = stream->device();
    m_device->seek(0);

    *stream << type << quint8(0) << m_option;
  }

  /*!
   * Создание виртуального пакета и запись заголовка.
   *
   * \param stream   output stream.
   * \param type     Тип пакета.
   * \param sender   Идентификатор отправителя.
   * \param dest     Идентификатор получателя, специальное значение "bc" устанавливает опцию Protocol::Broadcast.
   * \param channel  Идентификатор канала.
   * \param echo     true для включения опции Protocol::EnableEcho.
   */
  inline PacketWriter(QDataStream *stream, quint16 type, const QByteArray &sender, const QByteArray &dest = QByteArray(), const QByteArray &channel = QByteArray(), bool echo = false)
    : m_stream(stream)
    , m_option(Protocol::BasicHeader)
  {
    m_device = stream->device();
    m_device->seek(0);

    if (!sender.isEmpty())
      m_option |= Protocol::SenderField;

    if (!dest.isEmpty()) {
      m_option |= Protocol::DestinationField;
      if (dest.size() > 1 || SimpleID::typeOf(dest) == SimpleID::ChannelId)
        m_option |= Protocol::Multicast;
    }

    if (!channel.isEmpty())
      m_option |= Protocol::ChannelField;

    if (echo)
      m_option |= Protocol::EnableEcho;

    *stream << type << quint8(0) << m_option;

    if (m_option & Protocol::SenderField)
      putId(sender);

    if (m_option & Protocol::ChannelField)
      putId(channel);

    if (m_option & Protocol::DestinationField)
      putId(QList<QByteArray>() << dest);
  }

  /*!
   * Создание виртуального пакета и запись заголовка.
   *
   * \param stream   output stream.
   * \param type     Тип пакета.
   * \param sender   Идентификатор отправителя.
   * \param dest     Идентификатор получателя, специальное значение "bc" устанавливает опцию Protocol::Broadcast.
   * \param echo     true для включения опции Protocol::EnableEcho.
   */
  inline PacketWriter(QDataStream *stream, quint16 type, const QByteArray &sender, const QList<QByteArray> &dest = QList<QByteArray>(), bool echo = false)
    : m_stream(stream)
    , m_option(Protocol::SenderField)
  {
    m_device = stream->device();
    m_device->seek(0);

    if (!dest.isEmpty()) {
      m_option |= Protocol::DestinationField;
      if (dest.size() > 1 || SimpleID::typeOf(dest.at(0)) == SimpleID::ChannelId)
        m_option |= Protocol::Multicast;
    }

    if (echo)
      m_option |= Protocol::EnableEcho;

    *stream << type << quint8(0) << m_option;
     putId(sender);

    if (m_option & Protocol::DestinationField)
      putId(dest);
  }

  /*!
   * Возвращает результат работы класса, тело пакета.
   */
  inline QByteArray data() const
  {
    int size = m_device->pos();
    m_device->seek(0);
    return m_device->peek(size);
  }

  /// Запись UTF-8 строки.
  inline void put(const QString &text) {
    *m_stream << text.toUtf8();
  }

  inline void put(const QVariant &data) {
    *m_stream << JSON::generate(data);
  }

  inline void put(const QVariantMap &data) {
    *m_stream << JSON::generate(data);
  }

  template<class T>
  inline void put(const T &data) {
    *m_stream << data;
  }

  /// Запись Id.
  inline void putId(const QByteArray &data)
  {
    if (data.size() == SimpleID::DefaultSize)
      m_device->write(data);
    else
      m_device->write(SimpleID::make("", SimpleID::InvalidId));
  }

  /// Запись Id.
  inline void putId(const QList<QByteArray> &data) {
    if (data.isEmpty())
      return;

    put<quint32>(data.size());
    for (int i = 0; i < data.size(); ++i) {
      putId(data.at(i));
    }
  }

  inline bool is(int option) { return m_option & option; }

protected:
  QDataStream *m_stream; ///< output stream.
  QIODevice *m_device;   ///< output device.
  quint16 m_option;      ///< Опция заголовка.
};

#endif /* PACKETWRITER_H_ */
