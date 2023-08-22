/*
 * SPDX license identifier: MPL-2.0
 *
 * Copyright (C) 2011-2015, BMW AG
 *
 * This file is part of GENIVI Project DLT - Diagnostic Log and Trace.
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License (MPL), v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * For further information see http://www.genivi.org/.
 */

/*!
 * \author Alexander Wenzel <alexander.aw.wenzel@bmw.de>
 *
 * \copyright Copyright © 2011-2015 BMW AG. \n
 * License MPL-2.0: Mozilla Public License version 2.0 http://mozilla.org/MPL/2.0/.
 *
 * \file dlt_common.h
 */

/*******************************************************************************
**                                                                            **
**  SRC-MODULE: dlt_common.h                                                  **
**                                                                            **
**  TARGET    : linux                                                         **
**                                                                            **
**  PROJECT   : DLT                                                           **
**                                                                            **
**  AUTHOR    : Alexander Wenzel Alexander.AW.Wenzel@bmw.de                   **
**              Markus Klein                                                  **
**                                                                            **
**  PURPOSE   :                                                               **
**                                                                            **
**  REMARKS   :                                                               **
**                                                                            **
**  PLATFORM DEPENDANT [yes/no]: yes                                          **
**                                                                            **
**  TO BE CHANGED BY USER [yes/no]: no                                        **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Author Identity                                       **
********************************************************************************
**                                                                            **
** Initials     Name                       Company                            **
** --------     -------------------------  ---------------------------------- **
**  aw          Alexander Wenzel           BMW                                **
**  mk          Markus Klein               Fraunhofer ESK                     **
*******************************************************************************/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/

/*
 * $LastChangedRevision: 1670 $
 * $LastChangedDate: 2011-04-08 15:12:06 +0200 (Fr, 08. Apr 2011) $
 * $LastChangedBy$
 * Initials    Date         Comment
 * aw          13.01.2010   initial
 */
#ifndef DLT_COMMON_H
#   define DLT_COMMON_H

/**
 * \defgroup commonapi DLT Common API
 * \addtogroup commonapi
 \{
 */

#   include <netinet/in.h>
#   include <stdio.h>
#   include <stdbool.h>
#   ifdef __linux__
#      include <linux/limits.h>
#      include <sys/socket.h>
#   else
#      include <limits.h>
#   endif

#   if !defined(_MSC_VER)
#      include <unistd.h>
#      include <time.h>
#   endif

#   if defined(__GNUC__)
#      define PURE_FUNCTION __attribute__((pure))
#      define PRINTF_FORMAT(a,b) __attribute__ ((format (printf, a, b)))
#   else
#      define PURE_FUNCTION /* nothing */
#      define PRINTF_FORMAT(a,b) /* nothing */
#   endif

#   if !defined (__WIN32__) && !defined(_MSC_VER)
#      include <termios.h>
#   endif

#   include "dlt_types.h"
#   include "dlt_protocol.h"

#   define DLT_PACKED __attribute__((aligned(1), packed))

#   if defined (__MSDOS__) || defined (_MSC_VER)
/* set instead /Zp8 flag in Visual C++ configuration */
#      undef DLT_PACKED
#      define DLT_PACKED
#   endif

/*
 * Macros to swap the byte order.
 */
#   define DLT_SWAP_64(value) ((((uint64_t)DLT_SWAP_32((value) & 0xffffffffull)) << 32) | (DLT_SWAP_32((value) >> 32)))

#   define DLT_SWAP_16(value) ((((value) >> 8) & 0xff) | (((value) << 8) & 0xff00))
#   define DLT_SWAP_32(value) ((((value) >> 24) & 0xff) | (((value) << 8) & 0xff0000) | (((value) >> 8) & 0xff00) | \
                               (((value) << 24) & 0xff000000))

/* 如果没有定义，则将Big Endian和Little Endian设置为初始值 */
#   if !defined __USE_BSD
#      ifndef LITTLE_ENDIAN
#         define LITTLE_ENDIAN 1234
#      endif

#      ifndef BIG_ENDIAN
#         define BIG_ENDIAN    4321
#      endif
#   endif /* __USE_BSD */

/* If byte order is not defined, default to little endian */
#   if !defined __USE_BSD
#      ifndef BYTE_ORDER
#         define BYTE_ORDER LITTLE_ENDIAN
#      endif
#   endif /* __USE_BSD */

/* Check for byte-order */
#   if (BYTE_ORDER == BIG_ENDIAN)
/* #warning "Big Endian Architecture!" */
#      define DLT_HTOBE_16(x) ((x))
#      define DLT_HTOLE_16(x) DLT_SWAP_16((x))
#      define DLT_BETOH_16(x) ((x))
#      define DLT_LETOH_16(x) DLT_SWAP_16((x))

#      define DLT_HTOBE_32(x) ((x))
#      define DLT_HTOLE_32(x) DLT_SWAP_32((x))
#      define DLT_BETOH_32(x) ((x))
#      define DLT_LETOH_32(x) DLT_SWAP_32((x))

#      define DLT_HTOBE_64(x) ((x))
#      define DLT_HTOLE_64(x) DLT_SWAP_64((x))
#      define DLT_BETOH_64(x) ((x))
#      define DLT_LETOH_64(x) DLT_SWAP_64((x))
#   else
/* #warning "Litte Endian Architecture!" */
#      define DLT_HTOBE_16(x) DLT_SWAP_16((x))
#      define DLT_HTOLE_16(x) ((x))
#      define DLT_BETOH_16(x) DLT_SWAP_16((x))
#      define DLT_LETOH_16(x) ((x))

#      define DLT_HTOBE_32(x) DLT_SWAP_32((x))
#      define DLT_HTOLE_32(x) ((x))
#      define DLT_BETOH_32(x) DLT_SWAP_32((x))
#      define DLT_LETOH_32(x) ((x))

#      define DLT_HTOBE_64(x) DLT_SWAP_64((x))
#      define DLT_HTOLE_64(x) ((x))
#      define DLT_BETOH_64(x) DLT_SWAP_64((x))
#      define DLT_LETOH_64(x) ((x))
#   endif

#   define DLT_ENDIAN_GET_16(htyp, x) ((((htyp) & DLT_HTYP_MSBF) > 0) ? DLT_BETOH_16(x) : DLT_LETOH_16(x))
#   define DLT_ENDIAN_GET_32(htyp, x) ((((htyp) & DLT_HTYP_MSBF) > 0) ? DLT_BETOH_32(x) : DLT_LETOH_32(x))
#   define DLT_ENDIAN_GET_64(htyp, x) ((((htyp) & DLT_HTYP_MSBF) > 0) ? DLT_BETOH_64(x) : DLT_LETOH_64(x))

#   if defined (__WIN32__) || defined (_MSC_VER)
#      define LOG_EMERG     0
#      define LOG_ALERT     1
#      define LOG_CRIT      2
#      define LOG_ERR       3
#      define LOG_WARNING   4
#      define LOG_NOTICE    5
#      define LOG_INFO      6
#      define LOG_DEBUG     7

#      define LOG_PID     0x01
#      define LOG_DAEMON  (3 << 3)
#   endif

enum {
    DLT_LOG_TO_CONSOLE = 0,
    DLT_LOG_TO_SYSLOG = 1,
    DLT_LOG_TO_FILE = 2,
    DLT_LOG_TO_STDERR = 3,
    DLT_LOG_DROPPED = 4
};

/**
 * The standard TCP Port used for DLT daemon, can be overwritten via -p \<port\> when starting dlt-daemon
 */
#   define DLT_DAEMON_TCP_PORT 3490


/* Initial value for file descriptor */
#   define DLT_FD_INIT -1

/* Minimum value for a file descriptor except the POSIX Standards: stdin=0, stdout=1, stderr=2 */
#   define DLT_FD_MINIMUM 3

/**
 * The size of a DLT ID
 */
#   define DLT_ID_SIZE 4

#   define DLT_SIZE_WEID DLT_ID_SIZE
#   define DLT_SIZE_WSID (sizeof(uint32_t))
#   define DLT_SIZE_WTMS (sizeof(uint32_t))

/* Size of buffer for text output */
#define DLT_CONVERT_TEXTBUFSIZE  10024

/**
 * Definitions for GET_LOG_INFO
 */
#   define DLT_GET_LOG_INFO_HEADER 18  /*Get log info header size in response text */
#   define GET_LOG_INFO_LENGTH 13
#   define SERVICE_OPT_LENGTH 3

/**
 * Get the size of extra header parameters, depends on htyp.
 */
#   define DLT_STANDARD_HEADER_EXTRA_SIZE(htyp) ((DLT_IS_HTYP_WEID(htyp) ? DLT_SIZE_WEID : 0) + \
                                                 (DLT_IS_HTYP_WSID(htyp) ? DLT_SIZE_WSID : 0) + \
                                                 (DLT_IS_HTYP_WTMS(htyp) ? DLT_SIZE_WTMS : 0))


#   if defined (__MSDOS__) || defined (_MSC_VER)
#      define __func__ __FUNCTION__
#   endif

#   define PRINT_FUNCTION_VERBOSE(_verbose) \
    if (_verbose) \
        dlt_vlog(LOG_INFO, "%s()\n", __func__)

#   ifndef NULL
#      define NULL (char *)0
#   endif

#   define DLT_MSG_IS_CONTROL(MSG)          ((DLT_IS_HTYP_UEH((MSG)->standardheader->htyp)) && \
                                             (DLT_GET_MSIN_MSTP((MSG)->extendedheader->msin) == DLT_TYPE_CONTROL))

#   define DLT_MSG_IS_CONTROL_REQUEST(MSG)  ((DLT_IS_HTYP_UEH((MSG)->standardheader->htyp)) && \
                                             (DLT_GET_MSIN_MSTP((MSG)->extendedheader->msin) == DLT_TYPE_CONTROL) && \
                                             (DLT_GET_MSIN_MTIN((MSG)->extendedheader->msin) == DLT_CONTROL_REQUEST))

#   define DLT_MSG_IS_CONTROL_RESPONSE(MSG) ((DLT_IS_HTYP_UEH((MSG)->standardheader->htyp)) && \
                                             (DLT_GET_MSIN_MSTP((MSG)->extendedheader->msin) == DLT_TYPE_CONTROL) && \
                                             (DLT_GET_MSIN_MTIN((MSG)->extendedheader->msin) == DLT_CONTROL_RESPONSE))

#   define DLT_MSG_IS_CONTROL_TIME(MSG)     ((DLT_IS_HTYP_UEH((MSG)->standardheader->htyp)) && \
                                             (DLT_GET_MSIN_MSTP((MSG)->extendedheader->msin) == DLT_TYPE_CONTROL) && \
                                             (DLT_GET_MSIN_MTIN((MSG)->extendedheader->msin) == DLT_CONTROL_TIME))

#   define DLT_MSG_IS_NW_TRACE(MSG)         ((DLT_IS_HTYP_UEH((MSG)->standardheader->htyp)) && \
                                             (DLT_GET_MSIN_MSTP((MSG)->extendedheader->msin) == DLT_TYPE_NW_TRACE))

#   define DLT_MSG_IS_TRACE_MOST(MSG)       ((DLT_IS_HTYP_UEH((MSG)->standardheader->htyp)) && \
                                             (DLT_GET_MSIN_MSTP((MSG)->extendedheader->msin) == DLT_TYPE_NW_TRACE) && \
                                             (DLT_GET_MSIN_MTIN((MSG)->extendedheader->msin) == DLT_NW_TRACE_MOST))

#   define DLT_MSG_IS_NONVERBOSE(MSG)       (!(DLT_IS_HTYP_UEH((MSG)->standardheader->htyp)) || \
                                             ((DLT_IS_HTYP_UEH((MSG)->standardheader->htyp)) && \
                                              (!(DLT_IS_MSIN_VERB((MSG)->extendedheader->msin)))))

/*
 *
 * Definitions of DLT message buffer overflow
 */
#   define DLT_MESSAGE_BUFFER_NO_OVERFLOW     0x00/**< Buffer overflow has not occured */
#   define DLT_MESSAGE_BUFFER_OVERFLOW        0x01/**< Buffer overflow has occured */

/*
 * Definition of DLT output variants
 */
#   define DLT_OUTPUT_HEX              1
#   define DLT_OUTPUT_ASCII            2
#   define DLT_OUTPUT_MIXED_FOR_PLAIN  3
#   define DLT_OUTPUT_MIXED_FOR_HTML   4
#   define DLT_OUTPUT_ASCII_LIMITED    5

#   define DLT_FILTER_MAX 30 /**< Maximum number of filters */

#   define DLT_MSG_READ_VALUE(dst, src, length, type) \
    do { \
        if ((length < 0) || ((length) < ((int32_t)sizeof(type)))) \
        { length = -1; } \
        else \
        { dst = *((type *)src); src += sizeof(type); length -= sizeof(type); } \
    } while(0)

#   define DLT_MSG_READ_ID(dst, src, length) \
    do { \
        if ((length < 0) || ((length) < DLT_ID_SIZE)) \
        { length = -1; } \
        else \
        { memcpy(dst, src, DLT_ID_SIZE); src += DLT_ID_SIZE; length -= DLT_ID_SIZE; } \
    } while(0)

#   define DLT_MSG_READ_STRING(dst, src, maxlength, dstlength, length) \
    do { \
        if ((maxlength < 0) || (length <= 0) || (dstlength < length) || (maxlength < length)) \
        { \
            maxlength = -1; \
        } \
        else \
        { \
            memcpy(dst, src, length); \
            dlt_clean_string(dst, length); \
            dst[length] = 0; \
            src += length; \
            maxlength -= length; \
        } \
    } while(0)

#   define DLT_MSG_READ_NULL(src, maxlength, length) \
    do { \
        if (((maxlength) < 0) || ((length) < 0) || ((maxlength) < (length))) \
        { length = -1; } \
        else \
        { src += length; maxlength -= length; } \
    } while(0)

#   define DLT_HEADER_SHOW_NONE       0x0000
#   define DLT_HEADER_SHOW_TIME       0x0001
#   define DLT_HEADER_SHOW_TMSTP      0x0002
#   define DLT_HEADER_SHOW_MSGCNT     0x0004
#   define DLT_HEADER_SHOW_ECUID      0x0008
#   define DLT_HEADER_SHOW_APID       0x0010
#   define DLT_HEADER_SHOW_CTID       0x0020
#   define DLT_HEADER_SHOW_MSGTYPE    0x0040
#   define DLT_HEADER_SHOW_MSGSUBTYPE 0x0080
#   define DLT_HEADER_SHOW_VNVSTATUS  0x0100
#   define DLT_HEADER_SHOW_NOARG      0x0200
#   define DLT_HEADER_SHOW_ALL        0xFFFF

/* dlt_receiver_check_and_get flags */
#   define DLT_RCV_NONE        0
#   define DLT_RCV_SKIP_HEADER (1 << 0)
#   define DLT_RCV_REMOVE      (1 << 1)

/**
 * Maximal length of path in DLT
 * DLT limits the path length and does not do anything else to determine
 * the actual value, because the least that is supported on any system
 * that DLT runs on is 1024 bytes.
 */
#   define DLT_PATH_MAX 1024

/**
 * Maximal length of mounted path
 */
#   define DLT_MOUNT_PATH_MAX  1024

/**
 * Maximal length of an entry
 */
#   define DLT_ENTRY_MAX 100

/**
 * Maximal IPC path len
 */
#   define DLT_IPC_PATH_MAX 100

/**
 * Maximal receiver buffer size for application messages
 */
#   define DLT_RECEIVE_BUFSIZE 65535

/**
 * Maximal line length
 */
#   define DLT_LINE_LEN 1024

/**
 * Macros for network trace
 */
#define DLT_TRACE_NW_TRUNCATED "NWTR"
#define DLT_TRACE_NW_START "NWST"
#define DLT_TRACE_NW_SEGMENT "NWCH"
#define DLT_TRACE_NW_END "NWEN"

/**
 * Provision to test static function
 */
#   ifndef DLT_UNIT_TESTS
#      define DLT_STATIC static
#   else
#      define DLT_STATIC
#   endif

/**
 * Type to specify whether received data is from socket or file/fifo
 */
typedef enum
{
    DLT_RECEIVE_SOCKET,
    DLT_RECEIVE_UDP_SOCKET,
    DLT_RECEIVE_FD
} DltReceiverType;

/**
 * The definition of the serial header containing the characters "DLS" + 0x01.
 */
extern const char dltSerialHeader[DLT_ID_SIZE];

/**
 * The definition of the serial header containing the characters "DLS" + 0x01 as char.
 */
extern char dltSerialHeaderChar[DLT_ID_SIZE];


/**
 * The common base-path of the dlt-daemon-fifo and application-generated fifos
 */
extern char dltFifoBaseDir[DLT_PATH_MAX];


#ifdef DLT_SHM_ENABLE
/**
 * The common name of the dlt-daemon and application share memory
 */
extern char dltShmName[NAME_MAX + 1];
#endif

/**
 * The type of a DLT ID (context id, application id, etc.)
 */
typedef char ID4[DLT_ID_SIZE];

/**
 * The structure of the DLT file storage header. This header is used before each stored DLT message.
 */
typedef struct
{
    char pattern[DLT_ID_SIZE]; /**< This pattern should be DLT0x01 */
    uint32_t seconds;          /**< seconds since 1.1.1970 */
    int32_t microseconds;      /**< Microseconds */
    char ecu[DLT_ID_SIZE];     /**< The ECU id is added, if it is not already in the DLT message itself */
} DLT_PACKED DltStorageHeader;

/**
 * The structure of the DLT standard header. This header is used in each DLT message.
 */
typedef struct
{
    uint8_t htyp;           /**< This parameter contains several informations, see definitions below */
    uint8_t mcnt;           /**< The message counter is increased with each sent DLT message */
    uint16_t len;           /**< Length of the complete message, without storage header */
} DLT_PACKED DltStandardHeader;

/**
 * The structure of the DLT extra header parameters. Each parameter is sent only if enabled in htyp.
 */
typedef struct
{
    char ecu[DLT_ID_SIZE];       /**< ECU id */
    uint32_t seid;               /**< Session number */
    uint32_t tmsp;               /**< Timestamp since system start in 0.1 milliseconds */
} DLT_PACKED DltStandardHeaderExtra;

/**
 * The structure of the DLT extended header. This header is only sent if enabled in htyp parameter.
 */
typedef struct
{
    uint8_t msin;              /**< messsage info */
    uint8_t noar;              /**< number of arguments */
    char apid[DLT_ID_SIZE];    /**< application id */
    char ctid[DLT_ID_SIZE];    /**< context id */
} DLT_PACKED DltExtendedHeader;

/**
 * The structure to organise the DLT messages.
 * This structure is used by the corresponding functions.
 */
typedef struct sDltMessage
{
    /* flags */
    int8_t found_serialheader;

    /* offsets */
    int32_t resync_offset;

    /* size parameters */
    int32_t headersize;    /**< size of complete header including storage header */
    int32_t datasize;      /**< size of complete payload */

    /* buffer for current loaded message */
    uint8_t headerbuffer[sizeof(DltStorageHeader) +
                         sizeof(DltStandardHeader) + sizeof(DltStandardHeaderExtra) + sizeof(DltExtendedHeader)]; /**< buffer for loading complete header */
    uint8_t *databuffer;         /**< buffer for loading payload */
    int32_t databuffersize;

    /* header values of current loaded message */
    DltStorageHeader *storageheader;        /**< pointer to storage header of current loaded header */
    DltStandardHeader *standardheader;      /**< pointer to standard header of current loaded header */
    DltStandardHeaderExtra headerextra;     /**< extra parameters of current loaded header */
    DltExtendedHeader *extendedheader;      /**< pointer to extended of current loaded header */
} DltMessage;

/**
 * The structure of the DLT Service Get Log Info.
 */
typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t options;                /**< type of request */
    char apid[DLT_ID_SIZE];         /**< application id */
    char ctid[DLT_ID_SIZE];         /**< context id */
    char com[DLT_ID_SIZE];          /**< communication interface */
} DLT_PACKED DltServiceGetLogInfoRequest;

typedef struct
{
    uint32_t service_id;            /**< service ID */
} DLT_PACKED DltServiceGetDefaultLogLevelRequest;

/**
 * The structure of the DLT Service Get Log Info response.
 */
typedef struct
{
    char context_id[DLT_ID_SIZE];
    int16_t log_level;
    int16_t trace_status;
    uint16_t len_context_description;
    char *context_description;
} ContextIDsInfoType;

typedef struct
{
    char app_id[DLT_ID_SIZE];
    uint16_t count_context_ids;
    ContextIDsInfoType *context_id_info; /**< holds info about a specific con id */
    uint16_t len_app_description;
    char *app_description;
} AppIDsType;

typedef struct
{
    uint16_t count_app_ids;
    AppIDsType *app_ids;            /**< holds info about a specific app id */
} LogInfoType;

typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t status;                 /**< type of request */
    LogInfoType log_info_type;      /**< log info type */
    char com[DLT_ID_SIZE];      /**< communication interface */
} DltServiceGetLogInfoResponse;

/**
 * The structure of the DLT Service Set Log Level.
 */
typedef struct
{

    uint32_t service_id;            /**< service ID */
    char apid[DLT_ID_SIZE];         /**< application id */
    char ctid[DLT_ID_SIZE];         /**< context id */
    uint8_t log_level;              /**< log level to be set */
    char com[DLT_ID_SIZE];          /**< communication interface */
} DLT_PACKED DltServiceSetLogLevel;

/**
 * The structure of the DLT Service Set Default Log Level.
 */
typedef struct
{
    uint32_t service_id;                /**< service ID */
    uint8_t log_level;                  /**< default log level to be set */
    char com[DLT_ID_SIZE];              /**< communication interface */
} DLT_PACKED DltServiceSetDefaultLogLevel;

/**
 * The structure of the DLT Service Set Verbose Mode
 */
typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t new_status;             /**< new status to be set */
} DLT_PACKED DltServiceSetVerboseMode;

/**
 * The structure of the DLT Service Set Communication Interface Status
 */
typedef struct
{
    uint32_t service_id;            /**< service ID */
    char com[DLT_ID_SIZE];          /**< communication interface */
    uint8_t new_status;             /**< new status to be set */
} DLT_PACKED DltServiceSetCommunicationInterfaceStatus;

/**
 * The structure of the DLT Service Set Communication Maximum Bandwidth
 */
typedef struct
{
    uint32_t service_id;            /**< service ID */
    char com[DLT_ID_SIZE];          /**< communication interface */
    uint32_t max_bandwidth;         /**< maximum bandwith */
} DLT_PACKED DltServiceSetCommunicationMaximumBandwidth;

typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t status;                 /**< reponse status */
} DLT_PACKED DltServiceResponse;

typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t status;                 /**< reponse status */
    uint8_t log_level;              /**< log level */
} DLT_PACKED DltServiceGetDefaultLogLevelResponse;

typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t status;                 /**< reponse status */
    uint8_t overflow;               /**< overflow status */
    uint32_t overflow_counter;      /**< overflow counter */
} DLT_PACKED DltServiceMessageBufferOverflowResponse;

typedef struct
{
    uint32_t service_id;            /**< service ID */
} DLT_PACKED DltServiceGetSoftwareVersion;

typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t status;                 /**< reponse status */
    uint32_t length;                /**< length of following payload */
    char *payload;                  /**< payload */
} DLT_PACKED DltServiceGetSoftwareVersionResponse;

/**
 * The structure of the DLT Service Unregister Context.
 */
typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t status;                 /**< reponse status */
    char apid[DLT_ID_SIZE];         /**< application id */
    char ctid[DLT_ID_SIZE];         /**< context id */
    char comid[DLT_ID_SIZE];        /**< communication interface */
} DLT_PACKED DltServiceUnregisterContext;

/**
 * The structure of the DLT Service Connection Info
 */
typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t status;                 /**< reponse status */
    uint8_t state;                  /**< new state */
    char comid[DLT_ID_SIZE];        /**< communication interface */
} DLT_PACKED DltServiceConnectionInfo;

/**
 * The structure of the DLT Service Timezone
 */
typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t status;                 /**< reponse status */
    int32_t timezone;               /**< Timezone in seconds */
    uint8_t isdst;                  /**< Is daylight saving time */
} DLT_PACKED DltServiceTimezone;

/**
 * The structure of the DLT Service Marker
 */
typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint8_t status;                 /**< reponse status */
} DLT_PACKED DltServiceMarker;

/***
 * The structure of the DLT Service Offline Logstorage
 */
typedef struct
{
    uint32_t service_id;                  /**< service ID */
    char mount_point[DLT_MOUNT_PATH_MAX]; /**< storage device mount point */
    uint8_t connection_type;              /**< connection status of the connected device connected/disconnected */
    char comid[DLT_ID_SIZE];              /**< communication interface */
} DLT_PACKED DltServiceOfflineLogstorage;

typedef struct
{
    uint32_t service_id;            /**< service ID */
    uint32_t connection_status;     /**< connect/disconnect */
    char node_id[DLT_ID_SIZE];      /**< passive node ID */
} DLT_PACKED DltServicePassiveNodeConnect;

/**
 * The structure of DLT Service Passive Node Connection Status
 */
typedef struct
{
    uint32_t service_id;                       /**< service ID */
    uint8_t status;                            /**< response status */
    uint32_t num_connections;                  /**< number of connections */
    uint8_t connection_status[DLT_ENTRY_MAX];  /**< list of connection status */
    char node_id[DLT_ENTRY_MAX];               /**< list of passive node IDs */
} DLT_PACKED DltServicePassiveNodeConnectionInfo;

/**
 * Structure to store filter parameters.
 * ID are maximal four characters. Unused values are filled with zeros.
 * If every value as filter is valid, the id should be empty by having only zero values.
 */
typedef struct
{
    char apid[DLT_FILTER_MAX][DLT_ID_SIZE]; /**< application id */
    char ctid[DLT_FILTER_MAX][DLT_ID_SIZE]; /**< context id */
    int log_level[DLT_FILTER_MAX];          /**< log level */
    int32_t payload_max[DLT_FILTER_MAX];        /**< upper border for payload */
    int32_t payload_min[DLT_FILTER_MAX];        /**< lower border for payload */
    int counter;                            /**< number of filters */
} DltFilter;

/**
 * The structure to organise the access to DLT files.
 * This structure is used by the corresponding functions.
 */
typedef struct sDltFile
{
    /* file handle and index for fast access */
    FILE *handle;      /**< file handle of opened DLT file */
    long *index;       /**< file positions of all DLT messages for fast access to file, only filtered messages */

    /* size parameters */
    int32_t counter;       /**< number of messages in DLT file with filter */
    int32_t counter_total; /**< number of messages in DLT file without filter */
    int32_t position;      /**< current index to message parsed in DLT file starting at 0 */
    uint64_t file_length;    /**< length of the file */
    uint64_t file_position;  /**< current position in the file */

    /* error counters */
    int32_t error_messages; /**< number of incomplete DLT messages found during file parsing */

    /* filter parameters */
    DltFilter *filter;      /**< pointer to filter list. Zero if no filter is set. */
    int32_t filter_counter; /**< number of filter set */

    /* current loaded message */
    DltMessage msg;     /**< pointer to message */

} DltFile;

/**
 * The structure is used to organise the receiving of data
 * including buffer handling.
 * This structure is used by the corresponding functions.
 */
typedef struct
{
    int32_t lastBytesRcvd;    /**< bytes received in last receive call */
    int32_t bytesRcvd;        /**< received bytes */
    int32_t totalBytesRcvd;   /**< total number of received bytes */
    char *buffer;         /**< pointer to receiver buffer */
    char *buf;            /**< pointer to position within receiver buffer */
    char *backup_buf;     /** pointer to the buffer with partial messages if any **/
    int fd;               /**< connection handle */
    DltReceiverType type;     /**< type of connection handle */
    int32_t buffersize;       /**< size of receiver buffer */
    struct sockaddr_in addr;  /**< socket address information */
} DltReceiver;

typedef struct
{
    unsigned char *shm; /* pointer to beginning of shared memory */
    unsigned int size;  /* size of data area in shared memory */
    unsigned char *mem; /* pointer to data area in shared memory */

    uint32_t min_size;     /**< Minimum size of buffer */
    uint32_t max_size;     /**< Maximum size of buffer */
    uint32_t step_size;    /**< Step size of buffer */
} DltBuffer;

typedef struct
{
    int write;
    int read;
    int count;
} DltBufferHead;

#   define DLT_BUFFER_HEAD "SHM"

typedef struct
{
    char head[4];
    unsigned char status;
    int size;
} DltBufferBlockHead;

#   ifdef DLT_USE_IPv6
#      define DLT_IP_SIZE (INET6_ADDRSTRLEN)
#   else
#      define DLT_IP_SIZE (INET_ADDRSTRLEN)
#   endif
typedef struct DltBindAddress
{
    char ip[DLT_IP_SIZE];
    struct DltBindAddress *next;
} DltBindAddress_t;

#   define DLT_MESSAGE_ERROR_OK       0
#   define DLT_MESSAGE_ERROR_UNKNOWN -1
#   define DLT_MESSAGE_ERROR_SIZE    -2
#   define DLT_MESSAGE_ERROR_CONTENT -3

#   ifdef __cplusplus
extern "C"
{
#   endif

/**
 * 辅助函数打印十六进制字节数组。
 * @param ptr pointer to the byte array.
 * @param size number of bytes to be printed.
 */
void dlt_print_hex(uint8_t *ptr, int size);
/**
 * 将十六进制字节数组打印成字符串的辅助函数。
 * @param text pointer to a ASCII string, in which the text is written
 * @param textlength maximal size of text buffer
 * @param ptr pointer to the byte array.
 * @param size number of bytes to be printed.
 * @return negative value if there was an error
 */
DltReturnValue dlt_print_hex_string(char *text, int textlength, uint8_t *ptr, int size);
/**
 * 将十六进制和ascii格式的字节数组打印成字符串的辅助函数.
 * @param text pointer to a ASCII string, in which the text is written
 * @param textlength maximal size of text buffer
 * @param ptr pointer to the byte array.
 * @param size number of bytes to be printed.
 * @param html output is html? 0 - false, 1 - true
 * @return negative value if there was an error
 */
DltReturnValue dlt_print_mixed_string(char *text, int textlength, uint8_t *ptr, int size, int html);
/**
 * 辅助函数，用于将ascii格式的字节数组打印成字符串。
 * @param text pointer to a ASCII string, in which the text is written
 * @param textlength maximal size of text buffer
 * @param ptr pointer to the byte array.
 * @param size number of bytes to be printed.
 * @return negative value if there was an error
 */
DltReturnValue dlt_print_char_string(char **text, int textlength, uint8_t *ptr, int size);

/**
 * 辅助函数，用于确定字符串的有界长度。
如果@a str是空指针，这个函数返回0;
如果在@a str的第一个@a maxsize字节中没有找到空字符，则返回@a maxsize。
*这是C11的strnlen_s的重新实现，我们还不能假设它可用。
 * @param str pointer to string whose length is to be determined
 * @param maxsize maximal considered length of @a str
 * @return the bounded length of the string
 */
PURE_FUNCTION size_t dlt_strnlen_s(const char* str, size_t maxsize);

/**
 * 打印id的辅助函数。
 * @param text pointer to ASCII string where to write the id
 * @param id four byte char array as used in DLT mesages as IDs.
 */
void dlt_print_id(char *text, const char *id);

/**
 * 辅助函数，用于设置ID参数。
 * @param id four byte char array as used in DLT mesages as IDs.
 * @param text string to be copied into char array.
 */
void dlt_set_id(char *id, const char *text);

/**
 * 辅助函数用于删除不适合打印的字符，例如NULL或回车符。
 * @param text pointer to string to be cleaned.
 * @param length length of string excluding terminating zero.
 */
void dlt_clean_string(char *text, int length);

/**
 * 初始化过滤器列表。
*在使用dlt过滤器之前必须调用此函数。
 * @param filter pointer to structure of organising DLT filter
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_filter_init(DltFilter *filter, int verbose);
/**
 * 通过过滤器的组织结构释放已使用的内存。
 * @param filter pointer to structure of organising DLT filter
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_filter_free(DltFilter *filter, int verbose);
/**
 * 从文件加载过滤器列表.
 * @param filter pointer to structure of organising DLT filter
 * @param filename filename to load filters from
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_filter_load(DltFilter *filter, const char *filename, int verbose);
/**
 * 保存过滤器在空格分隔列表到文本文件。
 * @param filter pointer to structure of organising DLT filter
 * @param filename filename to safe filters into
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_filter_save(DltFilter *filter, const char *filename, int verbose);
/**
 * 在过滤器列表中查找过滤器的索引
 * @param filter pointer to structure of organising DLT filter
 * @param apid application id to be found in filter list
 * @param ctid context id to be found in filter list
 * @param log_level log level to be found in filter list
 * @param payload_min minimum payload lenght to be found in filter list
 * @param payload_max maximum payload lenght to be found in filter list
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error (or not found), else return index of filter
 */
int dlt_filter_find(DltFilter *filter, const char *apid, const char *ctid, const int log_level,
                                const int32_t payload_min, const int32_t payload_max, int verbose);
/**
 * 向过滤器列表中添加新的过滤器
 * @param filter pointer to structure of organising DLT filter
 * @param apid application id to be added to filter list (must always be set).
 * @param ctid context id to be added to filter list. empty equals don't care.
 * @param log_level log level to be added to filter list. 0 equals don't care.
 * @param payload_min min lenght of payload to be added to filter list. 0 equals don't care.
 * @param payload_max max lenght of payload to be added to filter list. INT32_MAX equals don't care.
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_filter_add(DltFilter *filter, const char *apid, const char *ctid, const int log_level,
                                const int32_t payload_min, const int32_t payload_max, int verbose);
/**
 * 从过滤器列表中删除过滤器
 * @param filter pointer to structure of organising DLT filter
 * @param apid application id to be deleted from filter list
 * @param ctid context id to be deleted from filter list
 * @param log_level log level to be deleted from filter list
 * @param payload_min minimum payload lenght to be deleted from filter list
 * @param payload_max maximum payload lenght to be deleted from filter list
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_filter_delete(DltFilter *filter, const char *apid, const char *ctid, const int log_level,
                                const int32_t payload_min, const int32_t payload_max, int verbose);

/**
 * 初始化用于访问DLT消息的结构。
这个函数必须在使用其他的dlt_message函数之前调用。
 * @param msg pointer to structure of organising access to DLT messages
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_init(DltMessage *msg, int verbose);
/**
 * 通过文件的组织结构释放已使用的内存。
 * @param msg pointer to structure of organising access to DLT messages
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_free(DltMessage *msg, int verbose);
/**
 * 将标头打印成ASCII字符串。
这个函数调用dlt_message_header_flags()， flags=DLT_HEADER_SHOW_ALL
 * @param msg pointer to structure of organising access to DLT messages
 * @param text pointer to a ASCII string, in which the header is written
 * @param textlength maximal size of text buffer
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_header(DltMessage *msg, char *text, size_t textlength, int verbose);
/**
 * 将标头打印成ASCII字符串，可选。
 * @param msg pointer to structure of organising access to DLT messages
 * @param text pointer to a ASCII string, in which the header is written
 * @param textlength maximal size of text buffer
 * @param flags select, bit-field to select, what should be printed (DLT_HEADER_SHOW_...)
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_header_flags(DltMessage *msg, char *text, size_t textlength, int flags, int verbose);
/**
 * 将有效载荷打印成ASCII字符串。
 * @param msg pointer to structure of organising access to DLT messages
 * @param text pointer to a ASCII string, in which the header is written
 * @param textlength maximal size of text buffer
 * @param type 1 = payload as hex, 2 = payload as ASCII.
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_payload(DltMessage *msg, char *text, size_t textlength, int type, int verbose);
/**
 * 检查消息是否被过滤。应用所有筛选器(逻辑或)
 * @param msg pointer to structure of organising access to DLT messages
 * @param filter pointer to filter
 * @param verbose if set to true verbose information is printed out.
 * @return 1 = filter matches, 0 = filter does not match, negative value if there was an error
 */
DltReturnValue dlt_message_filter_check(DltMessage *msg, DltFilter *filter, int verbose);

/**
 * 从内存缓冲区读取消息。
缓冲区中的消息没有存储头。
 * @param msg pointer to structure of organising access to DLT messages
 * @param buffer pointer to memory buffer
 * @param length length of message in buffer
 * @param resync if set to true resync to serial header is enforced
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
int dlt_message_read(DltMessage *msg, uint8_t *buffer, unsigned int length, int resync, int verbose);

/**
 * 获取标准标头额外参数
 * @param msg pointer to structure of organising access to DLT messages
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_get_extraparameters(DltMessage *msg, int verbose);

/**
 * 设置标准标头额外参数
 * @param msg pointer to structure of organising access to DLT messages
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_set_extraparameters(DltMessage *msg, int verbose);

/**
 * 初始化用于访问DLT文件的结构。
这个函数必须在使用其他dlt_file函数之前调用。
 * @param file pointer to structure of organising access to DLT file
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_file_init(DltFile *file, int verbose);
/**
 * 设置过滤器列表。
*如果要使用过滤器，这个函数应该在加载DLT文件之前调用。
*过滤器列表是一个过滤器数组。多个过滤器通过or操作逻辑地组合在一起。
*过滤器列表不会被复制，所以要注意在内存中保存列表。
 * @param file pointer to structure of organising access to DLT file
 * @param filter pointer to filter list array
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_file_set_filter(DltFile *file, DltFilter *filter, int verbose);
/**
 * 初始化加载DLT文件。
 * @param file pointer to structure of organising access to DLT file
 * @param filename filename of DLT file
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_file_open(DltFile *file, const char *filename, int verbose);
/**
 * 该函数读取DLT文件并逐个解析DLT消息。
每条消息将被写入新的文件。
*如果设置了过滤器，则使用过滤器列表。
 * @param file pointer to structure of organizing access to DLT file
 * @param filename file to contain parsed DLT messages.
 * @param type 1 = payload as hex, 2 = payload as ASCII.
 * @param verbose if set to true verbose information is printed out.
 * @return 0 = message does not match filter, 1 = message was read, negative value if there was an error
 */
DltReturnValue dlt_file_quick_parsing(DltFile *file, const char *filename, int type, int verbose);
/**
 * 在DLT文件中查找下一条消息并解析它们。
*该函数在DLT文件中查找下一条消息。
*如果设置了过滤器，则使用过滤器列表。
 * @param file pointer to structure of organising access to DLT file
 * @param verbose if set to true verbose information is printed out.
 * @return 0 = message does not match filter, 1 = message was read, negative value if there was an error
 */
DltReturnValue dlt_file_read(DltFile *file, int verbose);
/**
 * 在RAW格式的DLT文件中找到下一条消息(没有存储头)并解析它们。
*该函数在DLT文件中查找下一条消息。
*如果设置了过滤器，则使用过滤器列表。
 * @param file pointer to structure of organising access to DLT file
 * @param resync Resync to serial header when set to true
 * @param verbose if set to true verbose information is printed out.
 * @return 0 = message does not match filter, 1 = message was read, negative value if there was an error
 */
DltReturnValue dlt_file_read_raw(DltFile *file, int resync, int verbose);
/**
 * 正在关闭加载的DLT文件.
 * @param file pointer to structure of organising access to DLT file
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_file_close(DltFile *file, int verbose);
/**
 * 从文件中加载消息的标准头
 * @param file pointer to structure of organising access to DLT file
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_file_read_header(DltFile *file, int verbose);
/**
 * 以RAW格式从文件中加载消息的标准标头(没有存储标头)
 * @param file pointer to structure of organising access to DLT file
 * @param resync Resync to serial header when set to true
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_file_read_header_raw(DltFile *file, int resync, int verbose);
/**
 * 加载，如果消息中可用，额外的标准头字段和
*来自文件的消息的扩展头
* (dlt_file_read_header()必须在此调用之前被调用!)
 * @param file pointer to structure of organising access to DLT file
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_file_read_header_extended(DltFile *file, int verbose);
/**
 * 从文件加载消息的有效负载
* (dlt_file_read_header()必须在此调用之前被调用!)
 * @param file pointer to structure of organising access to DLT file
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_file_read_data(DltFile *file, int verbose);
/**
 * 由索引选择的消息的加载头和有效负载。
*如果设置了过滤器，则以过滤后的列表为索引。
 * @param file pointer to structure of organising access to DLT file
 * @param index position of message in the files beginning from zero
 * @param verbose if set to true verbose information is printed out.
 * @return number of messages loaded, negative value if there was an error
 */
DltReturnValue dlt_file_message(DltFile *file, int index, int verbose);
/**
 * 通过文件的组织结构释放已使用的内存。
 * @param file pointer to structure of organising access to DLT file
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_file_free(DltFile *file, int verbose);

/**
 * 设置内部日志文件名，如果模式2
 * @param filename the filename
 */
void dlt_log_set_filename(const char *filename);
#if defined DLT_DAEMON_USE_FIFO_IPC || defined DLT_LIB_USE_FIFO_IPC
/**
 * 设置FIFO基方向
 * @param pipe_dir the pipe direction
 */
void dlt_log_set_fifo_basedir(const char *pipe_dir);
#endif
/**
 *设置内部日志级别
 * @param level the level
 */
void dlt_log_set_level(int level);

/**
 * 设置是否在控制台输出中打印“name”和“unit”属性
 * @param state  true = with attributes, false = without attributes
 */
void dlt_print_with_attributes(bool state);

/**
 * 初始化(外部)日志记录设施
 * @param mode positive, 0 = log to stdout, 1 = log to syslog, 2 = log to file, 3 = log to stderr
 */
void dlt_log_init(int mode);
/**
 * 通过DLT_LOG_MODE环境变量(如fprintf)将变量参数打印到指定的文件描述符
 * @param format format string for message
 * @return negative value if there was an error or the total number of characters written is returned on success
 */
int dlt_user_printf(const char *format, ...) PRINTF_FORMAT(1, 2);
/**
 * 将带有空终止符的ASCII字符串记录到(外部)日志记录设施
 * @param prio priority (see syslog() call)
 * @param s Pointer to ASCII string with null-termination
 * @return negative value if there was an error
 */
DltReturnValue dlt_log(int prio, char *s);
/**
 * 带变量参数的日志到(外部)日志工具(如printf)
 * @param prio priority (see syslog() call)
 * @param format format string for log message
 * @return negative value if there was an error
 */
DltReturnValue dlt_vlog(int prio, const char *format, ...) PRINTF_FORMAT(2, 3);
/**
 * 带可变参数的日志大小字节到(外部)日志工具(类似于snprintf)
 * @param prio priority (see syslog() call)
 * @param size number of bytes to log
 * @param format format string for log message
 * @return negative value if there was an error
 */
DltReturnValue dlt_vnlog(int prio, size_t size, const char *format, ...) PRINTF_FORMAT(3, 4);
/**
 * 取消初始化(外部)日志记录设施
 */
void dlt_log_free(void);

/**
 * 初始化dlt接收器结构
 * @param receiver pointer to dlt receiver structure
 * @param _fd handle to file/socket/fifo, fram which the data should be received
 * @param type specify whether received data is from socket or file/fifo
 * @param _buffersize size of data buffer for storing the received data
 * @return negative value if there was an error
 */
DltReturnValue dlt_receiver_init(DltReceiver *receiver, int _fd, DltReceiverType type, int _buffersize);
/**
 * 释放dlt接收器结构
 * @param receiver pointer to dlt receiver structure
 * @return negative value if there was an error
 */
DltReturnValue dlt_receiver_free(DltReceiver *receiver);
/**
 * 初始化dlt接收器结构
 * @param receiver pointer to dlt receiver structure
 * @param fd handle to file/socket/fifo, fram which the data should be received
 * @param type specify whether received data is from socket or file/fifo
 * @param buffer data buffer for storing the received data
 * @return negative value if there was an error and zero if success
 */
DltReturnValue dlt_receiver_init_global_buffer(DltReceiver *receiver, int fd, DltReceiverType type, char **buffer);
/**
 * 释放dlt接收器结构
 * @param receiver pointer to dlt receiver structure
 * @return negative value if there was an error and zero if success
 */
DltReturnValue dlt_receiver_free_global_buffer(DltReceiver *receiver);
/**
 * 使用dlt接收结构从套接字或文件/fifo接收数据
 * @param receiver pointer to dlt receiver structure
 * @return number of received bytes or negative value if there was an error
 */
int dlt_receiver_receive(DltReceiver *receiver);
/**
 * 从接收到的数据中删除特定大小的字节
 * @param receiver pointer to dlt receiver structure
 * @param size amount of bytes to be removed
 * @return negative value if there was an error
 */
DltReturnValue dlt_receiver_remove(DltReceiver *receiver, int size);
/**
 * 将数据从最后一个接收调用移动到接收缓冲区的前面
 * @param receiver pointer to dlt receiver structure
 * @return negative value if there was an error
 */
DltReturnValue dlt_receiver_move_to_begin(DltReceiver *receiver);

/**
 * 检查接收端和接收端是否有to_get的数据量
*复制到dest，如果skip_header设置为1，跳过DltUserHeader.
 * @param receiver pointer to dlt receiver structure
 * @param dest pointer to the destination buffer
 * @param to_get size of the data to copy in dest
 * @param skip_header whether if the DltUserHeader must be skipped.
 */
int dlt_receiver_check_and_get(DltReceiver *receiver,
                               void *dest,
                               unsigned int to_get,
                               unsigned int skip_header);

/**
 * 填写dlt消息的存储头
 * @param storageheader pointer to storage header of a dlt message
 * @param ecu name of ecu to be set in storage header
 * @return negative value if there was an error
 */
DltReturnValue dlt_set_storageheader(DltStorageHeader *storageheader, const char *ecu);
/**
 * 检查存储头是否包含其标记
 * @param storageheader pointer to storage header of a dlt message
 * @return 0 no, 1 yes, negative value if there was an error
 */
DltReturnValue dlt_check_storageheader(DltStorageHeader *storageheader);

/**
 * 检查接收的大小是否足以容纳预期的数据
 * @param received size
 * @param required size
 * @return negative value if required size is not sufficient
 * */
DltReturnValue dlt_check_rcv_data_size(int received, int required);

/**
 * 初始化大小为size的静态ringbuffer。
*初始化为服务器。初始化计数器。
*内存已经分配。
 * @param buf Pointer to ringbuffer structure
 * @param ptr Ptr to ringbuffer memory
 * @param size Maximum size of buffer in bytes
 * @return negative value if there was an error
 */
DltReturnValue dlt_buffer_init_static_server(DltBuffer *buf, const unsigned char *ptr, uint32_t size);

/**
 * 初始化大小为size的静态ringbuffer。
*初始化为客户端。不要改变计数器。
*内存已经分配。
 * @param buf Pointer to ringbuffer structure
 * @param ptr Ptr to ringbuffer memory
 * @param size Maximum size of buffer in bytes
 * @return negative value if there was an error
 */
DltReturnValue dlt_buffer_init_static_client(DltBuffer *buf, const unsigned char *ptr, uint32_t size);

/**
 * 初始化大小为size的动态环形缓冲区。
*初始化为客户端。不要改变计数器。
*从min_size开始分配内存。
*如果需要更多的内存，可以使用step_size增加内存大小。
*最大大小为max_size。
 * @param buf Pointer to ringbuffer structure
 * @param min_size Minimum size of buffer in bytes
 * @param max_size Maximum size of buffer in bytes
 * @param step_size size of which ringbuffer is increased
 * @return negative value if there was an error
 */
DltReturnValue dlt_buffer_init_dynamic(DltBuffer *buf, uint32_t min_size, uint32_t max_size, uint32_t step_size);

/**
 * 取消初始化使用静态ringbuffer
 * @param buf Pointer to ringbuffer structure
 * @return negative value if there was an error
 */
DltReturnValue dlt_buffer_free_static(DltBuffer *buf);

/**
 * 释放和释放动态环缓冲区使用的内存
 * @param buf Pointer to ringbuffer structure
 * @return negative value if there was an error
 */
DltReturnValue dlt_buffer_free_dynamic(DltBuffer *buf);

/**
 * 检查消息是否适合缓冲区。
 * @param buf Pointer to buffer structure
 * @param needed Needed size
 * @return DLT_RETURN_OK if enough space, DLT_RETURN_ERROR otherwise
 */
DltReturnValue dlt_buffer_check_size(DltBuffer *buf, int needed);

/**
 * 向ringbuffer写入一个条目
 * @param buf Pointer to ringbuffer structure
 * @param data Pointer to data to be written to ringbuffer
 * @param size Size of data in bytes to be written to ringbuffer
 * @return negative value if there was an error
 */
DltReturnValue dlt_buffer_push(DltBuffer *buf, const unsigned char *data, unsigned int size);

/**
 * 最多写入三个条目到ringbuffer。
*条目被连接到一个块。
 * @param buf Pointer to ringbuffer structure
 * @param data1 Pointer to data to be written to ringbuffer
 * @param size1 Size of data in bytes to be written to ringbuffer
 * @param data2 Pointer to data to be written to ringbuffer
 * @param size2 Size of data in bytes to be written to ringbuffer
 * @param data3 Pointer to data to be written to ringbuffer
 * @param size3 Size of data in bytes to be written to ringbuffer
 * @return negative value if there was an error
 */
DltReturnValue dlt_buffer_push3(DltBuffer *buf,
                                const unsigned char *data1,
                                unsigned int size1,
                                const unsigned char *data2,
                                unsigned int size2,
                                const unsigned char *data3,
                                unsigned int size3);

/**
 * 从ringbuffer读取一个条目。
*并从ringbuffer中删除它.
 * @param buf Pointer to ringbuffer structure
 * @param data Pointer to data read from ringbuffer
 * @param max_size Max size of read data in bytes from ringbuffer
 * @return size of read data, zero if no data available, negative value if there was an error
 */
int dlt_buffer_pull(DltBuffer *buf, unsigned char *data, int max_size);

/**
 * 从ringbuffer读取一个条目。
*不要从ringbuffer中删除它.
 * @param buf Pointer to ringbuffer structure
 * @param data Pointer to data read from ringbuffer
 * @param max_size Max size of read data in bytes from ringbuffer
 * @return size of read data, zero if no data available, negative value if there was an error
 */
int dlt_buffer_copy(DltBuffer *buf, unsigned char *data, int max_size);

/**
 * 从ringbuffer中删除条目。
 * @param buf Pointer to ringbuffer structure
 * @return size of read data, zero if no data available, negative value if there was an error
 */
int dlt_buffer_remove(DltBuffer *buf);

/**
 * 将有关缓冲区和日志的信息打印到内部DLT日志。
 * @param buf Pointer to ringbuffer structure
 */
void dlt_buffer_info(DltBuffer *buf);

/**
 *将缓冲区和日志的状态打印到内部DLT日志。
 * @param buf Pointer to ringbuffer structure
 */
void dlt_buffer_status(DltBuffer *buf);

/**
 * 获取ringbuffer的总大小(以字节为单位)。
*如果buffer是动态的，则返回max size。
 * @param buf Pointer to ringbuffer structure
 * @return total size of buffer
 */
uint32_t dlt_buffer_get_total_size(DltBuffer *buf);

/**
 * 获取以字节为单位的环形缓冲区的使用大小.
 * @param buf Pointer to ringbuffer structure
 * @return used size of buffer
 */
int dlt_buffer_get_used_size(DltBuffer *buf);

/**
 * 获取ringbuffer中的条目数。
 * @param buf Pointer to ringbuffer structure
 * @return number of entries
 */
int dlt_buffer_get_message_count(DltBuffer *buf);

#   if !defined (__WIN32__)

/**
 * 辅助功能:设置串口连接
 * @param fd File descriptor of serial tty device
 * @param speed Serial line speed, as defined in termios.h
 * @return negative value if there was an error
 */
DltReturnValue dlt_setup_serial(int fd, speed_t speed);

/**
 * 辅助功能:将串行线波特率(如数字)转换为线速度(如terminos .h中定义)
 * @param baudrate Serial line baudrate (as number)
 * @return Serial line speed, as defined in termios.h
 */
speed_t dlt_convert_serial_speed(int baudrate);

/**
 * 打印dlt版本和dlt svn版本到buf
 * @param buf Pointer to buffer
 * @param size size of buffer
 */
void dlt_get_version(char *buf, size_t size);

/**
 * 打印dlt主版本到buf
 * @param buf Pointer to buffer
 * @param size size of buffer
 */
void dlt_get_major_version(char *buf, size_t size);

/**
 * 打印dlt子版本到buf
 * @param buf Pointer to buffer
 * @param size size of buffer
 */
void dlt_get_minor_version(char *buf, size_t size);

#   endif

/* 下列是应该只在内部使用的函数原型*/
/*                                                          */

/**
 * 初始化的常见部分。计算以下环境变量
*并将它们存储在dlt_user结构体中:
 * - DLT_DISABLE_EXTENDED_HEADER_FOR_NONVERBOSE
 * - DLT_LOCAL_PRINT_MODE (AUTOMATIC: 0, FORCE_ON: 2, FORCE_OFF: 3)
 * - DLT_INITIAL_LOG_LEVEL (e.g. APPx:CTXa:6;APPx:CTXb:5)
 * - DLT_FORCE_BLOCKING
 * - DLT_USER_BUFFER_MIN
 * - DLT_USER_BUFFER_MAX
 * - DLT_USER_BUFFER_STEP
 * - DLT_LOG_MSG_BUF_LEN
 * - DLT_DISABLE_INJECTION_MSG_AT_USER
 * @return negative value if there was an error
 */
DltReturnValue dlt_init_common(void);

/**
 * 以0.1 ms的分辨率返回系统的正常运行时间
 * @return 0 if there was an error
 */
uint32_t dlt_uptime(void);

/**
 * 打印DLT消息的标题
 * @param message pointer to structure of organising access to DLT messages
 * @param text pointer to a ASCII string, in which the header is written
 * @param size maximal size of text buffer
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_print_header(DltMessage *message, char *text, uint32_t size, int verbose);

/**
 * 将DLT消息的有效负载打印为十六进制输出
 * @param message pointer to structure of organising access to DLT messages
 * @param text pointer to a ASCII string, in which the output is written
 * @param size maximal size of text buffer
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_print_hex(DltMessage *message, char *text, uint32_t size, int verbose);

/**
 * 将DLT消息的有效负载打印为ASCII码输出
 * @param message pointer to structure of organising access to DLT messages
 * @param text pointer to a ASCII string, in which the output is written
 * @param size maximal size of text buffer
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_print_ascii(DltMessage *message, char *text, uint32_t size, int verbose);

/**
 * 将DLT消息的有效负载打印为混合输出(十六进制和ASCII)，用于纯文本输出
 * @param message pointer to structure of organising access to DLT messages
 * @param text pointer to a ASCII string, in which the output is written
 * @param size maximal size of text buffer
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_print_mixed_plain(DltMessage *message, char *text, uint32_t size, int verbose);

/**
 * 将DLT消息的有效负载打印为混合输出(十六进制和ASCII)，用于HTML文本输出
 * @param message pointer to structure of organising access to DLT messages
 * @param text pointer to a ASCII string, in which the output is written
 * @param size maximal size of text buffer
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_print_mixed_html(DltMessage *message, char *text, uint32_t size, int verbose);

/**
 * 解码并打印DLT消息的参数
 * @param msg pointer to structure of organising access to DLT messages
 * @param type_info Type of argument
 * @param ptr pointer to pointer to data (pointer to data is changed within this function)
 * @param datalength pointer to datalength (datalength is changed within this function)
 * @param text pointer to a ASCII string, in which the output is written
 * @param textlength maximal size of text buffer
 * @param byteLength If argument is a string, and this value is 0 or greater, this value will be taken as string length
 * @param verbose if set to true verbose information is printed out.
 * @return negative value if there was an error
 */
DltReturnValue dlt_message_argument_print(DltMessage *msg,
                                          uint32_t type_info,
                                          uint8_t **ptr,
                                          int32_t *datalength,
                                          char *text,
                                          size_t textlength,
                                          int byteLength,
                                          int verbose);

/**
 * 检查环境变量.
 */
void dlt_check_envvar(void);

/**
 * 解析响应文本并识别服务id及其选项.
 *
 * @param resp_text   char *
 * @param service_id  int *
 * @param service_opt int *
 * @return pointer to resp_text
 */
int dlt_set_loginfo_parse_service_id(char *resp_text, uint32_t *service_id, uint8_t *service_opt);

/**
 * 将获取的日志信息从ASCII转换为uint16
 *
 * @param rp        char
 * @param rp_count  int
 * @return length
 */
int16_t dlt_getloginfo_conv_ascii_to_uint16_t(char *rp, int *rp_count);

/**
 * 将获取的日志信息从ASCII转换为int16
 *
 * @param rp        char
 * @param rp_count  int
 * @return length
 */
int16_t dlt_getloginfo_conv_ascii_to_int16_t(char *rp, int *rp_count);

/**
 * 将获取的日志信息从ASCII转换为id
 *
 * @param rp        char
 * @param rp_count  int
 * @param wp        char
 * @param len       int
 */
void dlt_getloginfo_conv_ascii_to_id(char *rp, int *rp_count, char *wp, int len);

/**
 * 将获取的日志信息从ASCII转换为二进制
 * @param ptr    const char
 * @param binary uint8_t
 * @param size   int
 */
void dlt_hex_ascii_to_binary(const char *ptr, uint8_t *binary, int *size);

/**
 * 辅助函数，用于在新的子进程中执行execvp函数.
 * @param filename file path to store the stdout of command (NULL if not required)
 * @param command execution command followed by arguments with NULL-termination
 * @return negative value if there was an error
 */
int dlt_execute_command(char *filename, char *command, ...);

#   ifdef __cplusplus
}
#   endif

/**
 \}
 */

#endif /* DLT_COMMON_H */
