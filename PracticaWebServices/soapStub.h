/* soapStub.h
   Generated by gSOAP 2.8.75 for conecta4.h

gSOAP XML Web services tools
Copyright (C) 2000-2018, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#define OK_NAME_REGISTERED 10000
#define ERROR_NAME_REPEATED 10001
#define ERROR_SERVER_FULL 10002
#define ERROR_PLAYER_NOT_FOUND 10003
#define TURN_MOVE 70001
#define TURN_WAIT 70002
#define GAMEOVER_WIN 50052
#define GAMEOVER_DRAW 50053
#define GAMEOVER_LOSE 50054
#define BOARD_WIDTH 7
#define BOARD_HEIGHT 6
#define PLAYER_1_CHIP 'o'
#define PLAYER_2_CHIP 'x'
#define EMPTY_CELL ' '
#define TRUE 1
#define FALSE 0
#define STRING_LENGTH 128

#ifndef soapStub_H
#define soapStub_H
#include "stdsoap2.h"
#if GSOAP_VERSION != 20875
# error "GSOAP VERSION 20875 MISMATCH IN GENERATED CODE VERSUS LIBRARY CODE: PLEASE REINSTALL PACKAGE"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************\
 *                                                                            *
 * Enumeration Types                                                          *
 *                                                                            *
\******************************************************************************/


/* conecta4.h:59 */
#ifndef SOAP_TYPE_players
#define SOAP_TYPE_players (7)
/* players */
enum players {
	player1 = 0,
	player2 = 1
};
#endif

/* conecta4.h:62 */
#ifndef SOAP_TYPE_moves
#define SOAP_TYPE_moves (9)
/* moves */
enum moves {
	OK_move = 0,
	fullColumn_move = 1
};
#endif

/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Structs and Unions                                                         *
 *                                                                            *
\******************************************************************************/

struct tMessage;	/* conecta4.h:68 */
struct tBlock;	/* conecta4.h:74 */
struct conecta4ns__registerResponse;	/* conecta4.h:81 */
struct conecta4ns__register;	/* conecta4.h:81 */
struct conecta4ns__getStatusResponse;	/* conecta4.h:82 */
struct conecta4ns__getStatus;	/* conecta4.h:82 */
struct conecta4ns__insertChipResponse;	/* conecta4.h:83 */
struct conecta4ns__insertChip;	/* conecta4.h:83 */

/* conecta4.h:68 */
#ifndef SOAP_TYPE_tMessage
#define SOAP_TYPE_tMessage (12)
/* complex XML schema type 'tMessage': */
struct tMessage {
        /** Sequence of elements 'msg' of XML schema type 'xsd:string' stored in dynamic array msg of length __size */
        int __size;
        char *msg;
};
#endif

/* conecta4.h:74 */
#ifndef SOAP_TYPE_tBlock
#define SOAP_TYPE_tBlock (14)
/* complex XML schema type 'tBlock': */
struct tBlock {
        /** Required element 'code' of XML schema type 'xsd:unsignedInt' */
        unsigned int code;
        /** Required element 'msgStruct' of XML schema type 'conecta4ns:tMessage' */
        struct tMessage msgStruct;
        /** Sequence of elements 'board' of XML schema type 'xsd:string' stored in dynamic array board of length __size */
        int __size;
        char *board;
};
#endif

/* conecta4.h:81 */
#ifndef SOAP_TYPE_conecta4ns__registerResponse
#define SOAP_TYPE_conecta4ns__registerResponse (19)
/* complex XML schema type 'conecta4ns:registerResponse': */
struct conecta4ns__registerResponse {
        /** Optional element 'code' of XML schema type 'xsd:int' */
        int *code;
};
#endif

/* conecta4.h:81 */
#ifndef SOAP_TYPE_conecta4ns__register
#define SOAP_TYPE_conecta4ns__register (20)
/* complex XML schema type 'conecta4ns:register': */
struct conecta4ns__register {
        /** Required element 'playerName' of XML schema type 'conecta4ns:tMessage' */
        struct tMessage playerName;
};
#endif

/* conecta4.h:82 */
#ifndef SOAP_TYPE_conecta4ns__getStatusResponse
#define SOAP_TYPE_conecta4ns__getStatusResponse (23)
/* complex XML schema type 'conecta4ns:getStatusResponse': */
struct conecta4ns__getStatusResponse {
        /** Optional element 'status' of XML schema type 'conecta4ns:tBlock' */
        struct tBlock *status;
};
#endif

/* conecta4.h:82 */
#ifndef SOAP_TYPE_conecta4ns__getStatus
#define SOAP_TYPE_conecta4ns__getStatus (24)
/* complex XML schema type 'conecta4ns:getStatus': */
struct conecta4ns__getStatus {
        /** Required element 'playerName' of XML schema type 'conecta4ns:tMessage' */
        struct tMessage playerName;
};
#endif

/* conecta4.h:83 */
#ifndef SOAP_TYPE_conecta4ns__insertChipResponse
#define SOAP_TYPE_conecta4ns__insertChipResponse (26)
/* complex XML schema type 'conecta4ns:insertChipResponse': */
struct conecta4ns__insertChipResponse {
        /** Optional element 'status' of XML schema type 'conecta4ns:tBlock' */
        struct tBlock *status;
};
#endif

/* conecta4.h:83 */
#ifndef SOAP_TYPE_conecta4ns__insertChip
#define SOAP_TYPE_conecta4ns__insertChip (27)
/* complex XML schema type 'conecta4ns:insertChip': */
struct conecta4ns__insertChip {
        /** Required element 'playerName' of XML schema type 'conecta4ns:tMessage' */
        struct tMessage playerName;
        /** Required element 'playerMove' of XML schema type 'xsd:int' */
        int playerMove;
};
#endif

/* conecta4.h:86 */
#ifndef WITH_NOGLOBAL
#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (28)
/* SOAP_ENV__Header: */
struct SOAP_ENV__Header {
#ifdef WITH_NOEMPTYSTRUCT
	char dummy;	/* empty struct is a GNU extension */
#endif
};
#endif
#endif

/* conecta4.h:86 */
#ifndef WITH_NOGLOBAL
#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (29)
/* Type SOAP_ENV__Code is a recursive data type, (in)directly referencing itself through its (base or derived class) members */
/* SOAP_ENV__Code: */
struct SOAP_ENV__Code {
        /** Optional element 'SOAP-ENV:Value' of XML schema type 'xsd:QName' */
        char *SOAP_ENV__Value;
        /** Optional element 'SOAP-ENV:Subcode' of XML schema type 'SOAP-ENV:Code' */
        struct SOAP_ENV__Code *SOAP_ENV__Subcode;
};
#endif
#endif

/* conecta4.h:86 */
#ifndef WITH_NOGLOBAL
#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (31)
/* SOAP_ENV__Detail: */
struct SOAP_ENV__Detail {
        char *__any;
        /** Any type of element 'fault' assigned to fault with its SOAP_TYPE_<typename> assigned to __type */
        /** Do not create a cyclic data structure throught this member unless SOAP encoding or SOAP_XML_GRAPH are used for id-ref serialization */
        int __type;
        void *fault;
};
#endif
#endif

/* conecta4.h:86 */
#ifndef WITH_NOGLOBAL
#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (34)
/* SOAP_ENV__Reason: */
struct SOAP_ENV__Reason {
        /** Optional element 'SOAP-ENV:Text' of XML schema type 'xsd:string' */
        char *SOAP_ENV__Text;
};
#endif
#endif

/* conecta4.h:86 */
#ifndef WITH_NOGLOBAL
#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (35)
/* SOAP_ENV__Fault: */
struct SOAP_ENV__Fault {
        /** Optional element 'faultcode' of XML schema type 'xsd:QName' */
        char *faultcode;
        /** Optional element 'faultstring' of XML schema type 'xsd:string' */
        char *faultstring;
        /** Optional element 'faultactor' of XML schema type 'xsd:string' */
        char *faultactor;
        /** Optional element 'detail' of XML schema type 'SOAP-ENV:Detail' */
        struct SOAP_ENV__Detail *detail;
        /** Optional element 'SOAP-ENV:Code' of XML schema type 'SOAP-ENV:Code' */
        struct SOAP_ENV__Code *SOAP_ENV__Code;
        /** Optional element 'SOAP-ENV:Reason' of XML schema type 'SOAP-ENV:Reason' */
        struct SOAP_ENV__Reason *SOAP_ENV__Reason;
        /** Optional element 'SOAP-ENV:Node' of XML schema type 'xsd:string' */
        char *SOAP_ENV__Node;
        /** Optional element 'SOAP-ENV:Role' of XML schema type 'xsd:string' */
        char *SOAP_ENV__Role;
        /** Optional element 'SOAP-ENV:Detail' of XML schema type 'SOAP-ENV:Detail' */
        struct SOAP_ENV__Detail *SOAP_ENV__Detail;
};
#endif
#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/


/* (built-in):0 */
#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (5)
typedef char *_XML;
#endif

/* (built-in):0 */
#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (6)
typedef char *_QName;
#endif

/* conecta4.h:59 */
#ifndef SOAP_TYPE_conecta4ns__tPlayer
#define SOAP_TYPE_conecta4ns__tPlayer (8)
typedef enum players conecta4ns__tPlayer;
#endif

/* conecta4.h:62 */
#ifndef SOAP_TYPE_conecta4ns__tMove
#define SOAP_TYPE_conecta4ns__tMove (10)
typedef enum moves conecta4ns__tMove;
#endif

/* conecta4.h:65 */
#ifndef SOAP_TYPE_xsd__string
#define SOAP_TYPE_xsd__string (11)
typedef char *xsd__string;
#endif

/* conecta4.h:71 */
#ifndef SOAP_TYPE_conecta4ns__tMessage
#define SOAP_TYPE_conecta4ns__tMessage (13)
typedef struct tMessage conecta4ns__tMessage;
#endif

/* conecta4.h:79 */
#ifndef SOAP_TYPE_conecta4ns__tBlock
#define SOAP_TYPE_conecta4ns__tBlock (16)
typedef struct tBlock conecta4ns__tBlock;
#endif

/******************************************************************************\
 *                                                                            *
 * Serializable Types                                                         *
 *                                                                            *
\******************************************************************************/


/* char has binding name 'byte' for type 'xsd:byte' */
#ifndef SOAP_TYPE_byte
#define SOAP_TYPE_byte (3)
#endif

/* int has binding name 'int' for type 'xsd:int' */
#ifndef SOAP_TYPE_int
#define SOAP_TYPE_int (1)
#endif

/* unsigned int has binding name 'unsignedInt' for type 'xsd:unsignedInt' */
#ifndef SOAP_TYPE_unsignedInt
#define SOAP_TYPE_unsignedInt (15)
#endif

/* conecta4ns__tMove has binding name 'conecta4ns__tMove' for type 'conecta4ns:tMove' */
#ifndef SOAP_TYPE_conecta4ns__tMove
#define SOAP_TYPE_conecta4ns__tMove (10)
#endif

/* enum moves has binding name 'moves' for type 'moves' */
#ifndef SOAP_TYPE_moves
#define SOAP_TYPE_moves (9)
#endif

/* conecta4ns__tPlayer has binding name 'conecta4ns__tPlayer' for type 'conecta4ns:tPlayer' */
#ifndef SOAP_TYPE_conecta4ns__tPlayer
#define SOAP_TYPE_conecta4ns__tPlayer (8)
#endif

/* enum players has binding name 'players' for type 'players' */
#ifndef SOAP_TYPE_players
#define SOAP_TYPE_players (7)
#endif

/* struct SOAP_ENV__Fault has binding name 'SOAP_ENV__Fault' for type '' */
#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (35)
#endif

/* struct SOAP_ENV__Reason has binding name 'SOAP_ENV__Reason' for type '' */
#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (34)
#endif

/* struct SOAP_ENV__Detail has binding name 'SOAP_ENV__Detail' for type '' */
#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (31)
#endif

/* struct SOAP_ENV__Code has binding name 'SOAP_ENV__Code' for type '' */
#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (29)
#endif

/* struct SOAP_ENV__Header has binding name 'SOAP_ENV__Header' for type '' */
#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (28)
#endif

/* struct conecta4ns__insertChip has binding name 'conecta4ns__insertChip' for type 'conecta4ns:insertChip' */
#ifndef SOAP_TYPE_conecta4ns__insertChip
#define SOAP_TYPE_conecta4ns__insertChip (27)
#endif

/* struct conecta4ns__insertChipResponse has binding name 'conecta4ns__insertChipResponse' for type 'conecta4ns:insertChipResponse' */
#ifndef SOAP_TYPE_conecta4ns__insertChipResponse
#define SOAP_TYPE_conecta4ns__insertChipResponse (26)
#endif

/* struct conecta4ns__getStatus has binding name 'conecta4ns__getStatus' for type 'conecta4ns:getStatus' */
#ifndef SOAP_TYPE_conecta4ns__getStatus
#define SOAP_TYPE_conecta4ns__getStatus (24)
#endif

/* struct conecta4ns__getStatusResponse has binding name 'conecta4ns__getStatusResponse' for type 'conecta4ns:getStatusResponse' */
#ifndef SOAP_TYPE_conecta4ns__getStatusResponse
#define SOAP_TYPE_conecta4ns__getStatusResponse (23)
#endif

/* struct conecta4ns__register has binding name 'conecta4ns__register' for type 'conecta4ns:register' */
#ifndef SOAP_TYPE_conecta4ns__register
#define SOAP_TYPE_conecta4ns__register (20)
#endif

/* struct conecta4ns__registerResponse has binding name 'conecta4ns__registerResponse' for type 'conecta4ns:registerResponse' */
#ifndef SOAP_TYPE_conecta4ns__registerResponse
#define SOAP_TYPE_conecta4ns__registerResponse (19)
#endif

/* conecta4ns__tBlock has binding name 'conecta4ns__tBlock' for type 'conecta4ns:tBlock' */
#ifndef SOAP_TYPE_conecta4ns__tBlock
#define SOAP_TYPE_conecta4ns__tBlock (16)
#endif

/* struct tBlock has binding name 'tBlock' for type 'tBlock' */
#ifndef SOAP_TYPE_tBlock
#define SOAP_TYPE_tBlock (14)
#endif

/* conecta4ns__tMessage has binding name 'conecta4ns__tMessage' for type 'conecta4ns:tMessage' */
#ifndef SOAP_TYPE_conecta4ns__tMessage
#define SOAP_TYPE_conecta4ns__tMessage (13)
#endif

/* struct tMessage has binding name 'tMessage' for type 'tMessage' */
#ifndef SOAP_TYPE_tMessage
#define SOAP_TYPE_tMessage (12)
#endif

/* struct SOAP_ENV__Reason * has binding name 'PointerToSOAP_ENV__Reason' for type '' */
#ifndef SOAP_TYPE_PointerToSOAP_ENV__Reason
#define SOAP_TYPE_PointerToSOAP_ENV__Reason (37)
#endif

/* struct SOAP_ENV__Detail * has binding name 'PointerToSOAP_ENV__Detail' for type '' */
#ifndef SOAP_TYPE_PointerToSOAP_ENV__Detail
#define SOAP_TYPE_PointerToSOAP_ENV__Detail (36)
#endif

/* struct SOAP_ENV__Code * has binding name 'PointerToSOAP_ENV__Code' for type '' */
#ifndef SOAP_TYPE_PointerToSOAP_ENV__Code
#define SOAP_TYPE_PointerToSOAP_ENV__Code (30)
#endif

/* struct tBlock * has binding name 'PointerToconecta4ns__tBlock' for type 'conecta4ns:tBlock' */
#ifndef SOAP_TYPE_PointerToconecta4ns__tBlock
#define SOAP_TYPE_PointerToconecta4ns__tBlock (21)
#endif

/* int * has binding name 'PointerToint' for type 'xsd:int' */
#ifndef SOAP_TYPE_PointerToint
#define SOAP_TYPE_PointerToint (17)
#endif

/* xsd__string has binding name 'xsd__string' for type 'xsd:string' */
#ifndef SOAP_TYPE_xsd__string
#define SOAP_TYPE_xsd__string (11)
#endif

/* _QName has binding name '_QName' for type 'xsd:QName' */
#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (6)
#endif

/* _XML has binding name '_XML' for type '' */
#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (5)
#endif

/* char * has binding name 'string' for type 'xsd:string' */
#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string (4)
#endif

/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Client-Side Call Stub Functions                                            *
 *                                                                            *
\******************************************************************************/

    
    /** Web service synchronous operation 'soap_call_conecta4ns__register' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 soap_call_conecta4ns__register(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct tMessage playerName, int *code);
    /** Web service asynchronous operation 'soap_send_conecta4ns__register' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 soap_send_conecta4ns__register(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct tMessage playerName);
    /** Web service asynchronous operation 'soap_recv_conecta4ns__register' to receive a response message from the connected endpoint, returns SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 soap_recv_conecta4ns__register(struct soap *soap, int *code);
    
    /** Web service synchronous operation 'soap_call_conecta4ns__getStatus' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 soap_call_conecta4ns__getStatus(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct tMessage playerName, struct tBlock *status);
    /** Web service asynchronous operation 'soap_send_conecta4ns__getStatus' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 soap_send_conecta4ns__getStatus(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct tMessage playerName);
    /** Web service asynchronous operation 'soap_recv_conecta4ns__getStatus' to receive a response message from the connected endpoint, returns SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 soap_recv_conecta4ns__getStatus(struct soap *soap, struct tBlock *status);
    
    /** Web service synchronous operation 'soap_call_conecta4ns__insertChip' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 soap_call_conecta4ns__insertChip(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct tMessage playerName, int playerMove, struct tBlock *status);
    /** Web service asynchronous operation 'soap_send_conecta4ns__insertChip' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 soap_send_conecta4ns__insertChip(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct tMessage playerName, int playerMove);
    /** Web service asynchronous operation 'soap_recv_conecta4ns__insertChip' to receive a response message from the connected endpoint, returns SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 soap_recv_conecta4ns__insertChip(struct soap *soap, struct tBlock *status);

/******************************************************************************\
 *                                                                            *
 * Server-Side Operations                                                     *
 *                                                                            *
\******************************************************************************/

    /** Web service operation 'conecta4ns__register' implementation, should return SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 conecta4ns__register(struct soap*, struct tMessage playerName, int *code);
    /** Web service operation 'conecta4ns__getStatus' implementation, should return SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 conecta4ns__getStatus(struct soap*, struct tMessage playerName, struct tBlock *status);
    /** Web service operation 'conecta4ns__insertChip' implementation, should return SOAP_OK or error code */
    SOAP_FMAC5 int SOAP_FMAC6 conecta4ns__insertChip(struct soap*, struct tMessage playerName, int playerMove, struct tBlock *status);

/******************************************************************************\
 *                                                                            *
 * Server-Side Skeletons to Invoke Service Operations                         *
 *                                                                            *
\******************************************************************************/

SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_conecta4ns__register(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_conecta4ns__getStatus(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_conecta4ns__insertChip(struct soap*);

#ifdef __cplusplus
}
#endif

#endif

/* End of soapStub.h */