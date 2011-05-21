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

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

static const int     StreamVersion                   = QDataStream::Qt_4_4;
static const quint16 ProtocolVersion                 = 3;

static const quint8 FlagNone                         = 0;
static const quint8 FlagDirect                       = 1;
static const quint8 FlagLink                         = 2;

/**
 * Приветственное сообщение. Сервер отвечает на этот пакет
 * опкодом `OpcodeAccessGranted` если доступ получен либо `OpcodeAccessDenied`.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint16 -> версия протокола `ProtocolVersion`
 * quint8  -> флаг `Flag*`
 * quint8  -> пол участника: 0 - мужской, 1 - женский (numeric сервера при `FlagLink`)
 * QString -> ник участника (рекомендуется пустая строка при `FlagLink`)
 * QString -> полное имя участника, может быть пустой строкой (ключ сети при `FlagLink`)
 * QString -> строка идентифицирующая агент пользователя, формат: UserAgent/Version
 * QString -> сообщение о выходе из чата.
 * ОТПРАВИТЕЛЬ: ---------------
 * Клиент
 */
static const quint16 OpcodeGreeting                  = 100;

/**
 * Ответ на успешное рукопожатие.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint16  -> numeric удалённого сервера, клиент игнорирует это значение
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeAccessGranted             = 101;

/**
 * Отправка сообщения
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * QString -> канал/ник для кого предназначено сообщение (пустая строка - главный канал), либо если отправитель сервер ник пользователя отправившего сообщение.
 * QString -> Сообщение
 * ОТПРАВИТЕЛЬ: ---------------
 * Клиент, Сервер
 */
static const quint16 OpcodeMessage                   = 200;

/**
 * Отправка приватного сообщения
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> флаг эха, если 1 то это подтверждение отправки сообщения.
 * QString -> ник, отправившего сообщение (flag = 0), ник того кому предназначается сообщение (flag = 1).
 * QString -> Сообщение
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodePrivateMessage            = 201;

/**
 * Отправка сервером сообщения для клиента
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * QString -> Сообщение
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeServerMessage             = 202;

/**
 * Универсальное ретранслируемое сообщение
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * QString -> канал/ник для кого предназначено сообщение (пустая строка - главный канал)
 * QString -> ник отправителся
 * QString -> сообщение
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер, Клиент
 */
static const quint16 OpcodeRelayMessage              = 205;

/**
 * Унифицированный пакет содержащий в себе полную информацию о пользователе
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> флаг пакетной передачи профилей (0 - пакетная передача, 1 - обычная)
 * quint8  -> numeric сервера к которому физически подключен клиент
 * quint8  -> пол участника: 0 - мужской, 1 - женский
 * QString -> ник участника
 * QString -> полное имя участника, может быть пустой строкой
 * QString -> сообщение о выходе
 * QString -> строка идентифицирующая агент участника, формат: UserAgent/Version
 * QString -> адрес участника
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер, Клиент
 */
static const quint16 OpcodeNewUser                   = 310;

/**
 * Выход пользователя из чата
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> флаг пакетной передачи профилей (0 - пакетная передача, 1 - обычная)
 * QString -> ник участника
 * QString -> сообщение о выходе
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер, Клиент
 */
static const quint16 OpcodeUserLeave                 = 302;

/**
 * Отказ в подключении
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint16 -> причина отказа
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeAccessDenied              = 9000;

/**
 * Отправка ping пакета для проверки работоспособности соединения.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodePing                      = 400;

/**
 * Ответ на пакет `OpcodePing`.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * ОТПРАВИТЕЛЬ: ---------------
 * Клиент
 */
static const quint16 OpcodePong                      = 401;

/**
 * Смена профиля.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> пол участника: 0 - мужской, 1 - женский
 * QString -> ник участника (старый ник участника если отправитель сервер)
 * QString -> полное имя участника, может быть пустой строкой
 * ОТПРАВИТЕЛЬ: ---------------
 * Клиент, Сервер
 */
static const quint16 OpcodeNewProfile                = 600;

/**
 * Уведомление о смене участником профиля
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> новый пол участника: 0 - мужской, 1 - женский
 * QString -> старый ник участника
 * QString -> новый ник участника
 * QString -> новое полное имя участника, может быть пустой строкой
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeNewNick                  = 602;

/**
 * Отправка клиентом сообщения о выходе.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * QString -> Текст сообщения о выходе
 * ОТПРАВИТЕЛЬ: ---------------
 * Клиент
 */
static const quint16 OpcodeByeMsg                    = 603;

/**
 * Уведомление о подключении к сети нового сервера
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> numeric сервера
 * QString -> Название сети
 * QString -> Адрес сервера
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeNewLink                   = 502;

/**
 * Уведомление о выходе их сети нового сервера
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> numeric сервера
 * QString -> Название сети
 * QString -> Адрес сервера
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeLinkLeave                 = 503;

/**
 * Список номеров серверов сети
 * ФОРМАТ: --------------------
 * quint16       -> размер пакета
 * quint16       -> опкод
 * QList<quint8> -> список
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeSyncNumerics              = 1100;

/**
 * Уведомление о завершении отправки пользователей
 * ФОРМАТ: --------------------
 * quint16       -> размер пакета
 * quint16       -> опкод
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeSyncUsersEnd              = 1110;

/**
 * Синхроницазия сообщения о выходе.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * QString -> Ник пользователя изменившего сообщение
 * QString -> Текст сообщения о выходе
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер, Клиент
 */
static const quint16 OpcodeSyncByeMsg                = 1120;

/*!
 * Универсальный пакет.
 * quint16        -> размер пакета
 * quint16        -> опкод
 * quint16        -> субопкод
 * QList<quint32> -> контейнер для данных типа quint32
 * QStringList    -> контейнер для данных типа QString
 */
static const quint16 OpcodeUniversal                 = 7666;


namespace schat
{

/*!
 * Передачи информации о статусе.
 * QList<quint32>[0] -> Статус (StatusNormal, StatusAway).
 * Отправитель: Клиент.
 */
static const quint16 UniStatus                      = 10;


/*!
 * Передача списка пользователей с определённым статусом.
 * QList<quint32>[0] -> Статус (StatusNormal, StatusAway).
 * QList<quint32>[1] -> 1+ - включить отображение в тексте статуса,
 *                      0  - тихий режим, по умолчанию 0.
 * QStringList       -> как минимум один ник пользователя.
 * Отправитель: Сервер.
 */
static const quint16 UniStatusList                  = 20;


/*!
 * Статусы для субопкода UniStatus.
 */
static const quint32 StatusNormal                   = 0;
static const quint32 StatusAway                     = 1;
static const quint32 StatusAutoAway                 = 2;
static const quint32 StatusDnD                      = 3;
static const quint32 StatusOffline                  = 32768;
}


/**
 * КОДЫ ОШИБОК
 */
static const quint16 ErrorNickAlreadyUse             = 200; // Выбранный ник уже занят.
static const quint16 ErrorOldClientProtocol          = 100; // Клиент использует устаревшую версию протокола.
static const quint16 ErrorOldServerProtocol          = 104; // Сервер использует устаревшую версию протокола.
static const quint16 ErrorBadGreetingFlag            = 101; // Клиент отправил неподдерживаемый флаг приветствия.
static const quint16 ErrorBadNickName                = 102; // Выбранный клиентом ник, не допустим в чате.
static const quint16 ErrorBadUserAgent               = 103; // Клиент отправил недопустимый UserAgent.
static const quint16 ErrorNotNetworkConfigured       = 400; // На сервере, к которому пытается слинковаться другой сервер, не настроена сеть.
static const quint16 ErrorBadNetworkKey              = 401; // Ошибка проверки ключа сети.
static const quint16 ErrorNumericAlreadyUse          = 403; // Ошибка линковки numeric подключаемого сервера уже используется.
static const quint16 ErrorBadNumeric                 = 404; // Ошибка линковки, неверный/некорректный numeric.
static const quint16 ErrorRootServerIsSlave          = 405; // Ошибка подключения к корневому серверу, т.к он сконфигурирован как вторичный.
static const quint16 ErrorUsersLimitExceeded         = 500; // Превышено максимально допустимое количество пользователей на этом сервере.
static const quint16 ErrorLinksLimitExceeded         = 501; // Превышено максимально допустимое количество серверов подключенных к этому серверу.
static const quint16 ErrorMaxUsersPerIpExceeded      = 502;

#endif /*PROTOCOL_H_*/
