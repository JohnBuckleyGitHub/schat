/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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
#include <QCryptographicHash>
#include <QDataStream>

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
    , m_headerOption(Protocol::BasicHeader)
  {
    m_device = stream->device();
    m_device->seek(0);

    *stream << type << quint8(0) << quint8(0) << m_headerOption;
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
    , m_headerOption(Protocol::BasicHeader)
  {
    m_device = stream->device();
    m_device->seek(0);

    if (!sender.isEmpty())
      m_headerOption |= Protocol::SenderField;

    if (!dest.isEmpty()) {
      if (dest == "bc") {
        m_headerOption |= Protocol::Broadcast;
        echo = true;
      }
      else
        m_headerOption |= Protocol::DestinationField;
    }

    if (!channel.isEmpty())
      m_headerOption |= Protocol::ChannelField;

    if (echo)
      m_headerOption |= Protocol::EnableEcho;

    *stream << type << quint8(0) << quint8(0) << m_headerOption;

    if (m_headerOption & Protocol::SenderField)
      putId(sender);

    if (m_headerOption & Protocol::DestinationField)
      putId(dest);

    if (m_headerOption & Protocol::ChannelField)
      putId(channel);
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

  template<class T>
  inline void put(const T &data) {
    *m_stream << data;
  }

  /// Запись Id.
  inline void putId(const QByteArray &data)
  {
    if (data.size() == SimpleID::DefaultSize) {
      m_device->write(data);
    }
    else {
      m_device->write(QCryptographicHash::hash("", QCryptographicHash::Sha1) += SimpleID::InvalidId);
    }
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

protected:
  QDataStream *m_stream; ///< output stream.
  QIODevice *m_device;   ///< output device.
  quint8 m_headerOption; ///< Опция заголовка.
};

#endif /* PACKETWRITER_H_ */
