/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#include <QtCore>

#include "abstractprofile.h"
#include "userunit.h"

/*!
 * \brief Конструктор класса UserUnit.
 */
UserUnit::UserUnit()
  : m_profile(0),
    m_messages(0),
    m_reconnects(0),
    m_repeatedMsgs(0),
    m_numeric(0),
    m_muteTime(0),
    m_timeStamp(QDateTime::currentDateTime().toTime_t())
{
}


/*!
 * \brief Конструктор класса UserUnit, полностью инициализирующий данные.
 * \param list Стандартный список, содержащий в себе полные данные пользователя.
 * \param service Указатель на сервис клиента.
 * \param numeric номер сервера к которому подключен клиент.
 */
UserUnit::UserUnit(const QStringList &list, const FloodLimits &floodLimits, DaemonService *service, quint8 numeric)
  : m_profile(new AbstractProfile(list)),
    m_floodLimits(floodLimits),
    m_messages(0),
    m_reconnects(0),
    m_repeatedMsgs(0),
    m_service(service),
    m_numeric(numeric),
    m_muteTime(0),
    m_timeStamp(QDateTime::currentDateTime().toTime_t())
{
}


UserUnit::~UserUnit()
{
#ifdef SCHAT_DEBUG
  qDebug() << "UserUnit::~UserUnit()";
#endif

  if (m_profile)
    delete m_profile;
}


/**
 * Проверка сообщения на удовлетворение критериям флуда.
 *
 * \return Время в секундах на которое пользователь лишён права голоса, по причине флуда, 0 - если флуд не обнаружен.
 */
int UserUnit::isFlood(const QString &message)
{
  if (m_previousMessage == message)
    m_repeatedMsgs++;
  else
    m_repeatedMsgs = 0;

  m_previousMessage = message;
  m_lastMsgTime = QDateTime::currentDateTime().toTime_t();

  if (m_messages == 0) {
    m_floodDetectStartTime = m_lastMsgTime;
    m_messages++;
  }
  else if (m_lastMsgTime - m_floodDetectStartTime <= (uint) m_floodLimits.floodDetectTime()) {
    m_messages++;
  }
  else {
    m_messages = 0;
  }

  if (m_repeatedMsgs >= m_floodLimits.maxRepeatedMsgs() || m_messages >= m_floodLimits.floodLimit()) {
    m_muteTime = m_lastMsgTime;
  }

  int offset = m_lastMsgTime - m_muteTime;
  if (m_muteTime != 0 && offset <= m_floodLimits.muteTime())
    return m_floodLimits.muteTime() - offset;

  m_muteTime = 0;
  return 0;
}


int UserUnit::reconnects() const
{
  if (QDateTime::currentDateTime().toTime_t() - m_timeStamp < (uint) m_floodLimits.joinFloodDetectTime())
    return m_reconnects;

  return 0;
}


/*!
 * Установка новых параметров защиты от флуда и сброс текущих ограничений.
 */
void UserUnit::setFloodLimits(const FloodLimits &limits)
{
  m_messages = 0;
  m_muteTime = 0;
  m_floodLimits = limits;
}
