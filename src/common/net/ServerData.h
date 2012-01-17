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

#ifndef SERVERDATA_H_
#define SERVERDATA_H_

#include <QString>

#include "schat.h"

/*!
 * Данные сервера.
 *
 * \deprecated Этот класс должен быть заменён на канал сервера и фиды.
 */
class SCHAT_EXPORT ServerData
{
public:
  /// Ограничения.
  enum Limits {
    MinNameLengh = 4,     ///< Минимальная длина имени.
    MaxNameLength = 63,   ///< Максимальная длина имени.
  };

  /// Возможности сервера.
  enum Features {
    NoFeatures = 0,         ///< Только базовые возможности.
    AutoJoinSupport = 1,    ///< Сервер поддерживает основной канал, для автоматического входа клиента в него (на усмотрение клиента).
    PasswordAuthSupport = 2 ///< Сервер поддерживает авторизацию по имени и паролю, этот флаг автоматически устанавливается если для сервера установлено корректное имя.
  };

  ServerData();
  bool setChannelId(const QByteArray &id);
  bool setName(const QString &name);
  inline bool is(quint32 feature) const { return m_features & feature; }
  inline QByteArray channelId() const { return m_channelId; }
  inline QByteArray id() const { return m_id; }
  inline QByteArray privateId() const { return m_privateId; }
  inline QString name() const { return m_name; }
  inline quint32 features() const { return m_features; }
  inline quint8 number() const { return m_number; }
  inline void setFeatures(quint32 features) { m_features = features; }
  inline void setId(const QByteArray &id) { m_id = id; }
  inline void setNumber(quint8 number) { m_number = number; }
//  void setPrivateId(const QByteArray &id);

private:
  QByteArray m_channelId; ///< Идентификатор главного канала.
  QByteArray m_id;        ///< Публичный идентификатор сервера.
  QByteArray m_privateId; ///< Приватный идентификатор сервера.
  QString m_name;         ///< Имя сервера.
  quint32 m_features;     ///< Возможности сервера.
  quint8 m_number;        ///< Номер сервера, по умолчанию 0, для вторичных серверов должно быть уникальное в пределах сети значение отличное от 0.
};

#endif /* SERVERDATA_H_ */
