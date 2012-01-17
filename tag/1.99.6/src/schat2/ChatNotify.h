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

#ifndef CHATNOTIFY_H_
#define CHATNOTIFY_H_

#include <QObject>
#include <QVariant>

#include "schat.h"

/*!
 * Базовый класс уведомления.
 */
class Notify
{
public:
  enum Actions {
    OpenAbout        = 0x6F61, ///< "oa" Открытие вкладки О Simple Chat.
    OpenChannel      = 0x6F63, ///< "oc" Открытие канала, команда передаёт идентификатор канала.
    OpenSettings     = 0x6F73, ///< "os" Открытие настроек.
    Quit             = 0x7163, ///< "qc" Выход из чата.
    InsertText       = 0x6974, ///< "it" Запрос на вставку текста в поле отравки.
    NetworkChanged   = 0x6E63, ///< "nc" Уведомление об изменении выбранной сети.
    NetworkSelected  = 0x6E73, ///< "ns" Изменён выбор текущей сети.
    ToggleVisibility = 0x7476, ///< "tv" Изменение состояния видимости окна.
    ShowChat         = 0x7363, ///< "sc" Запрос на открытие и активацию окна чата.
    EditTopic        = 0x6574, ///< "et" Редактирование темы канала, параметр: идентификатор канала.
    SetSendFocus     = 0x5366, ///< "Sf" Установка фокуса на поле отправки сообщения.
    CopyRequest      = 0x6372, ///< "cr" Запрос на копирование текста, отправляется полем редактирования текста в случае если в нём отсутствует выделение.
    ShowID           = 0x5349  ///< "SI" Отобразить идентификатор канала.
  };

  Notify(int type, const QVariant &data = QVariant())
  : m_type(type)
  , m_data(data)
  {}

  inline const QVariant& data() const { return m_data; }
  inline int type() const { return m_type; }

private:
  int m_type;
  QVariant m_data;
};


/*!
 * Отправка и получение внутренних уведомлений чата.
 */
class SCHAT_CORE_EXPORT ChatNotify : public QObject
{
  Q_OBJECT

public:
  ChatNotify(QObject *parent = 0);
  inline static ChatNotify *i() { return m_self; }
  inline static void start(const Notify &notify) { m_self->startNotify(notify); }
  inline static void start(int type, const QVariant &data = QVariant()) { m_self->startNotify(Notify(type, data)); }

signals:
  void notify(const Notify &notify);

private:
  void startNotify(const Notify &notify);

  static ChatNotify *m_self; ///< Указатель на себя.
};

#endif /* CHATNOTIFY_H_ */
