// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KIRCNumerics.generated.h"

USTRUCT( BlueprintType )
struct FKIRCNumerics
{
    GENERATED_BODY()

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 NumericMin            = 000;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 NumericMax            = 511;


    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyNone             = 000;

    
	// Initial

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyWelcome          = 001;                  // :Welcome to the Internet Relay Network <nickname>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyYourHost         = 002;                  // :Your host is <server>; running version <ver>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyCreated          = 003;                  // :This server was created <datetime>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyMyInfo           = 004;                  // <server> <ver> <usermode> <chanmode>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyMap              = 005;                  // :map

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfMap         = 007;                  // :End of /MAP

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyMotdStart        = 375;                  // :- server Message of the Day

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyMotd             = 372;                  // :- <info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyMotdAlt          = 377;                  // :- <info>                                                                        (some)

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyMotdAlt2         = 378;                  // :- <info>                                                                        (some)

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyMotdEnd          = 376;                  // :End of /MOTD command.

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyUModeIs          = 221;                  // <mode>


    // IsOn/UserHost

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyUserHost         = 302;                  // :userhosts

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyIsOn             = 303;                  // :nicknames


    // Away

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyAway             = 301;                  // <nick> :away

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyUnAway           = 305;                  // :You are no longer marked as being away

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyNowAway          = 306;                  // :You have been marked as being away


    // WHOIS/WHOWAS

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyWhoisHelper      = 310;                  // <nick> :looks very helpful                                                       DALNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyWhoIsUser        = 311;                  // <nick> <username> <address> * :<info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyWhoIsServer      = 312;                  // <nick> <server> :<info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyWhoIsOperator    = 313;                  // <nick> :is an IRC Operator

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyWhoIsIdle        = 317;                  // <nick> <seconds> <signon> :<info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfWhois       = 318;                  // <request> :End of /WHOIS list.

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyWhoIsChannels    = 319;                  // <nick> :<channels>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyWhoWasUser       = 314;                  // <nick> <username> <address> * :<info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfWhoWas      = 369;                  // <request> :End of WHOWAS

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyWhoReply         = 352;                  // <channel> <username> <address> <server> <nick> <flags> :<hops> <info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfWho         = 315;                  // <request> :End of /WHO list.

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyUserIPs          = 307;                  // :userips                                                                         UNDERNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyUserIP           = 340;                  // <nick> :<nickname>=+<user>@<IP.address>                                          UNDERNET


    // List

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyListStart        = 321;                  // Channel :Users Name

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyList             = 322;                  // <channel> <users> :<topic>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyListEnd          = 323;                  // :End of /LIST

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyLinks            = 364;                  // <server> <hub> :<hops> <info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfLinks       = 365;                  // <mask> :End of /LINKS list.


    // Post-Channel Join

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyUniqOpIs         = 325;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyChannelModeIs    = 324;                  // <channel> <mode>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyChannelUrl       = 328;                  // <channel> :url                                                                   DALNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyChannelCreated   = 329;                  // <channel> <time>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyNoTopic          = 331;                  // <channel> :No topic is set.

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTopic            = 332;                  // <channel> :<topic>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTopicSetBy       = 333;                  // <channel> <nickname> <time>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyNamReply         = 353;                  // = <channel> :<names>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfNames       = 366;                  // <channel> :End of /NAMES list.


    // Invitational

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyInviting         = 341;                  // <nick> <channel>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplySummoning        = 342;


    // Channel Lists

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyInviteList       = 346;                  // <channel> <invite> <nick> <time>                                                 IRCNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfInviteList  = 357;                  // <channel> :End of Channel Invite List                                            IRCNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyExceptList       = 348;                  // <channel> <exception> <nick> <time>                                              IRCNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfExceptList  = 349;                  // <channel> :End of Channel Exception List                                         IRCNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyBanList          = 367;                  // <channel> <ban> <nick> <time>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfBanList     = 368;                  // <channel> :End of Channel Ban List


    // server/misc

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyVersion          = 351;                  // <version>.<debug> <server> :<info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyInfo             = 371;                  // :<info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfInfo        = 374;                  // :End of /INFO list.

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyYoureOper        = 381;                  // :You are now an IRC Operator

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyRehashing        = 382;                  // <file> :Rehashing

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyYoureService     = 383;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTime             = 391;                  // <server> :<time>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyUsersStart       = 392;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyUsers            = 393;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfUsers       = 394;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyNoUsers          = 395;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyServList         = 234;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyServListEnd      = 235;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyAdminMe          = 256;                  // :Administrative info about server

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyAdminLoc1        = 257;                  // :<info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyAdminLoc2        = 258;                  // :<info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyAdminEMail       = 259;                  // :<info>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTryAgain         = 263;                  // :Server load is temporarily too heavy. Please wait a while and try again.


    // tracing

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceLink        = 200;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceConnecting  = 201;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceHandshake   = 202;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceUnknown     = 203;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceOperator    = 204;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceUser        = 205;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceServer      = 206;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceService     = 207;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceNewType     = 208;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceClass       = 209;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceReconnect   = 210;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceLog         = 261;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyTraceEnd         = 262;


    // stats

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsLinkInfo    = 211;                  // <connection> <sendq> <sentmsg> <sentbyte> <recdmsg> <recdbyte> :<open>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsCommands    = 212;                  // <command> <uses> <bytes>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsCLine       = 213;                  // C <address> * <server> <port> <class>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsNLine       = 214;                  // N <address> * <server> <port> <class>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsILine       = 215;                  // I <ipmask> * <hostmask> <port> <class>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsKLine       = 216;                  // k <address> * <username> <details>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsPLine       = 217;                  // P <port> <??> <??>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsQLine       = 222;                  // <mask> :<comment>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsELine       = 223;                  // E <hostmask> * <username> <??> <??>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsDLine       = 224;                  // D <ipmask> * <username> <??> <??>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsLLine       = 241;                  // L <address> * <server> <??> <??>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsoLine       = 243;                  // o <mask> <password> <user> <??> <class>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsHLine       = 244;                  // H <address> * <server> <??> <??>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsGLine       = 247;                  // G <address> <timestamp> :<reason>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsULine       = 248;                  // U <host> * <??> <??> <??>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsZLine       = 249;                  // :info

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsYLine       = 218;                  // Y <class> <ping> <freq> <maxconnect> <sendq>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfStats       = 219;                  // <char> :End of /STATS report

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyStatsUptime      = 242;


    // GLINE

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyGLineList        = 280;                  // <address> <timestamp> <reason>                                                   UNDERNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfGLineList   = 281;                  // :End of G-line List                                                              UNDERNET


    // Silence

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplySilenceList      = 271;                  // <nick> <mask>                                                                    UNDERNET/DALNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyEndOfSilenceList = 272;                  // <nick> :End of Silence List                                                      UNDERNET/DALNET


    // LUser

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyLUserClient      = 251;                  // :There are <user> users and <invis> invisible on <serv> servers

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyLUserOp          = 252;                  // <num> :operator(s) online

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyLUserUnknown     = 253;                  // <num> :unknown connection(s)

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyLUserChannels    = 254;                  // <num> :channels formed

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyLUserMe          = 255;                  // :I have <user> clients and <serv> servers

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyLUserLocalUser   = 265;                  // :Current local users: <curr> Max: <max>

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ReplyLUserGlobalUser  = 266;                  // :Current global users: <curr> Max: <max>


    // Errors

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoSuchNick       = 401;                  // <nickname> :No such nick

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoSuchServer     = 402;                  // <server> :No such server

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoSuchChannel    = 403;                  // <channel> :No such channel

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorCannotSendToChan = 404;                  // <channel> :Cannot send to channel

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorTooManyChannels  = 405;                  // <channel> :You have joined too many channels

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorWasNoSuchNick    = 406;                  // <nickname> :There was no such nickname

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorTooManyTargets   = 407;                  // <target> :Duplicate recipients. No message delivered

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoColors         = 408;                  // <nickname> #<channel> :You cannot use colors on this channel. Not sent: <text>   DALNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoOrigin         = 409;                  // :No origin specified

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoRecipient      = 411;                  // :No recipient given (<command>)

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoTextToSend     = 412;                  // :No text to send

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoTopLevel       = 413;                  // <mask> :No toplevel domain specified

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorWildTopLevel     = 414;                  // <mask> :Wildcard in toplevel Domain

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorBadMask          = 415;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorTooMuchInfo      = 416;                  // <command> :Too many lines in the output; restrict your query                     UNDERNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorUnknownCommand   = 421;                  // <command> :Unknown command

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoMotd           = 422;                  // :MOTD File is missing

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoAdminInfo      = 423;                  // <server> :No administrative info available

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorFileError        = 424;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoNicknameGiven  = 431;                  // :No nickname given

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorErroneusNickname = 432;                  // <nickname> :Erroroneus Nickname

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNickNameInUse    = 433;                  // <nickname> :Nickname is already in use.

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNickCollision    = 436;                  // <nickname> :Nickname collision KILL

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorUnAvailResource  = 437;                  // <channel> :Cannot change nickname while banned on channel

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNickTooFast      = 438;                  // <nick> :Nick change too fast. Please wait <sec> seconds.                         (most)

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorTargetTooFast    = 439;                  // <target> :Target change too fast. Please wait <sec> seconds.                     DALNET/UNDERNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorUserNotInChannel = 441;                  // <nickname> <channel> :They aren't on that channel

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNotOnChannel     = 442;                  // <channel> :You're not on that channel

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorUserOnChannel    = 443;                  // <nickname> <channel> :is already on channel

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoLogin          = 444;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorSummonDisabled   = 445;                  // :SUMMON has been disabled

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorUsersDisabled    = 446;                  // :USERS has been disabled

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNotRegistered    = 451;                  // <command> :Register first.

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNeedMoreParams   = 461;                  // <command> :Not enough parameters

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorAlreadyRegistered= 462;                  // :You may not reregister

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoPermForHost    = 463;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorPasswdMistmatch  = 464;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorYoureBannedCreep = 465;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorYouWillBeBanned  = 466;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorKeySet           = 467;                  // <channel> :Channel key already set

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorServerCanChange  = 468;                  // <channel> :Only servers can change that mode                                     DALNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorChannelIsFull    = 471;                  // <channel> :Cannot join channel (+l)

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorUnknownMode      = 472;                  // <char> :is unknown mode char to me

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorInviteOnlyChan   = 473;                  // <channel> :Cannot join channel (+i)

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorBannedFromChan   = 474;                  // <channel> :Cannot join channel (+b)

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorBadChannelKey    = 475;                  // <channel> :Cannot join channel (+k)

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorBadChanMask      = 476;

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNickNotRegistered= 477;                  // <channel> :You need a registered nick to join that channel.                      DALNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorBanListFull      = 478;                  // <channel> <ban> :Channel ban/ignore list is full

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoPrivileges     = 481;                  // :Permission Denied- You're not an IRC operator

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorChanOPrivsNeeded = 482;                  // <channel> :You're not channel operator

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorCantKillServer   = 483;                  // :You cant kill a server!

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorRestricted       = 484;                  // <nick> <channel> :Cannot kill; kick or deop channel service                      UNDERNET

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorUniqOPrivsNeeded = 485;                  // <channel> :Cannot join channel (reason)

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorNoOperHost       = 491;                  // :No O-lines for your host

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorUModeUnknownFlag = 501;                  // :Unknown MODE flag

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorUsersDontMatch   = 502;                  // :Cant change mode for other users

    UPROPERTY( Category = "KeshIRC|Controller", VisibleAnywhere, BlueprintReadOnly )
    int32 ErrorSilenceListFull  = 511;                  // <mask> :Your silence list is full                                                UNDERNET/DALNET

};
