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

#include <QCoreApplication>

#include "cores/AnonymousAuth.h"
#include "cores/GenericCore.h"
#include "debugstream.h"
#include "events.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "Storage.h"

GenericCore::GenericCore(QObject *parent)
  : Core(parent)
{
  SCHAT_DEBUG_STREAM(this << "GENERIC CORE");

  addAuth(new AnonymousAuth(this));
}


bool GenericCore::checkPacket()
{
//  qDebug() << "GenericCore::checkPacket()";

  if (m_reader->sender().isEmpty())
    return false;

//  qDebug() << "#1";
  if (m_packetsEvent->userId() != m_reader->sender())
    return false;

//  qDebug() << "#2";
//  if (!(m_reader->headerOption() & Protocol::Broadcast)) {
//    int idType = SimpleID::typeOf(m_reader->dest());
//
//    if (idType == SimpleID::InvalidId)
//      return false;
//
//    if (m_reader->type() != Protocol::MessagePacket && idType == SimpleID::UserId && m_storage->user(m_reader->dest()) == 0)
//      return false;
//  }

  return true;
}


void GenericCore::readPacket(int type)
{
  qDebug() << "GenericCore::readPacket()" << type;

  switch (type) {
    case Protocol::MessagePacket:
      readMessage();
      break;

    case Protocol::UserDataPacket:
      readUserData();
      break;

    default:
      route();
      break;
  }
}
