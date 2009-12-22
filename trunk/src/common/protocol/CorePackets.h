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

#ifndef COREPACKETS_H_
#define COREPACKETS_H_

#include <QByteArray>
#include <QList>
#include <QString>

/*!
 * \brief Абстрактный сырой пакет.
 */
class AbstractRawPacket
{
public:
  AbstractRawPacket(quint16 opcode = 0);
  virtual ~AbstractRawPacket();

  bool read(quint16 opcode, const QByteArray &body);
  inline bool isValid() const            { return m_valid; }
  inline quint16 opcode() const          { return m_opcode; }
  inline static QList<quint16> opcodes() { return QList<quint16>() << 0; }
  QByteArray data() const;

protected:
  virtual bool readStream(QDataStream *stream);
  virtual void writeStream(QDataStream *stream) const;

  bool m_valid;     ///< \a true в случае если пакет корректен.
  quint16 m_opcode; ///< Код пакета.
};


/*!
 * \brief Отправка сообщения.
 */
class MessagePacket : public AbstractRawPacket
{
public:
  MessagePacket();
  inline QString channel() const           { return m_channel; }
  inline QString message() const           { return m_message; }
  inline QString nick() const              { return m_nick; }
  inline void setNick(const QString &nick) { m_nick = nick; }
  static QList<quint16> opcodes() { return QList<quint16>() << 200; }

protected:
  bool readStream(QDataStream *stream);
  void writeStream(QDataStream *stream) const;

  QString m_nick;    ///< Локальный ник.
  QString m_channel; ///< Канал (ник) для кого предназначено сообщение.
  QString m_message; ///< Сообщение.
};

#endif /* COREPACKETS_H_ */
