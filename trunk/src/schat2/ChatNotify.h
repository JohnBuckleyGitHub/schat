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
#include <QQueue>
#include <QSharedPointer>
#include <QVariant>

#include "schat.h"

class FeedNotice;

/*!
 * Базовый класс уведомления.
 */
class SCHAT_CORE_EXPORT Notify
{
public:
  enum Actions {
    OpenAbout        = 0x6F61, ///< "oa" Открытие вкладки О Simple Chat.
    OpenChannel      = 0x6F63, ///< "oc" Открытие канала, команда передаёт идентификатор канала.
    OpenInfo         = 0x4F49, ///< "OI" Открытие информации о канале.
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
    ShowID           = 0x5349, ///< "SI" Отобразить идентификатор канала.
    ServerRenamed    = 0x5352, ///< "SR" Сервер сменил имя.
    FeedData         = 0x4644, ///< "FD" Полученные данные фида.
    FeedReply        = 0x4652, ///< "FR" Успешный ответ на запрос к фиду.
    QueryError       = 0x5145, ///< "QE" Ошибка запроса к фиду.
    ClearChat        = 0x4343, ///< "CC" Очистка текущего разговора.
    ClearCache       = 0x4363, ///< "Cc" Очистка кешей.
    Language         = 0x4C61, ///< "La" Изменение языка.
    ChannelTabClosed = 0x7443, ///< "tC" Уведомление о закрытии вкладки канала.
    Restart          = 0x5273  ///< "Rs" Перезапуск чата.
  };

  Notify(int type, const QVariant &data = QVariant())
  : m_type(type)
  , m_data(data)
  {}

  virtual ~Notify() {}

  inline const QVariant& data() const { return m_data; }
  inline int type() const             { return m_type; }

protected:
  int m_type;
  QVariant m_data;
};

typedef QSharedPointer<Notify> NotifyPtr;


/*!
 * Уведомления связанные с фидами.
 */
class SCHAT_CORE_EXPORT FeedNotify : public Notify
{
public:
  FeedNotify(const QByteArray &channel, const FeedNotice *packet);
  FeedNotify(int type, const QByteArray &channel, const QString &name, const QVariantMap &json = QVariantMap(), int status = 200);
  bool match(const QByteArray &id, const QString &name, const QString &action = QString()) const;
  bool match(const QString &name, const QString &action = QString()) const;
  inline const QByteArray& channel() const { return m_channel; }
  inline const QString command() const     { return m_command; }
  inline const QString& name() const       { return m_name; }
  inline const QVariantMap& json() const   { return m_json; }
  inline int status() const                { return m_status; }
  QString action() const;

private:
  int m_status;         ///< Статус.
  QByteArray m_channel; ///< Идентификатор канала.
  QString m_command;    ///< Команда.
  QString m_name;       ///< Имя фида.
  QVariantMap m_json;   ///< JSON данные.
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
  static void start(const Notify &notify);
  static void start(int type, const QVariant &data = QVariant(), bool queued = false);
  static void start(Notify *notify);

signals:
  void notify(const Notify &notify);

private slots:
  void start();

private:
  void startNotify(const Notify &notify);
  void startNotify(Notify *notify);

  QQueue<NotifyPtr> m_queue; ///< Очередь отправки.
  static ChatNotify *m_self; ///< Указатель на себя.
};

#endif /* CHATNOTIFY_H_ */
