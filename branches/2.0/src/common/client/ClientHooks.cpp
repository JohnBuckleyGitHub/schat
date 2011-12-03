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

namespace Hooks
{

Messages::Messages(QObject *parent)
  : QObject(parent)
{
}


/*!
 * Обработка команды в тексте.
 * Текст предварительно очищен от HTML.
 *
 * \param dest Идентификатор получателя.
 * \param cmd  Команда и тело команды.
 *
 * \return \b true если команда обработана.
 */
bool Messages::command(const QByteArray &dest, const ClientCmd &cmd)
{
  qDebug() << cmd.command() << cmd.body();
  QString command = cmd.command().toLower();

  if (command == "join") {
    ChatClient::channels()->join(cmd.body());
    return true;
  }

  return false;
}

}  // namespace Hooks
