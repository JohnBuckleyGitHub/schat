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

#include <QDateTime>

#include "debugstream.h"

#include "client/ClientCmd.h"
#include "client/ClientHelper.h"
#include "client/SimpleClient.h"
#include "net/packets/message.h"
#include "net/packets/notices.h"
#include "text/PlainTextFilter.h"

ClientHelper::ClientHelper(SimpleClient *client)
  : QObject(client)
  , m_richText(false)
  , m_client(client)
{
  m_commands.append(QLatin1String("me"));
  m_commands.append(QLatin1String("topic"));

  connect(m_client, SIGNAL(notice(const Notice &)), SLOT(notice(const Notice &)));
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

  if (m_richText) {
    text = PlainTextFilter::filter(data.text);
  }
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
 * Универсальная функция для регистрации или авторизации пользователя.
 *
 * \param command  Команда, например "reg" или "login".
 * \param name     Имя пользователя, функция не проверяет корректность этого параметра.
 * \param password Пароль, может содержать любые символы.
 * \param json     JSON данные.
 *
 * \return Идентификатор сообщения или пустой массив, если произошла ошибка.
 */
QByteArray ClientHelper::login(const QString &command, const QString &name, const QString &password, const QVariantMap &json)
{
  Notice notice(m_client->userId(), SimpleID::password(password), command, timestamp(), randomId(), json);
  notice.setText(name);
  if (m_client->send(notice))
    return notice.id();

  return QByteArray();
}


QByteArray ClientHelper::randomId() const
{
  return SimpleID::randomId(SimpleID::MessageId, m_client->userId());
}


qint64 ClientHelper::timestamp()
{
# if QT_VERSION >= 0x040700
  return QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
# else
  return qint64(QDateTime::currentDateTime().toUTC().toTime_t()) * 1000;
# endif
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

  data.id = randomId();
  data.autoSetOptions();

  return m_client->send(data, true);
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


void ClientHelper::notice(const Notice &notice)
{
  if (!notice.isValid())
    return;

  SCHAT_DEBUG_STREAM(">> NOTICE <<" << notice.type() << notice.status() << notice.status(notice.status()) << notice.command() << notice.text() << notice.raw())

  m_notice = &notice;
  QString command = notice.command();
  if (command == "reg.reply")
    regReply(notice);
  else if (command == "login.reply")
    loginReply(notice);
  else
    this->notice();
}


bool ClientHelper::loginReply(const Notice &notice)
{
  if (notice.status() != Notice::OK)
    return false;

  if (notice.dest() != m_client->userId())
    return false;

  if (SimpleID::typeOf(notice.sender()) != SimpleID::PasswordId)
    return false;

  if (notice.text().isEmpty())
    return false;

  m_client->user()->setAccount(notice.text());
  emit loggedIn(notice.text());
  return true;
}


bool ClientHelper::regReply(const Notice &notice)
{
  if (notice.status() != Notice::OK)
    return false;

  if (notice.dest() != m_client->userId())
    return false;

  if (SimpleID::typeOf(notice.sender()) != SimpleID::PasswordId)
    return false;

  if (notice.text().isEmpty())
    return false;

  emit registered(notice.text(), notice.sender());
  return true;
}
