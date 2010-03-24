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

#ifndef USERUNIT_H_
#define USERUNIT_H_

#include <QPointer>

#include "daemonservice.h"

class AbstractProfile;


/*!
 * Параметры защиты от флуда.
 */
class FloodLimits {
public:
  FloodLimits()
  : floodDetectTime(16),
    floodLimit(8),
    maxRepeatedMsgs(3),
    muteTime(60)
  {
  }

  FloodLimits(int floodDetectTime, int floodLimit, int maxRepeatedMsgs, int muteTime)
  : floodDetectTime(floodDetectTime),
    floodLimit(floodLimit),
    maxRepeatedMsgs(maxRepeatedMsgs),
    muteTime(muteTime)
  {
  }

  const int floodDetectTime; ///< Контрольное время в течении которого обнаруживается флуд.
  const int floodLimit;      ///< Максимальное число сообщений которые могут быть отправлены за время floodDetectTime.
  const int maxRepeatedMsgs; ///< Максимальное число повторяющихся подряд сообщений.
  const int muteTime;        ///< Время в течении которого пользователь не может говорить.
};


/*!
 * \brief Базовый класс хранящий информацию о пользователе.
 */
class UserUnit {
  
public:
  UserUnit();
  UserUnit(const QStringList &list, DaemonService *service = 0, quint8 numeric = 0);
  ~UserUnit();

  inline AbstractProfile* profile()      { return m_profile; }
  inline DaemonService* service()        { return m_service; }
  inline quint8 numeric()                { return m_numeric; }
  inline void setNumeric(quint8 numeric) { m_numeric = numeric; }
  int isFlood(const QString &message);

private:
  AbstractProfile *m_profile;        ///< Профиль пользователя.
  FloodLimits m_floodLimits;         ///< Параметры защиты от флуда.
  int m_messages;                    ///< Счётчик сообщений для обнаружения флуда.
  int m_repeatedMsgs;                ///< Число зафиксированных повторяющихся сообщений.
  QPointer<DaemonService> m_service; ///< Сервис обслуживающий пользователя.
  QString m_previousMessage;         ///< Предыдущее сообщение.
  quint8 m_numeric;                  ///< Номер сервера.
  uint m_floodDetectStartTime;       ///< Контрольное время старта проверки на флуд.
  uint m_lastMsgTime;                ///< Время последнего сообщения.
  uint m_muteTime;                   ///< Время когда начало действовать ограничение.
};

#endif /*USERUNIT_H_*/
