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
    m_joinFloodBanTime(60),
    m_joinFloodDetectTime(60),
    m_joinFloodLimit(3),
    m_maxRepeatedMsgs(3),
    m_muteTime(60)
  {
  }

  FloodLimits(int floodDetectTime, int floodLimit, int joinFloodBanTime, int joinFloodDetectTime, int joinFloodLimit, int maxRepeatedMsgs, int muteTime)
  : m_floodDetectTime(floodDetectTime),
    m_floodLimit(floodLimit),
    m_joinFloodBanTime(joinFloodBanTime),
    m_joinFloodDetectTime(joinFloodDetectTime),
    m_joinFloodLimit(joinFloodLimit),
    m_maxRepeatedMsgs(maxRepeatedMsgs),
    m_muteTime(muteTime)
  {
  }

  inline int floodDetectTime() const     { return m_floodDetectTime;  }
  inline int floodLimit() const          { return m_floodLimit;  }
  inline int joinFloodBanTime() const    { return m_joinFloodBanTime; }
  inline int joinFloodDetectTime() const { return m_joinFloodDetectTime; }
  inline int joinFloodLimit() const      { return m_joinFloodLimit; }
  inline int maxRepeatedMsgs() const     { return m_maxRepeatedMsgs;  }
  inline int muteTime() const            { return m_muteTime;  }

private:
  int m_floodDetectTime;     ///< Контрольное время в течении которого обнаруживается флуд.
  int m_floodLimit;          ///< Максимальное число сообщений которые могут быть отправлены за время m_floodDetectTime.
  int m_joinFloodBanTime;    ///< Время в течении которого пользователь не может подключатся.
  int m_joinFloodDetectTime; ///< Контрольное время в течении которого обнаруживаются слишком частые подключения.
  int m_joinFloodLimit;      ///< Максимальное число подключений за время m_joinFloodDetectTime.
  int m_maxRepeatedMsgs;     ///< Максимальное число повторяющихся подряд сообщений.
  int m_muteTime;            ///< Время в течении которого пользователь не может говорить.
};


/**
 * Хранит информацию о флуд лимитах, пока пользователь отключен.
 */
class FloodOfflineItem {
public:
  FloodOfflineItem()
  : m_reconnects(0),
    m_muteTime(0),
    m_timeStamp(0)
  {
  }

  FloodOfflineItem(int reconnects, const QString &host, uint muteTime, uint timeStamp)
  : m_reconnects(reconnects),
    m_host(host),
    m_muteTime(muteTime),
    m_timeStamp(timeStamp)
  {
  }

  inline int reconnects() const { return m_reconnects; }
  inline QString host() const   { return m_host; }
  inline uint muteTime() const  { return m_muteTime; }
  inline uint timeStamp() const { return m_timeStamp; }

private:
  int m_reconnects;
  QString m_host;
  uint m_muteTime;
  uint m_timeStamp;
};


class FloodChannel {
public:
  FloodChannel()
  : messages(0),
    repeated(0),
    muteTime(0)
  {
  }

  int messages;            ///< Счётчик сообщений для обнаружения флуда.
  int repeated;              ///< Число зафиксированных повторяющихся сообщений.
  QString previous;          ///< Предыдущее сообщение.
  uint floodDetectStartTime; ///< Контрольное время старта проверки на флуд.
  uint lastMsgTime;          ///< Время последнего сообщения.
  uint muteTime;             ///< Время когда начало действовать ограничение.
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
  inline uint timeStamp() const           { return m_timeStamp; }
  inline void setMuteTime(uint muteTime)  { m_muteTime = muteTime; }
  inline void setNumeric(quint8 numeric)  { m_numeric = numeric; }
  inline void setReconnects(int count)    { m_reconnects = count; }
  int isFlood(const QString &channel, const QString &message);
  int reconnects() const;
  void setFloodLimits(const FloodLimits &limits);

private:
  FloodChannel floodChannel(const QString &channel) const;
  void setFloodChannel(const QString &channel, const FloodChannel &fc);

  AbstractProfile *m_profile;        ///< Профиль пользователя.
  FloodLimits m_floodLimits;         ///< Параметры защиты от флуда.
  int m_reconnects;                  ///< Количество зафиксированных попыток переподключения.
  QHash<QString, FloodChannel> m_channels;
  QPointer<DaemonService> m_service; ///< Сервис обслуживающий пользователя.
  quint8 m_numeric;                  ///< Номер сервера.
  uint m_muteTime;                   ///< Время когда начало действовать ограничение.
  uint m_timeStamp;                  ///< Время когда пользователь подключился.
};

#endif /*USERUNIT_H_*/
