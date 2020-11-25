#ifndef GRETIOWEBCONSTANTS_H
#define GRETIOWEBCONSTANTS_H

#endif // GRETIOWEBCONSTANTS_H



/**
 * This file contains agreed standards for ids, values, etc.
 */
/**
 * Type values
 */
#define MESSAGE_TYPE_VOID -1
#define MESSAGE_TYPE_REQUEST 1
#define MESSAGE_TYPE_RESPONSE 2
#define MESSAGE_TYPE_UNSOLICITED 3

/**
 * ID Values for services
 */
#define MESSAGE_ID_VOID -1
#define MESSAGE_ID_GETINFO 1
#define MESSAGE_ID_SUBSCRIBEINFO 2
#define MESSAGE_ID_STATUS 3
#define MESSAGE_ID_PROTOCOL 4
#define MESSAGE_ID_QUERY_MODULES 5
#define MESSAGE_ID_GET_ALL_FEATURES 6
#define MESSAGE_ID_GET_SUPPORTED_FEATURES 7
#define MESSAGE_ID_GET_MODULE_DTCS 8
#define MESSAGE_ID_CLEAR_MODULE_DTCS 9
#define MESSAGE_ID_GET_MODULE_INFO 10
#define MESSAGE_ID_GET_MODULE_CONTROLS 11
#define MESSAGE_ID_EXECUTE_MODULE_CONTROLS 12
#define MESSAGE_ID_RETURN_MODULE_TO_NORMAL 13
#define MESSAGE_ID_GET_MODULE_CONTROLS_INFO 14

#define MESSAGE_ID_BEGIN_RECORD 21
#define MESSAGE_ID_END_RECORD 22
#define MESSAGE_ID_GET_ACTIVE_RECORDS 23



#define MESSAGE_ID_NET_JOB_STATUS 95

#define MESSAGE_ID_AUTH 80

#define MESSAGE_ID_PT_CONNECT 9001
#define MESSAGE_ID_PT_DISCONNECT 9002
#define MESSAGE_ID_PT_READ_MSGS 9003
#define MESSAGE_ID_PT_START_PERIODIC 9004
#define MESSAGE_ID_PT_STOP_PERIODIC 9005
#define MESSAGE_ID_PT_START_MESSAGE_FILTER 9006
#define MESSAGE_ID_PT_STOP_MESSAGE_FILTER 9007
#define MESSAGE_ID_PT_WRITE_MSGS 9013










/**
 * General Response COdes (RCO)
 */

// This is the final sign off a server will make in a protocol before switching to a different one.
// Due to the async nature of the server, there may be remnants from the old protocol the client should handle.
#define RCO_OKAY_SWITCHING_PROTOCOLS 101


// SUCCESS CODES

#define RCO_OKAY 200

// Request sucessfully recieved, but will take too long.
// Norm is to retry in 1 second.
#define RCO_OKAY_PENDING 201

// Request sucessfully recieved, but cannot be fullfilled.
// Example would be if client requests PID 45, but PID 45 does
#define RCO_OKAY_NO_CONTENT 204

// Request sucessfully recieved, but has only been partially fulfilled.
#define RCO_OKAY_PARTIAL_CONENT 214

// When a service  is completely not supported.
#define RCO_UNKNOWN_OR_NOT_IMPLEMENTED_SERVICE 404




// Service request has bad or missing arguments.
#define RCO_MISSING_OR_INVALID_ARGUMENTS 422

// Service is locked for some other usage.
#define RCO_SERVICE_LOCKED 423



// Server is aware of service. But does not have the service implemented.
#define RCO_UNSUPPORTED_SERVICE 501

