/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

static const int     sChatStreamVersion             = QDataStream::Qt_4_3;
static const quint8  sChatProtocolVersion           = 0;

static const quint16 sChatStateWaitingForGreeting   = 2000;
static const quint16 sChatStateReadingGreeting      = 2001;
static const quint16 sChatStateReadyForUse          = 2002;
static const quint16 sChatStateDisconnected         = 2003;
static const quint16 sChatStateWaitingForChecking   = 2004;
static const quint16 sChatStateSendingGreeting      = 2000;

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
