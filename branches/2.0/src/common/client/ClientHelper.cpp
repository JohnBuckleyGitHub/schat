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

#include "client/ClientCmd.h"
#include "client/ClientHelper.h"
#include "client/SimpleClient.h"
#include "net/packets/message.h"

ClientHelper::ClientHelper(SimpleClient *client)
  : QObject(client)
  , m_richText(false)
  , m_name(1)
  , m_client(client)
{
  m_commands.append(QLatin1String("me"));
  m_commands.append(QLatin1String("topic"));
}


/*!
 * Отправка сообщения, если в сообщении содержаться команды, то они будут обработаны.
 */
bool ClientHelper::send(MessageData &data)
{
  if (data.text.isEmpty())
    return false;

  m_destId = data.destId();
  QString text;

  if (m_richText)
    text = MessageUtils::toPlainText(data.text);
  else
    text = data.text.simplified();

  for (int i = 0; i < m_commands.size(); ++i) {
    int result = command(data, m_commands.at(i), text);
    if (result == 0)
      continue;

    if (result == 1) {
      sendText(data);
      return true;
    }

    if (result == 2)
      return false;
  }

  if (text.at(0) != QLatin1Char('/')) {
    sendText(data);
    return true;
  }

  QStringList commands = (QLatin1String(" ") + text).split(QLatin1String(" /"), QString::SkipEmptyParts);
  for (int i = 0; i < commands.size(); ++i) {
    ClientCmd cmd(commands.at(i));
    if (cmd.isValid())
      command(cmd);
  }

  return true;
}


/*!
 * Отправка сообщения без обработки команд и без изменения текста.
 */
bool ClientHelper::sendText(MessageData &data)
{
  if (data.destId().isEmpty())
    return false;

  if (data.senderId.isEmpty())
    data.senderId = m_client->userId();

  ++m_name;
  data.id = SimpleID::randomId(SimpleID::MessageId, m_client->userId());
  data.autoSetOptions();

  if (!m_client->send(data)) {
    --m_name;
    return false;
  }

  return true;
}


/*!
 * Обработчик команд требующих полную поддержку html текста, например команда "/me".
 *
 * \param data Данные сообщения.
 * \param cmd  Команда без начального слэша.
 * \param text Текст, очищенный от html тегов.
 *
 * \return Результат обработки команды
 *  - 0 команда не найдена,
 *  - 1 команда найдена
 *  - 2 команда найдена и обработана.
 */
int ClientHelper::command(MessageData &data, const QString &cmd, const QString &text)
{
  QString full = QLatin1String("/") + cmd + QLatin1String(" ");
  if (text.startsWith(full, Qt::CaseInsensitive)) {
    if (MessageUtils::remove(full, data.text))
      data.command = cmd;

    return 1;
  }

  return 0;
}


void ClientHelper::command(const ClientCmd &cmd)
{
  Q_UNUSED(cmd)
}
