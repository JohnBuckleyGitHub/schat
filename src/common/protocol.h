/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define SCHAT_DATA_STREAM_VERSION        QDataStream::Qt_4_3

#define SCHAT_STATE_WAITING_FOR_GREETING 2000
#define SCHAT_STATE_READING_GREETING     2001
#define SCHAT_STATE_READY_FOR_USE        2002
#define SCHAT_STATE_WAITING_FOR_CHECKING 2004

#define SCHAT_STATE_SENDING_GREETING     2000
#define SCHAT_STATE_DISCONNECTED         2003

#define SCHAT_GREETING                   100
#define SCHAT_GREETING_OK                101

#define SCHAT_SEND_MESSAGE               200

#define SCHAT_NEED_PARTICIPANT_LIST      300
#define SCHAT_NEW_PARTICIPANT_TO_LIST    301
#define SCHAT_NEW_PARTICIPANT_END        302

#define SCHAT_PROTOCOL_ERROR             9000
#define SCHAT_ERROR_NICK_ALREADY_USE     9001

static const int     sChatStreamVersion             = QDataStream::Qt_4_3;
static const quint8  sChatProtocolVersion           = 0;

static const quint16 sChatStateWaitingForGreeting   = 2000;

static const quint16 sChatOpcodeGreeting            = 100;
static const quint16 sChatOpcodeGreetingOk          = 101;
static const quint16 sChatOpcodeSendMessage         = 200;
static const quint16 sChatOpcodeNeedParticipantList = 300;
static const quint16 sChatOpcodeNewParticipant      = 301;
static const quint16 sChatOpcodeParticipantLeft     = 302;
static const quint16 sChatOpcodeNewParticipantQuiet = 303;
static const quint16 sChatOpcodeError               = 9000;

static const quint16 sChatErrorNickAlreadyUse       = 200;
static const quint16 sChatErrorBadProtocolVersion   = 100;
static const quint16 sChatErrorBadGreetingFlag      = 101;
static const quint16 sChatErrorBadNickName          = 102;
static const quint16 sChatErrorBadUserAgent         = 103;
static const quint16 sChatErrorInvalidConnection    = 110;


#endif /*PROTOCOL_H_*/
