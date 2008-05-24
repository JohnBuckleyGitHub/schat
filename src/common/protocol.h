/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

static const int     sChatStreamVersion              = QDataStream::Qt_4_4;
static const quint8  sChatProtocolVersion            = 0;

static const quint16 sChatStateWaitingForGreeting    = 2000;
static const quint16 sChatStateReadingGreeting       = 2001;
static const quint16 sChatStateReadyForUse           = 2002;
static const quint16 sChatStateDisconnected          = 2003;
static const quint16 sChatStateWaitingForChecking    = 2004;
static const quint16 sChatStateSendingGreeting       = 2000;

static const quint8 sChatFlagNone                    = 0;
static const quint8 sChatFlagDirect                  = 1;

/**
 * Приветственное сообщение.
 * ФОРМАТ: --------------------
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  -> версия протокола `sChatProtocolVersion`
 * quint8  -> флаг `sChatFlag*`
 * quint8  -> пол участника: 0 - мужской, 1 - женский
 * QString -> ник участника
 * QString -> полное имя участника, может быть пустой строкой
 * QString -> строка идентифицирующая агент участника, формат: UserAgent/Version
 * ОТПРАВИТЕЛЬ: ---------------
 * Клиент
 */
static const quint16 sChatOpcodeGreeting             = 100;
static const quint16 sChatOpcodeGreetingOk           = 101;
static const quint16 sChatOpcodeSendMessage          = 200;
static const quint16 sChatOpcodeSendPrivateMessage   = 201;
static const quint16 sChatOpcodeSendPrvMessageEcho   = 202;
static const quint16 sChatOpcodeNewParticipant       = 301;
static const quint16 sChatOpcodeParticipantLeft      = 302;
static const quint16 sChatOpcodeNewParticipantQuiet  = 303;
static const quint16 sChatOpcodeError                = 9000;
static const quint16 sChatOpcodePing                 = 400;
static const quint16 sChatOpcodePong                 = 401;
static const quint16 sChatOpcodeMaxDoublePingTimeout = 402;
static const quint16 sChatOpcodeClientQuit           = 500;

static const quint16 sChatErrorNickAlreadyUse        = 200;
static const quint16 sChatErrorBadProtocolVersion    = 100;
static const quint16 sChatErrorBadGreetingFlag       = 101;
static const quint16 sChatErrorBadNickName           = 102;
static const quint16 sChatErrorBadUserAgent          = 103;
static const quint16 sChatErrorInvalidConnection     = 110;
static const quint16 sChatErrorNoSuchChannel         = 300;
static const quint16 sChatErrorDirectNotAllow        = 400;

static const int PingInterval                        = 6 * 1000;

#endif /*PROTOCOL_H_*/
