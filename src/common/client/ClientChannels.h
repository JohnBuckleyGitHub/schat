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

#ifndef CLIENTCHANNELS_H_
#define CLIENTCHANNELS_H_

#include <QObject>

#include "Channel.h"

class ChannelNotice;
class SimpleClient;

namespace Hooks
{
  class Channels;
}


class ChannelInfo
{
public:
  /// Информация об создании канала.
  enum Option {
    New,     ///< Канал создан в результате чтения пакета и ранее был не известен клиенту.
    Updated, ///< Канал существовал ранее и был обновлён.
    Renamed  ///< Канал существовал ранее и был обновлён и переименован.
  };

  ChannelInfo(const QByteArray &id)
  : m_option(New)
  , m_id(id)
  {}

  inline const QByteArray& id() const { return m_id; }
  inline int option() const           { return m_option; }
  inline void setOption(int option)   { m_option = option; }

private:
  int m_option;
  QByteArray m_id;
};


class SCHAT_EXPORT ClientChannels : public QObject
{
  Q_OBJECT

public:
  ClientChannels(QObject *parent = 0);
  inline Hooks::Channels *hooks() const { return m_hooks; }
  inline const QHash<QByteArray, ClientChannel>& channels() const { return m_channels; }

  bool info(const QList<QByteArray> &channels);
  bool join(const QByteArray &id);
  bool join(const QString &name);
  bool name(const QByteArray &id, const QString &name);
  bool nick(const QString &nick);
  bool part(const QByteArray &id);
  bool update();
  ClientChannel get(const QByteArray &id);

signals:
  void channel(const ChannelInfo &info);                          ///< Общая информация о канале.
  void channel(const QByteArray &id);                             ///< Команда "channel".
  void channels(const QList<QByteArray> &channels);               ///< Пакет новых каналов.
  void joined(const QByteArray &channel, const QByteArray &user); ///< Команда "+".
  void notice(const ChannelNotice &notice);                       ///< Сырой пакет.
  void part(const QByteArray &channel, const QByteArray &user);   ///< Команда "-".
  void quit(const QByteArray &user);                              ///< Команда "quit".

private slots:
  void clientStateChanged(int state, int previousState);
  void idle();
  void notice(int type);
  void restore();
  void setup();

private:
  ClientChannel add();
  void channel();
  void info();
  void joined();
  void part();
  void quit();
  void sync(ClientChannel channel);

  ChannelNotice *m_packet;                     ///< Текущий прочитанный пакет.
  Hooks::Channels *m_hooks;                    ///< Хуки.
  QHash<QByteArray, ClientChannel> m_channels; ///< Таблица каналов.
  QList<QByteArray> m_joined;                  ///< Список каналов в которых находится клиент.
  QList<QByteArray> m_synced;                  ///< Список синхронизированных каналов.
  QList<QByteArray> m_unsynced;                ///< Список не синхронизированных каналов.
  SimpleClient *m_client;                      ///< Клиент чата.
};

#endif /* CLIENTCHANNELS_H_ */
