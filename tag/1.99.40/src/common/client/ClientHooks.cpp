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

#include <QDebug>

#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientCmd.h"
#include "client/ClientFeeds.h"
#include "client/ClientHooks.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"
#include "net/SimpleID.h"
#include "sglobal.h"

namespace Hooks
{

Messages::Messages(QObject *parent)
  : QObject(parent)
{
}


QString Messages::remove(const QString &cmd, const QString &msg)
{
  QString c = cmd;
  int index = msg.indexOf(c, 0, Qt::CaseInsensitive);
  if (index == -1 && c.endsWith(LC(' '))) {
    c = c.left(c.size() - 1);
    index = msg.indexOf(c, 0, Qt::CaseInsensitive);
  }

  if (index == -1)
    return msg;

  QString out = msg;
  out.remove(index, c.size());
  return out;
}


/*!
 * Обработка команды в тексте.
 *
 * \param dest Идентификатор получателя.
 * \param cmd  Команда и тело команды.
 *
 * \return \b true если команда обработана.
 */
bool Messages::command(const QByteArray &dest, const ClientCmd &cmd)
{
  if (m_hooks.isEmpty())
    return false;

  for (int i = 0; i < m_hooks.size(); ++i) {
    if (m_hooks.at(i)->command(dest, cmd))
      return true;
  }

  QString command = cmd.command().toLower();

  if (command == LS("join")) {
    if (cmd.isBody())
      ChatClient::channels()->join(cmd.body());
    else
      ChatClient::channels()->join(dest);

    return true;
  }

  if (command == LS("nick")) {
    ChatClient::channels()->nick(cmd.body());
    return true;
  }

  if (command == LS("name")) {
    ChatClient::channels()->name(dest, cmd.body());
    return true;
  }

  if (command == LS("part")) {
    ChatClient::channels()->part(dest);
    return true;
  }

  return false;
}


/*!
 * Обработка команд требующих сохранения форматирования сообщения.
 *
 * \param dest  Идентификатор получателя.
 * \param text  Текст сообщения.
 * \param plain Текст очищенный от HTML тегов.
 *
 * \return \b true если команда обработана.
 */
bool Messages::command(const QByteArray &dest, const QString &text, const QString &plain)
{
  if (m_hooks.isEmpty())
    return false;

  for (int i = 0; i < m_hooks.size(); ++i) {
    if (m_hooks.at(i)->command(dest, text, plain))
      return true;
  }

  if (plain.startsWith(LS("/me "), Qt::CaseInsensitive)) {
    ChatClient::messages()->sendText(dest, remove(LS("/me "), text), LS("me"));
    return true;
  }

  if (plain.startsWith(LS("/say "), Qt::CaseInsensitive)) {
    ChatClient::messages()->sendText(dest, remove(LS("/say "), text), LS("say"));
    return true;
  }

  return false;
}


/*!
 * Чтение полученного сообщения.
 *
 * \param packet Пакет с сообщением.
 *
 * \return 0 если пакет не был обработан или 1 если обработан.
 */
int Messages::readText(MessagePacket packet)
{
  if (m_hooks.isEmpty())
    return 0;

  int matches = 0;
  foreach (Messages *hook, m_hooks) {
    matches += hook->readText(packet);
  }

  if (matches)
    return matches;

  foreach (Messages *hook, m_hooks) {
    hook->unhandled(packet);
  }

  return 0;
}


/*!
 * Обработка копии только что отправленного сообщения.
 */
void Messages::sendText(MessagePacket packet)
{
  if (m_hooks.isEmpty())
    return;

  foreach (Messages *hook, m_hooks) {
    hook->sendText(packet);
  }
}


void Messages::unhandled(MessagePacket /*packet*/) const
{
}


Channels::Channels(QObject *parent)
  : QObject(parent)
{
}


ClientChannel Channels::get(const QByteArray &id)
{
  if (m_hooks.isEmpty())
    return ClientChannel();

  ClientChannel channel;
  foreach (Channels *hook, m_hooks) {
    channel = hook->get(id);
    if (channel)
      return channel;
  }

  return channel;
}


void Channels::add(ClientChannel channel, const ChannelInfo &info, const QVariantMap &json)
{
  if (m_hooks.isEmpty())
    return;

  if (ChatClient::id() == info.id())
    ChatClient::io()->setNick(ChatClient::channel()->name());

  channel->setSynced(true);
  if (channel->type() == SimpleID::ChannelId)
    channel->status() = Status::Online;

  ChatClient::feeds()->hooks()->add(channel, info, json);

  foreach (Channels *hook, m_hooks) {
    hook->add(channel, info, json);
  }
}


Client::Client(QObject *parent)
  : QObject(parent)
{
}


bool Client::openId(const QByteArray &id, bool &matched)
{
  if (m_hooks.isEmpty())
    return false;

  foreach (Client *hook, m_hooks) {
    bool result = hook->openId(id, matched);
    if (matched)
      return result;
  }

  return false;
}


/*!
 * Получение идентификатора пользователя.
 * Эта функция будет вызвана только если не удалось получить
 * идентификатор пользователя стандартым методом.
 */
QByteArray Client::id()
{
  if (m_hooks.isEmpty())
    return QByteArray();

  foreach (Client *hook, m_hooks) {
    QByteArray id = hook->id();
    if (!id.isEmpty())
      return id;
  }

  return QByteArray();
}


/*!
 * Получение идентификатора сервера.
 * Эта функция будет вызвана только если не удалось получить
 * идентификатор сервера стандартым методом.
 */
QByteArray Client::serverId()
{
  if (m_hooks.isEmpty())
    return QByteArray();

  foreach (Client *hook, m_hooks) {
    QByteArray id = hook->serverId();
    if (!id.isEmpty())
      return id;
  }

  return QByteArray();
}


void Client::restore()
{
  if (m_hooks.isEmpty())
    return;

  foreach (Client *hook, m_hooks) {
    hook->restore();
  }
}


void Client::setup()
{
  if (m_hooks.isEmpty())
    return;

  foreach (Client *hook, m_hooks) {
    hook->setup();
  }
}


Feeds::Feeds(QObject *parent)
  : QObject(parent)
{
}


void Feeds::add(ClientChannel channel, const ChannelInfo &info, const QVariantMap &json)
{
  foreach (Feeds *hook, m_hooks) {
    hook->addImpl(channel, info, json);
  }
}


/*!
 * Чтение полученного сообщения.
 */
void Feeds::readFeed(const FeedNotice &packet)
{
  if (m_hooks.isEmpty())
    return;

  foreach (Feeds *hook, m_hooks) {
    hook->readFeedImpl(packet);
  }
}


/*!
 * Получение списка фидов, для которых требуется синхронизация.
 *
 * \param channel канал фидов.
 * \param feeds   заголовки фидов.
 * \param extra   обязательные фиды, если они отсутствуют у канала то попадут в список.
 */
QStringList Feeds::unsynced(ClientChannel channel, const QVariantMap &feeds, const QStringList &extra)
{
  QStringList out;
  QMapIterator<QString, QVariant> i(feeds);
  while (i.hasNext()) {
    i.next();
    FeedPtr feed = channel->feed(i.key(), false);
    if (!feed) {
      if (extra.contains(i.key()))
        out.append(i.key());

      continue;
    }

    qint64 date = 0;
    const QVariant &value = i.value();

    if (value.type() == QVariant::LongLong)
      date = value.toLongLong();
    else if (value.type() == QVariant::Map)
      date = value.toMap().value(LS("date")).toLongLong();

    if (date == 0)
      continue;

    if (feed->head().date() != date)
      out.append(i.key());
  }

  return out;
}


void Feeds::addImpl(ClientChannel /*channel*/, const ChannelInfo & /*info*/, const QVariantMap & /*json*/)
{
}


void Feeds::readFeedImpl(const FeedNotice & /*packet*/)
{
}

}  // namespace Hooks
