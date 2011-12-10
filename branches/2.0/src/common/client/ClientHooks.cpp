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

#include <QDebug>

#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientCmd.h"
#include "client/ClientHooks.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"

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
  if (index == -1 && c.endsWith(' ')) {
    c = c.left(c.size() - 1);
    index = msg.indexOf(c, 0, Qt::CaseInsensitive);
  }

  if (index == -1)
    return false;

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

  if (command == "join") {
    ChatClient::channels()->join(cmd.body());
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

  if (plain.startsWith("/me ", Qt::CaseInsensitive)) {
    ChatClient::messages()->sendText(dest, remove("/me ", text), "me");
    return true;
  }

  if (plain.startsWith("/say ", Qt::CaseInsensitive)) {
    ChatClient::messages()->sendText(dest, remove("/say ", text), "say");
    return true;
  }

  return false;
}


/*!
 * Чтение полученного сообщения.
 */
void Messages::readText(const MessagePacket &packet)
{
  if (m_hooks.isEmpty())
    return;

  foreach (Messages *hook, m_hooks) {
    hook->readText(packet);
  }
}


/*!
 * Обработка копии только что отправленного сообщения.
 */
void Messages::sendText(const MessagePacket &packet)
{
  if (m_hooks.isEmpty())
    return;

  foreach (Messages *hook, m_hooks) {
    hook->sendText(packet);
  }
}


Channels::Channels(QObject *parent)
  : QObject(parent)
{
}


void Channels::add(const ChannelInfo &info)
{
  if (m_hooks.isEmpty())
    return;

  if (ChatClient::id() == info.id())
    ChatClient::io()->setNick(ChatClient::channel()->name());

  foreach (Channels *hook, m_hooks) {
    hook->add(info);
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


void Client::setup()
{
  if (m_hooks.isEmpty())
    return;

  foreach (Client *hook, m_hooks) {
    hook->setup();
  }
}

}  // namespace Hooks
