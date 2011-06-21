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

#ifndef ABSTRACTMESSAGE_H_
#define ABSTRACTMESSAGE_H_

#include <QDateTime>
#include <QHash>
#include <QString>

class AbstractMessage
{
public:
  /// Тип сообщения.
  enum MessageType {
    UnknownType,     ///< Неизвестный тип.
    UserMessageType, ///< Сообщение от пользователя.
    AlertMessageType ///< Уведомления.
  };

  /// Опции обработки текста.
  enum ParseOptions {
    NoParse = 0,         ///< Не обрабатывать.
    RemoveAllHtml = 1,   ///< Удалить весь html код.
    RemoveUnSafeHtml = 2 ///< Удалить небезопасный html код.
  };

  /// Направление сообщения.
  enum Direction {
    UnknownDirection,
    OutgoingDirection,
    IncomingDirection
  };

  AbstractMessage(int type, const QString &text = QString(), const QByteArray &destId = QByteArray(), int parseOptions = NoParse);
  inline int direction() const { return m_direction; }
  inline int type() const { return m_type; }
  inline QByteArray destId() const { return m_destId; }
  inline QByteArray senderId() const { return m_senderId; }
  inline qint64 timestamp() const { return m_timestamp; }
  inline QString text() const { return m_text; }
  static QString tpl(const QString &fileName);
  virtual QString js() const;
  void setText(const QString &text, int parseOptions);

protected:
  QDateTime dateTime() const;
  QString appendMessage(QString &html) const;
  void replaceText(QString &html) const;
  void replaceTimeStamp(QString &html) const;

  int m_direction;       ///< Направление сообщения.
  int m_parseOptions;    ///< Опции обработки сообщения.
  QByteArray m_destId;   ///< Идентификатор назначения.
  QByteArray m_senderId; ///< Идентификатор отправителя.
  qint64 m_timestamp;    ///< Отметка времени.
  QString m_text;        ///< Текст сообщения.

private:
  int m_type;                                 ///< Тип сообщения.
  static QHash<QString, QString> m_templates; ///< Таблица шаблонов для сообщений.
};

#endif /* ABSTRACTMESSAGE_H_ */
