/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

static const int     StreamVersion                   = QDataStream::Qt_4_3;
static const quint16 ProtocolVersion                 = 2;

static const quint16 sChatStateWaitingForGreeting    = 2000;
static const quint16 sChatStateReadingGreeting       = 2001;
static const quint16 sChatStateReadyForUse           = 2002;
static const quint16 sChatStateDisconnected          = 2003;
static const quint16 sChatStateWaitingForChecking    = 2004;
static const quint16 sChatStateSendingGreeting       = 2000;

static const quint8 FlagNone                         = 0;
static const quint8 FlagDirect                       = 1;

/**
 * Приветственное сообщение. Сервер отвечает на этот пакет
 * опкодом `OpcodeAccessGranted` если доступ получен либо `OpcodeAccessDenied`.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> версия протокола `sChatProtocolVersion`
 * quint8  -> флаг `sChatFlag*`
 * quint8  -> пол участника: 0 - мужской, 1 - женский
 * QString -> ник участника
 * QString -> полное имя участника, может быть пустой строкой
 * QString -> строка идентифицирующая агент участника, формат: UserAgent/Version
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
 * quint16 -> полученный уровень доступа, в настоящие время всегда `0`.
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
static const quint16 sChatOpcodeSendPrivateMessage   = 201;
static const quint16 sChatOpcodeSendPrvMessageEcho   = 202;

/**
 * Отправка клиенту профиля участника
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> флаг пакетной передачи профилей (0 - пакетная передача, 1 - обычная)
 * quint8  -> пол участника: 0 - мужской, 1 - женский
 * QString -> ник участника
 * QString -> полное имя участника, может быть пустой строкой
 * QString -> строка идентифицирующая агент участника, формат: UserAgent/Version
 * QString -> адрес участника
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeNewUser                   = 301;

/**
 * Выход пользователя из чата
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> флаг пакетной передачи профилей (0 - пакетная передача, 1 - обычная)
 * QString -> ник участника
 * QString -> сообщение о выходе
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeUserLeave                 = 302;

/**
 * Отказ в подключении
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint16 -> причина отказа.
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 OpcodeAccessDenied              = 9000;
static const quint16 sChatOpcodePing                 = 400;
static const quint16 sChatOpcodePong                 = 401;
static const quint16 sChatOpcodeMaxDoublePingTimeout = 402;
static const quint16 sChatOpcodeClientQuit           = 500;

/**
 * Смена профиля.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint16 -> пол участника: 0 - мужской, 1 - женский
 * QString -> ник участника
 * QString -> полное имя участника, может быть пустой строкой
 * ОТПРАВИТЕЛЬ: ---------------
 * Клиент
 */
static const quint16 sChatOpcodeNewProfile           = 600;

/**
 * Уведомление о смене участником профиля
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint16 -> новый пол участника: 0 - мужской, 1 - женский
 * QString -> старый ник участника
 * QString -> новое полное имя участника, может быть пустой строкой
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 sChatOpcodeChangedProfile       = 601;

/**
 * Уведомление о смене участником профиля
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint16 -> новый пол участника: 0 - мужской, 1 - женский
 * QString -> старый ник участника
 * QString -> новый ник участника
 * QString -> новое полное имя участника, может быть пустой строкой
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 sChatOpcodeChangedNick          = 602;


/**
 * Отправка клиентом сообщения о выходе.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * QString -> Текст сообщения о выходе
 * ОТПРАВИТЕЛЬ: ---------------
 * Клиент
 */
static const quint16 sChatOpcodeSendByeMsg           = 603;


/**
 * Запрос информации о сервере.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * ОТПРАВИТЕЛЬ: ---------------
 * Клиент
 */
static const quint16 sChatOpcodeGetServerInfo        = 700;


/**
 * Отправка информации о сервере.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * QString -> Информация
 * ОТПРАВИТЕЛЬ: ---------------
 * Сервер
 */
static const quint16 sChatOpcodeServerInfo           = 701;


/**
 * КОДЫ ОШИБОК
 */
static const quint16 ErrorNickAlreadyUse             = 200; // Выбранный ник уже занят.
static const quint16 ErrorOldClientProtocol          = 100; // Клиент использует устаревшую версию протокола.
static const quint16 ErrorOldServerProtocol          = 104; // Сервер использует устаревшую версию протокола.
static const quint16 ErrorBadGreetingFlag            = 101; // Клиент отправил неподдерживаемый флаг приветствия.
static const quint16 ErrorBadNickName                = 102; // Выбранный клиентом ник, не допустим в чате.
static const quint16 ErrorBadUserAgent               = 103; // Клиент отправил недопустимый UserAgent.
static const quint16 sChatErrorNoSuchChannel         = 300;
static const quint16 sChatErrorDirectNotAllow        = 400;

static const int PingInterval                        = 6 * 1000;

#endif /*PROTOCOL_H_*/
