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

#ifndef CHATALERTS_H_
#define CHATALERTS_H_

#include <QObject>
#include <QVariant>

#include "schat.h"

class SCHAT_CORE_EXPORT Alert
{
public:
  enum Type {
    PublicMessage  = 0x434D, ///< "CM" Сообщение в канале от другого пользователя.
    PrivateMessage = 0x504D, ///< "PM" Приватное сообщение от другого пользователя.
    Connected      = 0x436F, ///< "Co" Уведомление об успешном подключении к серверу.
    ConnectionLost = 0x434C  ///< "CL" Обработка потери соединения.
  };

  Alert(int type);
  Alert(int type, const QByteArray &id, qint64 date);
  virtual ~Alert() {}

  inline const QByteArray id() const     { return m_id; }
  inline const QByteArray& tab() const   { return m_tab; }
  inline const QVariantMap& data() const { return m_data; }
  inline int type() const                { return m_type; }
  inline qint64 date() const             { return m_date; }

  inline QVariantMap& data()             { return m_data; }

protected:
  int m_type;          ///< Тип оповещения.
  QByteArray m_id;     ///< Уникальный идентификатор оповещения.
  QByteArray m_tab;    ///< Идентификатор вкладки канала.
  qint64 m_date;       ///< Отметка времени.
  QVariantMap m_data;  ///< JSON данные.
};


class SCHAT_CORE_EXPORT ChatAlerts : public QObject
{
  Q_OBJECT

public:
  ChatAlerts(QObject *parent = 0);
  inline static bool hasAlerts()                  { return !m_self->m_channels.isEmpty(); }
  inline static ChatAlerts *i()                   { return m_self; }
  inline static void add(const QByteArray &id)    { m_self->addImpl(id); }
  inline static void remove(const QByteArray &id) { m_self->removeImpl(id); }
  inline static void start(const Alert &alert)    { m_self->startAlert(alert); }

signals:
  void alert(bool alert);
  void alert(const Alert &alert);

private slots:
  void offline();
  void online();

private:
  void startAlert(const Alert &alert);
  void addImpl(const QByteArray &id);
  void removeImpl(const QByteArray &id);

  QList<QByteArray> m_channels; ///< Список каналов для которых активно уведомление о новых сообщениях.
  static ChatAlerts *m_self;    ///< Указатель на себя.
};

#endif /* CHATALERTS_H_ */
