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
  : m_floodDetectTime(16),
    m_floodLimit(8),
    m_maxRepeatedMsgs(3),
    m_muteTime(60)
  {
  }

  FloodLimits(int floodDetectTime, int floodLimit, int maxRepeatedMsgs, int muteTime)
  : m_floodDetectTime(floodDetectTime),
    m_floodLimit(floodLimit),
    m_maxRepeatedMsgs(maxRepeatedMsgs),
    m_muteTime(muteTime)
  {
  }

  inline int floodDetectTime() const { return m_floodDetectTime;  }
  inline int floodLimit() const      { return m_floodLimit;  }
  inline int maxRepeatedMsgs() const { return m_maxRepeatedMsgs;  }
  inline int muteTime() const        { return m_muteTime;  }

private:
  int m_floodDetectTime; ///< Контрольное время в течении которого обнаруживается флуд.
  int m_floodLimit;      ///< Максимальное число сообщений которые могут быть отправлены за время floodDetectTime.
  int m_maxRepeatedMsgs; ///< Максимальное число повторяющихся подряд сообщений.
  int m_muteTime;        ///< Время в течении которого пользователь не может говорить.
};


/**
 * Хранит информацию о флуд лимитах, пока пользователь отключен.
 */
class FloodOfflineItem {
public:
  FloodOfflineItem()
  : m_muteTime(0)
  {
  }

  FloodOfflineItem(const QString &host, uint muteTime)
  : m_host(host),
    m_muteTime(muteTime)
  {
  }

  inline QString host() const  { return m_host; }
  inline uint muteTime() const { return m_muteTime; }

private:
  QString m_host;
  uint m_muteTime;
};


/*!
 * \brief Базовый класс хранящий информацию о пользователе.
 */
class UserUnit {
  
public:
  UserUnit();
  UserUnit(const QStringList &list, const FloodLimits &floodLimits, DaemonService *service = 0, quint8 numeric = 0);
  ~UserUnit();

  inline AbstractProfile* profile() const { return m_profile; }
  inline DaemonService* service() const   { return m_service; }
  inline quint8 numeric() const           { return m_numeric; }
  inline uint muteTime() const            { return m_muteTime; }
  inline void setMuteTime(uint muteTime)  { m_muteTime = muteTime; }
  inline void setNumeric(quint8 numeric)  { m_numeric = numeric; }
  int isFlood(const QString &message);
  void setFloodLimits(const FloodLimits &limits);

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
