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

#ifndef CHATALERTS_H_
#define CHATALERTS_H_

#include <QObject>
#include <QVariant>

#include "schat.h"

/*!
 * Базовый класс для оповещений чата.
 */
class SCHAT_CORE_EXPORT Alert
{
public:
  /// Опции оповещения.
  enum Option {
    NoOptions = 0, ///< Нет опций.
    Tab       = 1, ///< Оповещение меняет состояние вкладки.
    Global    = 2  ///< Оповещение запускает глобальное оповещение.
  };

  Q_DECLARE_FLAGS(Options, Option)

  Alert(const QString &type, Options options = NoOptions);
  Alert(const QString &type, const QByteArray &id, qint64 date, Options options = NoOptions);
  Alert(const QString &type, qint64 date, Options options = NoOptions);
  virtual ~Alert() {}

  inline const QByteArray id() const       { return m_id; }
  inline const QByteArray& tab() const     { return m_tab; }
  inline const QString& type() const       { return m_type; }
  inline const QVariantMap& data() const   { return m_data; }
  inline Options options() const           { return m_options; }
  inline qint64 date() const               { return m_date; }

  inline QVariantMap& data()               { return m_data; }
  inline void setTab(const QByteArray &id) { m_tab = id; }
  void setTab(const QByteArray &sender, const QByteArray &dest);

protected:
  Options m_options;   ///< Опции оповещения.
  QByteArray m_id;     ///< Уникальный идентификатор оповещения.
  QByteArray m_tab;    ///< Идентификатор вкладки канала.
  qint64 m_date;       ///< Отметка времени.
  QString m_type;      ///< Тип оповещения.
  QVariantMap m_data;  ///< JSON данные.
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Alert::Options)


class SCHAT_CORE_EXPORT ChatAlerts : public QObject
{
  Q_OBJECT

public:
  ChatAlerts(QObject *parent = 0);
  inline static bool hasAlerts()                  { return !m_self->m_channels.isEmpty(); }
  inline static ChatAlerts *i()                   { return m_self; }
  static void add(const QByteArray &id);
  static void remove(const QByteArray &id);
  static void start(const Alert &alert);

signals:
  void alert(bool alert);
  void alert(const Alert &alert);

private slots:
  void offline();
  void online();

private:
  static ChatAlerts *m_self;           ///< Указатель на себя.
  static QList<QByteArray> m_channels; ///< Список каналов для которых активно глобальное уведомление о новых сообщениях.
};

#endif /* CHATALERTS_H_ */
