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
 * \file dlt_user.h
 */

/*******************************************************************************
**                                                                            **
**  SRC-MODULE: dlt_user.h                                                    **
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

#ifndef DLT_USER_H
#   define DLT_USER_H

/**
 * \defgroup userapi DLT User API
 * \addtogroup userapi
 \{
 */

#ifndef DLT_NETWORK_TRACE_ENABLE
#define DLT_NETWORK_TRACE_ENABLE
#endif

#define DLT_DISABLE_MACRO 0

#ifdef DLT_NETWORK_TRACE_ENABLE
#   include <mqueue.h>
#else
#    include <sys/types.h>
#    include <fcntl.h>
#endif

#   include <pthread.h>

#   if !defined (__WIN32__)
#      include <semaphore.h>
#   endif

#   include "dlt_types.h"
#   include "dlt_shm.h"
#if !DLT_DISABLE_MACRO
#   include "dlt_user_macros.h"
#endif

#   ifdef __cplusplus
extern "C" {
#   endif

#   define DLT_USER_BUF_MAX_SIZE 1390            /**< 每个用户缓冲区的最大大小，也用于注入缓冲区 */

#   define DLT_USER_RESENDBUF_MAX_SIZE (DLT_USER_BUF_MAX_SIZE + 100)    /**< 重发缓冲区的大小;DLT消息的最大大小是1390字节加上一些额外的标题空间  */

/* 使用来自操作系统的信号量或互斥锁来防止对DLT缓冲区的并发访问 */
#define DLT_SEM_LOCK() do{\
    while ((sem_wait(&dlt_mutex) == -1) && (errno == EINTR)) \
        continue;       /* 如果中断则重新启动 */ \
    } while(0)
#define DLT_SEM_FREE() { sem_post(&dlt_mutex); }

/**
 * 此结构用于应用程序中使用的每个上下文。
 */
typedef struct
{
    char contextID[DLT_ID_SIZE];                  /**< context id */
    int32_t log_level_pos;                        /**< offset in user-application context field */
    int8_t *log_level_ptr;                        /**< pointer to the log level */
    int8_t *trace_status_ptr;                     /**< pointer to the trace status */
    uint8_t mcnt;                                 /**< message counter */
} DltContext;

/**
 * 此结构用于应用程序中使用的上下文数据。
 */
typedef struct
{
    DltContext *handle;                           /**< pointer to DltContext */
    unsigned char *buffer;                        /**< buffer for building log message*/
    int32_t size;                                 /**< payload size */
    int32_t log_level;                            /**< log level */
    int32_t trace_status;                         /**< trace status */
    int32_t args_num;                             /**< number of arguments for extended header*/
    char *context_description;                    /**< description of context */
    DltTimestampType use_timestamp;               /**< whether to use user-supplied timestamps */
    uint32_t user_timestamp;                      /**< user-supplied timestamp to use */
    int8_t verbose_mode;                          /**< verbose mode: 1 enabled, 0 disabled */
} DltContextData;

typedef struct
{
    uint32_t service_id;
    int (*injection_callback)(uint32_t service_id, void *data, uint32_t length);
    int (*injection_callback_with_id)(uint32_t service_id, void *data, uint32_t length, void *priv_data);
    void *data;
} DltUserInjectionCallback;

typedef struct
{
    char contextID[DLT_ID_SIZE];      /**< Context ID */
    int8_t log_level;                 /**< Log level */
    int8_t trace_status;              /**< Trace status */
    void (*log_level_changed_callback)(char context_id[DLT_ID_SIZE], uint8_t log_level, uint8_t trace_status);
} DltUserLogLevelChangedCallback;

/**
 * 此结构用于管理应用程序中所有上下文和相应日志级别的表中
 */
typedef struct
{
    char contextID[DLT_ID_SIZE];      /**< Context ID */
    int8_t log_level;                 /**< Log level */
    int8_t *log_level_ptr;             /**< Ptr to the log level */
    int8_t trace_status;              /**< Trace status */
    int8_t *trace_status_ptr;             /**< Ptr to the trace status */
    char *context_description;        /**< description of context */
    DltUserInjectionCallback *injection_table; /**< Table with pointer to injection functions and service ids */
    uint32_t nrcallbacks;

    /* Log Level changed callback */
    void (*log_level_changed_callback)(char context_id[DLT_ID_SIZE], uint8_t log_level, uint8_t trace_status);

} dlt_ll_ts_type;

/**
 * @brief 保存给定appId:ctxId对的初始日志级别
 */
typedef struct
{
    char appId[DLT_ID_SIZE];
    char ctxId[DLT_ID_SIZE];
    int8_t ll;
} dlt_env_ll_item;


/**
 * @brief 保存通过环境变量DLT_INITIAL_LOG_LEVEL给出的所有初始日志级别
 */
typedef struct
{
    dlt_env_ll_item *item;
    size_t array_size;
    size_t num_elem;
} dlt_env_ll_set;


/**
 * 这个结构只在一个应用程序中使用一次
 */
typedef struct
{
    char ecuID[DLT_ID_SIZE];                   /**< ECU ID */
    char appID[DLT_ID_SIZE];                   /**< Application ID */
    int dlt_log_handle;                        /**< Handle to fifo of dlt daemon */
    int dlt_user_handle;                       /**< Handle to own fifo */
#ifdef DLT_NETWORK_TRACE_ENABLE
    mqd_t dlt_segmented_queue_read_handle;     /**< Handle message queue */
    mqd_t dlt_segmented_queue_write_handle;    /**< Handle message queue */
    pthread_t dlt_segmented_nwt_handle;        /**< thread handle of segmented sending */
#endif
    char path[DLT_MOUNT_PATH_MAX];
    int32_t  file_count;
    int64_t  file_size;
    struct DltOfflineFileList *file_list;
    struct DltOfflineFileList *dlt_file_handle;/**< Handle to file of dlt file */
    int8_t dlt_is_file;                        /**< Target of logging: 1 to file, 0 to daemon */
    int8_t dlt_is_daemon;                       /**< Target of logging: 1 to to daemon */
    unsigned int filesize_max;                 /**< Maximum size of existing file in case dlt_is_file=1 */

    dlt_ll_ts_type *dlt_ll_ts;                 /** [MAX_DLT_LL_TS_ENTRIES]; < Internal management struct for all
                                                *  contexts */
    uint32_t dlt_ll_ts_max_num_entries;        /**< Maximum number of contexts */

    uint32_t dlt_ll_ts_num_entries;            /**< Number of used contexts */

    int8_t overflow;                           /**< Overflow marker, set to 1 on overflow, 0 otherwise */
    uint32_t overflow_counter;                 /**< Counts the number of lost messages */

    char *application_description;             /**< description of application */

    DltReceiver receiver;                      /**< Receiver for internal user-defined messages from daemon */

    int8_t verbose_mode;                       /**< Verbose mode enabled: 1 enabled, 0 disabled */
    int8_t use_extended_header_for_non_verbose; /**< Use extended header for non verbose: 1 enabled, 0 disabled */
    int8_t with_session_id;                    /**< Send always session id: 1 enabled, 0 disabled */
    int8_t with_timestamp;                     /**< Send always timestamp: 1 enabled, 0 disabled */
    int8_t with_ecu_id;                        /**< Send always ecu id: 1 enabled, 0 disabled */

    int8_t enable_local_print;                 /**< Local printing of log messages: 1 enabled, 0 disabled */
    int8_t local_print_mode;                   /**< Local print mode, controlled by environment variable */
    int8_t disable_injection_msg;               /**< Injection msg availability: 1 disabled, 0 enabled (default) */

    int8_t log_state;                          /**< Log state of external connection:
                                                * 1 client connected,
                                                * 0 not connected,
                                                * -1 unknown */

    DltBuffer startup_buffer; /**< Ring-buffer for buffering messages during startup and missing connection */
    /* Buffer used for resending, locked by DLT semaphore */
    uint8_t *resend_buffer;

    uint32_t timeout_at_exit_handler; /**< timeout used in dlt_user_atexit_blow_out_user_buffer, in 0.1 milliseconds */
    dlt_env_ll_set initial_ll_set;

#   ifdef DLT_SHM_ENABLE
    DltShm dlt_shm;
#   endif
#   ifdef DLT_TEST_ENABLE
    int corrupt_user_header;
    int corrupt_message_size;
    int16_t corrupt_message_size_size;
#   endif
#   if defined DLT_LIB_USE_UNIX_SOCKET_IPC || defined DLT_LIB_USE_VSOCK_IPC
    DltUserConnectionState connection_state;
#   endif
    uint16_t log_buf_len;        /**< length of message buffer, by default: DLT_USER_BUF_MAX_SIZE */
} DltUser;

typedef int (*dlt_injection_callback_id)(uint32_t, void *, uint32_t, void *);
typedef int (*dlt_injection_callback)(uint32_t, void *, uint32_t);

/**************************************************************************************************
* 以下API函数为DLT定义了一个低级函数接口
**************************************************************************************************/

/**
 *初始化DLT日志消息的生成(用于详细模式)
当应用程序想要发送一个新的日志消息时，这个函数必须先被调用。
* 接下来当返回值大于零时,dlt_user_log_write_string和dlt_user_log_write_finish函数也必须被调用。
*
 * @param handle pointer to an object containing information about one special logging context
 * @param log pointer to an object containing information about logging context data
 * @param loglevel this is the current log level of the log message to be sent
 * @return Value from DltReturnValue enum, DLT_RETURN_TRUE if log level is matching
 */
DltReturnValue dlt_user_log_write_start(DltContext *handle, DltContextData *log, DltLogLevelType loglevel);

/**
 *初始化DLT日志消息的生成(用于详细模式)
当应用程序想要发送一个新的日志消息时，这个函数必须先被调用。
* 接下来当返回值大于零时,dlt_user_log_write_string和dlt_user_log_write_finish函数也必须被调用。
*
 * @param handle pointer to an object containing information about one special logging context
 * @param log pointer to an object containing information about logging context data
 * @param loglevel this is the current log level of the log message to be sent
 * @param messageid message id of message
 * @return Value from DltReturnValue enum, DLT_RETURN_TRUE if log level is matching
 */
DltReturnValue dlt_user_log_write_start_id(DltContext *handle,
                                           DltContextData *log,
                                           DltLogLevelType loglevel,
                                           uint32_t messageid);

/**
*初始化从DLT应用程序生成具有给定缓冲区的DLT日志消息。
*这可以考虑替换为dlt_user_log_write_start/dlt_user_log_write_start_id
*或者其他数据函数，如dlt_user_log_write_string。
*第四，第五，第六个参数应该由DLT应用程序准备;该函数只负责检查日志级别并将给定值设置为上下文数据。
当一个应用程序准备发送一个新的日志消息与给定的缓冲区，必须先调用这个函数。
这个函数只与dlt_user_log_write_finish_w_given_buffer组合工作，并且函数必须只有
当返回值大于零时才被调用*。该函数目前只支持详细模式。
 * @param handle pointer to an object containing information about one special logging context
 * @param log pointer to an object containing information about logging context data
 * @param loglevel this is the current log level of the log message to be sent
 * @param buffer data with log message
 * @param size buffer size
 * @param args_num number of arguments in buffer
 * @return Value from DltReturnValue enum, DLT_RETURN_TRUE if log level is matching
 */
DltReturnValue dlt_user_log_write_start_w_given_buffer(DltContext *handle,
                                                       DltContextData *log,
                                                       DltLogLevelType loglevel,
                                                       char *buffer,
                                                       size_t size,
                                                       int32_t args_num);

/**
 *完成DLT日志消息的生成并发送给DLT Daemon。
*此函数在写入日志消息的所有日志属性后必须被调用。
 * @param log pointer to an object containing information about logging context data
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_finish(DltContextData *log);

/**
*完成DLT日志消息的生成并将其发送到DLT守护进程
*释放日志缓冲区。此函数仅与 dlt_user_log_write_start_w_given_buffer组合工作。
*这个函数在写完日志信息的日志属性之后必须被调用
 * @param log pointer to an object containing information about logging context data
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_finish_w_given_buffer(DltContextData *log);

/**
*写一个布尔参数到DLT日志消息。
*dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param data boolean parameter written into log message (mapped to uint8)
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_bool(DltContextData *log, uint8_t data);

/**
在DLT日志消息中写入一个带有name属性的布尔参数。
* dlt_user_log_write_start必须在向日志消息添加任何参数之前调用。
*通过调用dlt_user_log_write_finish完成日志消息的构建。
*
*如果@a name为NULL，该函数将添加一个长度为0的属性字段并且消息没有内容。
*
 * @param log  pointer to an object containing information about logging context data
 * @param data  boolean parameter written into log message (mapped to uint8)
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_bool_attr(DltContextData *log, uint8_t data, const char *name);

/**
*将float参数写入DLT日志消息。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param data float32_t parameter written into log message.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_float32(DltContextData *log, float32_t data);

/**
 *将double参数写入DLT日志消息。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param data float64_t parameter written into log message.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_float64(DltContextData *log, double data);

/**
*将带有属性的float参数写入DLT日志消息。
* dlt_user_log_write_start必须在向日志消息添加任何参数之前调用。
*通过调用dlt_user_log_write_finish完成日志消息的构建。
*如果@一个名称或@一个单位为NULL，该函数将添加一个长度为0的相应属性字段
*并且该属性的消息中没有内容。
*
 * @param log  pointer to an object containing information about logging context data
 * @param data  float32_t parameter written into log message
 * @param name  the "name" attribute (or NULL)
 * @param unit  the "unit" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_float32_attr(DltContextData *log, float32_t data, const char *name, const char *unit);

/**
在DLT日志消息中写入带有属性的double参数。
* dlt_user_log_write_start必须在向日志消息添加任何参数之前调用。
*通过调用dlt_user_log_write_finish完成日志消息的构建。
*如果@一个名称或@一个单位为NULL，该函数将添加一个长度为0的相应属性字段
*并且该属性的消息中没有内容。
 *
 * @param log  pointer to an object containing information about logging context data
 * @param data  float64_t parameter written into log message
 * @param name  the "name" attribute (or NULL)
 * @param unit  the "unit" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_float64_attr(DltContextData *log, float64_t data, const char *name, const char *unit);


/**New add
在DLT日志消息中写入带有属性的void*参数。
* dlt_user_log_write_start必须在向日志消息添加任何参数之前调用。
*通过调用dlt_user_log_write_finish完成日志消息的构建。
*如果@一个名称或@一个单位为NULL，该函数将添加一个长度为0的相应属性字段
*并且该属性的消息中没有内容。
 *
 * @param log  pointer to an object containing information about logging context data
 * @param data  float64_t parameter written into log message
 * @param datalength  date length
 * @param name  the "name" attribute (or NULL)
 * @param unit  the "unit" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_SpanByte_attr(DltContextData *log, void* data, size_t datalength, const char *name, const char *unit);

/**New add
在DLT日志消息中写入带有属性的void*参数。
* dlt_user_log_write_start必须在向日志消息添加任何参数之前调用。
*通过调用dlt_user_log_write_finish完成日志消息的构建。
*如果@一个名称或@一个单位为NULL，该函数将添加一个长度为0的相应属性字段
*并且该属性的消息中没有内容。
 *
 * @param log  pointer to an object containing information about logging context data
 * @param data  float64_t parameter written into log message
 * @param datalength  date length
 * @param name  the "name" attribute (or NULL)
 * @param unit  the "unit" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_StringView_attr(DltContextData *log, void* data, size_t datalength, const char *name, const char *unit);

/**
*将一个int参数写入DLT日志消息。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param data unsigned int parameter written into log message.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_uint(DltContextData *log, unsigned int data);
DltReturnValue dlt_user_log_write_uint8(DltContextData *log, uint8_t data);
DltReturnValue dlt_user_log_write_uint16(DltContextData *log, uint16_t data);
DltReturnValue dlt_user_log_write_uint32(DltContextData *log, uint32_t data);
DltReturnValue dlt_user_log_write_uint64(DltContextData *log, uint64_t data);

/**
写一个带属性的int参数到DLT日志消息中。
* dlt_user_log_write_start必须在向日志消息添加任何参数之前调用。
*通过调用dlt_user_log_write_finish完成日志消息的构建。
 *
 * If @a name or @a unit is NULL, this function will add a corresponding attribute field with length 0
 * and no content to the message for that attribute.
 *
 * @param log  pointer to an object containing information about logging context data
 * @param data  unsigned int parameter written into log message
 * @param name  the "name" attribute (or NULL)
 * @param unit  the "unit" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_uint_attr(DltContextData *log, unsigned int data, const char *name, const char *unit);
DltReturnValue dlt_user_log_write_uint8_attr(DltContextData *log, uint8_t data, const char *name, const char *unit);
DltReturnValue dlt_user_log_write_uint16_attr(DltContextData *log, uint16_t data, const char *name, const char *unit);
DltReturnValue dlt_user_log_write_uint32_attr(DltContextData *log, uint32_t data, const char *name, const char *unit);
DltReturnValue dlt_user_log_write_uint64_attr(DltContextData *log, uint64_t data, const char *name, const char *unit);

/**
*将一个int参数写入DLT日志消息。输出将按照参数类型进行格式化。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param data unsigned int parameter written into log message.
 * @param type The formatting type of the string output.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_uint8_formatted(DltContextData *log, uint8_t data, DltFormatType type);
DltReturnValue dlt_user_log_write_uint16_formatted(DltContextData *log, uint16_t data, DltFormatType type);
DltReturnValue dlt_user_log_write_uint32_formatted(DltContextData *log, uint32_t data, DltFormatType type);
DltReturnValue dlt_user_log_write_uint64_formatted(DltContextData *log, uint64_t data, DltFormatType type);

/**
 *写入一个自变的指针值结构。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param data void* parameter written into log message.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_ptr(DltContextData *log, void *data);

/**
*将int参数写入DLT日志消息。
*dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param data int parameter written into log message.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_int(DltContextData *log, int data);
DltReturnValue dlt_user_log_write_int8(DltContextData *log, int8_t data);
DltReturnValue dlt_user_log_write_int16(DltContextData *log, int16_t data);
DltReturnValue dlt_user_log_write_int32(DltContextData *log, int32_t data);
DltReturnValue dlt_user_log_write_int64(DltContextData *log, int64_t data);

/**
写一个带属性的int参数到DLT日志消息中。
*dlt_user_log_write_start必须在向日志消息添加任何参数之前调用。
*通过调用dlt_user_log_write_finish完成日志消息的构建。
*如果@一个名称或@一个单位为NULL，该函数将添加一个长度为0的相应属性字段
*并且该属性的消息中没有内容。
 *
 * @param log  pointer to an object containing information about logging context data
 * @param data  int parameter written into log message
 * @param name  the "name" attribute (or NULL)
 * @param unit  the "unit" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_int_attr(DltContextData *log, int data, const char *name, const char *unit);
DltReturnValue dlt_user_log_write_int8_attr(DltContextData *log, int8_t data, const char *name, const char *unit);
DltReturnValue dlt_user_log_write_int16_attr(DltContextData *log, int16_t data, const char *name, const char *unit);
DltReturnValue dlt_user_log_write_int32_attr(DltContextData *log, int32_t data, const char *name, const char *unit);
DltReturnValue dlt_user_log_write_int64_attr(DltContextData *log, int64_t data, const char *name, const char *unit);

/**
*写一个空结束的ASCII字符串到DLT日志消息。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param text pointer to the parameter written into log message containing null termination.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_string(DltContextData *log, const char *text);

/**
*将可能非空终止的ASCII字符串写入DLT日志消息。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param text pointer to the parameter written into log message
 * @param length length in bytes of @a text (without any termination character)
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_sized_string(DltContextData *log, const char *text, uint16_t length);

/**
*写入一个恒定的以空结尾的ASCII字符串到DLT日志消息。
*在非详细模式下，DLT参数根本不会被发送。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param text pointer to the parameter written into log message containing null termination.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_constant_string(DltContextData *log, const char *text);

/**
写一个常量，可能是非空结束的ASCII字符串到DLT日志消息。
*在非详细模式下，DLT参数根本不会被发送。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param text pointer to the parameter written into log message
 * @param length length in bytes of @a text (without any termination character)
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_sized_constant_string(DltContextData *log, const char *text, uint16_t length);

/**
*写一个空结束的UTF8字符串到DLT日志消息。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param text pointer to the parameter written into log message containing null termination.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_utf8_string(DltContextData *log, const char *text);

/**
*将可能非空终止的UTF8字符串写入DLT日志消息。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param text pointer to the parameter written into log message
 * @param length length in bytes of @a text (without any termination character)
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_sized_utf8_string(DltContextData *log, const char *text, uint16_t length);

/**
*将一个以null结尾的UTF8字符串写入DLT日志消息。
*在非详细模式下，DLT参数根本不会被发送。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param text pointer to the parameter written into log message containing null termination.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_constant_utf8_string(DltContextData *log, const char *text);

/**
 写一个常量，可能是非空结束的UTF8字符串到DLT日志消息。
*在非详细模式下，DLT参数根本不会被发送。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param text pointer to the parameter written into log message
 * @param length length in bytes of @a text (without any termination character)
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_sized_constant_utf8_string(DltContextData *log, const char *text, uint16_t length);

/**
在DLT日志信息中写入一个带有"name"属性的以空结尾的ASCII字符串。
* dlt_user_log_write_start必须在向日志消息添加任何参数之前调用。
*通过调用dlt_user_log_write_finish完成日志消息的构建。
*如果@a name为NULL，该函数将添加一个长度为0的属性字段并且消息没有内容。
 *
 * @param log  pointer to an object containing information about logging context data
 * @param text  pointer to the parameter written into log message containing null termination
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_string_attr(DltContextData *log, const char *text, const char *name);

/**
 * Write a potentially non-null-terminated ASCII string with "name" attribute into a DLT log message.
 * dlt_user_log_write_start has to be called before adding any parameters to the log message.
 * Finish building a log message by calling dlt_user_log_write_finish.
 *
 * If @a name is NULL, this function will add an attribute field with length 0
 * and no content to the message.
 *
 * @param log  pointer to an object containing information about logging context data
 * @param text  pointer to the parameter written into log message
 * @param length  length in bytes of @a text (without any termination character)
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_sized_string_attr(DltContextData *log, const char *text, uint16_t length, const char *name);

/**
 * Write a constant, null-terminated ASCII string with "name" attribute into a DLT log message.
 * In non-verbose mode, this parameter will not be sent at all.
 * dlt_user_log_write_start has to be called before adding any parameters to the log message.
 * Finish building a log message by calling dlt_user_log_write_finish.
 *
 * If @a name is NULL, this function will add an attribute field with length 0
 * and no content to the message.
 *
 * @param log  pointer to an object containing information about logging context data
 * @param text  pointer to the parameter written into log message containing null termination
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_constant_string_attr(DltContextData *log, const char *text, const char *name);

/**
 * Write a constant, potentially non-null-terminated ASCII string with "name" attribute into a DLT log message.
 * In non-verbose mode, this parameter will not be sent at all.
 * dlt_user_log_write_start has to be called before adding any parameters to the log message.
 * Finish building a log message by calling dlt_user_log_write_finish.
 *
 * If @a name is NULL, this function will add an attribute field with length 0
 * and no content to the message.
 *
 * @param log  pointer to an object containing information about logging context data
 * @param text  pointer to the parameter written into log message
 * @param length  length in bytes of @a text (without any termination character)
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_sized_constant_string_attr(DltContextData *log, const char *text, uint16_t length, const char *name);

/**
 * Write a null-terminated UTF-8 string with "name" attribute into a DLT log message.
 * dlt_user_log_write_start has to be called before adding any parameters to the log message.
 * Finish building a log message by calling dlt_user_log_write_finish.
 *
 * If @a name is NULL, this function will add an attribute field with length 0
 * and no content to the message.
 *
 * @param log  pointer to an object containing information about logging context data
 * @param text  pointer to the parameter written into log message containing null termination
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_utf8_string_attr(DltContextData *log, const char *text, const char *name);

/**
 * Write a potentially non-null-terminated UTF-8 string with "name" attribute into a DLT log message.
 * dlt_user_log_write_start has to be called before adding any parameters to the log message.
 * Finish building a log message by calling dlt_user_log_write_finish.
 *
 * If @a name is NULL, this function will add an attribute field with length 0
 * and no content to the message.
 *
 * @param log  pointer to an object containing information about logging context data
 * @param text  pointer to the parameter written into log message
 * @param length  length in bytes of @a text (without any termination character)
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_sized_utf8_string_attr(DltContextData *log, const char *text, uint16_t length, const char *name);

/**
 * Write a constant, null-terminated UTF8 string with "name" attribute into a DLT log message.
 * In non-verbose mode, this parameter will not be sent at all.
 * dlt_user_log_write_start has to be called before adding any parameters to the log message.
 * Finish building a log message by calling dlt_user_log_write_finish.
 *
 * If @a name is NULL, this function will add an attribute field with length 0
 * and no content to the message.
 *
 * @param log  pointer to an object containing information about logging context data
 * @param text  pointer to the parameter written into log message containing null termination
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_constant_utf8_string_attr(DltContextData *log, const char *text, const char *name);

/**
 * Write a constant, potentially non-null-terminated UTF8 string with "name" attribute into a DLT log message.
 * In non-verbose mode, this parameter will not be sent at all.
 * dlt_user_log_write_start has to be called before adding any parameters to the log message.
 * Finish building a log message by calling dlt_user_log_write_finish.
 *
 * If @a name is NULL, this function will add an attribute field with length 0
 * and no content to the message.
 *
 * @param log  pointer to an object containing information about logging context data
 * @param text  pointer to the parameter written into log message
 * @param length  length in bytes of @a text (without any termination character)
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_sized_constant_utf8_string_attr(DltContextData *log, const char *text, uint16_t length, const char *name);

/**
 * Write a binary memory block into a DLT log message.
 * dlt_user_log_write_start has to be called before adding any attributes to the log message.
 * Finish sending log message by calling dlt_user_log_write_finish.
 * @param log pointer to an object containing information about logging context data
 * @param data pointer to the parameter written into log message.
 * @param length length in bytes of the parameter written into log message.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_raw(DltContextData *log, void *data, uint16_t length);

/**
 *将二进制内存块写入DLT日志消息。
* dlt_user_log_write_start必须在向日志消息添加任何属性之前调用。
*通过调用dlt_user_log_write_finish完成发送日志消息。
 * @param log pointer to an object containing information about logging context data
 * @param data pointer to the parameter written into log message.
 * @param length length in bytes of the parameter written into log message.
 * @param type the format information.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_raw_formatted(DltContextData *log, void *data, uint16_t length, DltFormatType type);

/**
*将带有"name"属性的二进制内存块写入DLT日志消息。
* dlt_user_log_write_start必须在向日志消息添加任何参数之前调用。
*通过调用dlt_user_log_write_finish完成日志消息的构建。
*如果@a name为NULL，该函数将添加一个长度为0的属性字段并且消息没有内容。
 *
 * @param log  pointer to an object containing information about logging context data
 * @param data  pointer to the parameter written into log message.
 * @param length  length in bytes of the parameter written into log message
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_raw_attr(DltContextData *log, const void *data, uint16_t length, const char *name);

/**
 * Write a binary memory block with "name" attribute into a DLT log message.
 * dlt_user_log_write_start has to be called before adding any parameters to the log message.
 * Finish building a log message by calling dlt_user_log_write_finish.
 *
 * If @a name is NULL, this function will add an attribute field with length 0
 * and no content to the message.
 *
 * @param log  pointer to an object containing information about logging context data
 * @param data  pointer to the parameter written into log message.
 * @param length  length in bytes of the parameter written into log message
 * @param type  the format information
 * @param name  the "name" attribute (or NULL)
 * @return value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_write_raw_formatted_attr(DltContextData *log, const void *data, uint16_t length, DltFormatType type, const char *name);

/**
 * 跟踪网络消息
 * @param handle 指向一个对象的指针，该对象包含关于一个特殊日志上下文的信息
 * @param nw_trace_type 网络跟踪类型(DLT_NW_TRACE_IPC、DLT_NW_TRACE_CAN、DLT_NW_TRACE_FLEXRAY或DLT_NW_TRACE_MOST)
 * @param header_len 网络消息头的长度
 * @param header 指向网络消息头的指针
 * @param payload_len 网络消息有效负载的长度
 * @param payload 指向网络消息负载的指针
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_trace_network(DltContext *handle,
                                      DltNetworkTraceType nw_trace_type,
                                      uint16_t header_len,
                                      void *header,
                                      uint16_t payload_len,
                                      void *payload);

/**
 * 跟踪网络消息，必要时截断。
 * @param handle pointer to an object containing information about logging context
 * @param nw_trace_type type of network trace (DLT_NW_TRACE_IPC, DLT_NW_TRACE_CAN, DLT_NW_TRACE_FLEXRAY, or DLT_NW_TRACE_MOST)
 * @param header_len length of network message header
 * @param header pointer to network message header
 * @param payload_len length of network message payload
 * @param payload pointer to network message payload
 * @param allow_truncate 设置为> 0,如果它太大,允许截断信息s.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_trace_network_truncated(DltContext *handle,
                                                DltNetworkTraceType nw_trace_type,
                                                uint16_t header_len,
                                                void *header,
                                                uint16_t payload_len,
                                                void *payload,
                                                int allow_truncate);

/**
*在分段异步模式下跟踪网络消息。
数据的发送是在一个单独的线程中完成的。
请注意句柄必须在应用程序的生命周期内存在，
因为数据块在不确定的未来时间异步发送。
 * @param handle pointer to an object containing information about logging context
 * @param nw_trace_type type of network trace (DLT_NW_TRACE_IPC, DLT_NW_TRACE_CAN, DLT_NW_TRACE_FLEXRAY, or DLT_NW_TRACE_MOST)
 * @param header_len length of network message header
 * @param header pointer to network message header
 * @param payload_len length of network message payload
 * @param payload pointer to network message payload
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_trace_network_segmented(DltContext *handle,
                                                DltNetworkTraceType nw_trace_type,
                                                uint16_t header_len,
                                                void *header,
                                                uint16_t payload_len,
                                                void *payload);

/**************************************************************************************************
* 以下API函数定义了DLT的高级函数接口
**************************************************************************************************/

/**
*初始化user lib用于和daemon通信。
*在使用任何DLT用户库函数之前，必须首先调用此函数。
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_init();

/**
 *初始化usr lib 只用于写文件。
*在使用任何DLT用户库函数之前，必须首先调用此函数。
 * @param name name of an optional log file
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_init_file(const char *name);
DltReturnValue dlt_init_file_dir(const char *path, int32_t file_count, int64_t file_size);
/**
 * 如果lib配置为只写文件，则设置最大文件大小。
 * 这个函数必须在dlt_init_file()之后调用。
 * @param filesize maximum file size
 * @return Value from DltReturnValue enum
 */

DltReturnValue dlt_set_filesize_max(unsigned int filesize);

/**
 *  enable daemon
  */
void dlt_enable_daemon(void);

/**
 *  enable daemon
  */
void dlt_disable_daemon(void);

/**
 * 终止用户库。
 * 这个函数必须在完成使用DLT用户库时调用。
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_free();

/**
 * 检查DLT库版本。
 * @param user_major_version the major version to be compared
 * @param user_minor_version the minor version to be compared
 * @return Value from DltReturnValue enum, DLT_RETURN_ERROR if there is a mismatch
 */
DltReturnValue dlt_check_library_version(const char *user_major_version, const char *user_minor_version);

/**
 * Register an application in the daemon.
 * @param apid four byte long character array with the application id
 * @param description long name of the application
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_register_app(const char *apid, const char *description);

/**
 * Unregister an application in the daemon.
 * This function has to be called when finishing using an application.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_unregister_app(void);

/**
*在守护进程中注销应用程序，同时刷新缓冲日志。
*此函数必须在完成使用应用程序时调用。
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_unregister_app_flush_buffered_logs(void);

/**
 * Get the application id
 * @param four byte long character array to store the application id
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_get_appid(char *appid);

/**
 * Register a context in the daemon.
 * This function has to be called before first usage of the context.
 * @param handle pointer to an object containing information about one special logging context
 * @param contextid four byte long character array with the context id
 * @param description long name of the context
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_register_context(DltContext *handle, const char *contextid, const char *description);

/**
 *在守护进程中注册一个具有预定义日志级别和预定义跟踪状态的上下文。
这个函数必须在第一次使用上下文之前被调用。
 * @param handle pointer to an object containing information about one special logging context
 * @param contextid four byte long character array with the context id
 * @param description long name of the context
 * @param loglevel This is the log level to be pre-set for this context
 *        (DLT_LOG_DEFAULT is not allowed here)
 * @param tracestatus This is the trace status to be pre-set for this context
 *        (DLT_TRACE_STATUS_DEFAULT is not allowed here)
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_register_context_ll_ts(DltContext *handle,
                                          const char *contextid,
                                          const char *description,
                                          int loglevel,
                                          int tracestatus);

/**
*在守护进程中注册一个日志级别改变回调函数的上下文。
*此功能是为了避免注册时遗漏日志级别和跟踪状态可以更改回调
 * @param handle pointer to an object containing information about one special logging context
 * @param contextid four byte long character array with the context id
 * @param description long name of the context
 * @param *dlt_log_level_changed_callback This is the fn which will be called when log level is changed
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_register_context_llccb(DltContext *handle,
                                          const char *contextid,
                                          const char *description,
                                          void (*dlt_log_level_changed_callback)(char context_id[DLT_ID_SIZE],
                                                                                 uint8_t log_level,
                                                                                 uint8_t trace_status));

/**
 * 在DLT守护进程中注销上下文。
 * 这个函数必须在结束使用上下文时调用。
 * @param handle pointer to an object containing information about one special logging context
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_unregister_context(DltContext *handle);


/**
 * 设置退出时重新发送的最大超时时间
 * @param timeout_in_milliseconds maximum time to wait until giving up re-sending, default 10000 (equals to 10 seconds)
 */
int dlt_set_resend_timeout_atexit(uint32_t timeout_in_milliseconds);

/**
 * 设置守护进程使用的日志模式。
 * 日志模式由守护进程持久化存储。
 * @see DltUserLogMode
 * @param mode the new logging mode used by the daemon: off, extern, internal, both.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_set_log_mode(DltUserLogMode mode);

/**
 * 获取连接到dlt-daemon的客户端的状态。

 * 当客户端连接或断开时，用户应用程序获得一条消息。
 * 该值包含最后一个状态。
 * 需要一些时间，直到应用程序从守护进程获取状态。
 * 在此之前，状态为“未知状态”。
 * @return -1 = unknown state, 0 = client not connected, 1 = client connected
 */
int dlt_get_log_state();

/**
 *接收到注入消息时调用注册回调函数
 * @param handle pointer to an object containing information about one special logging context
 * @param service_id the service id to be waited for
 * @param (*dlt_injection_callback) function pointer to callback function
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_register_injection_callback(DltContext *handle, uint32_t service_id,
                                               int (*dlt_injection_callback)(uint32_t service_id,
                                                                             void *data,
                                                                             uint32_t length));

/**
 *注册回调函数，当接收到注入消息时调用私有数据
 * @param handle pointer to an object containing information about one special logging context
 * @param service_id the service id to be waited for
 * @param (*dlt_injection_callback) function pointer to callback function
 * @param priv private data
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_register_injection_callback_with_id(DltContext *handle, uint32_t service_id,
                                                       int (*dlt_injection_callback)(uint32_t service_id,
                                                                                     void *data,
                                                                                     uint32_t length,
                                                                                     void *priv_data), void *priv);

/**
 * 更改上下文的日志级别时调用的注册回调函数
 * @param handle pointer to an object containing information about one special logging context
 * @param (*dlt_log_level_changed_callback) function pointer to callback function
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_register_log_level_changed_callback(DltContext *handle,
                                                       void (*dlt_log_level_changed_callback)(
                                                           char context_id[DLT_ID_SIZE],
                                                           uint8_t log_level,
                                                           uint8_t trace_status));

/**
 * 切换到详细模式
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_verbose_mode(void);

/**
 * 检查dlt库的版本和应用程序使用的库版本。
 * @param user_major_version version number of application - see dlt_version.h
 * @param user_minor_version version number of application - see dlt_version.h
 *  @return DltReturnValue enum中的值，如果不匹配，则为DLT_RETURN_ERROR
 */
DltReturnValue dlt_user_check_library_version(const char *user_major_version, const char *user_minor_version);

/**
 * 切换到非详细模式
 *
 * 这并不强制将所有消息作为非冗长消息发送，因为这没有多大意义。
相反，它允许发送详细和非详细消息，具体取决于哪个api正在被调用。
 */
DltReturnValue dlt_nonverbose_mode(void);

/**
 * 在非详细模式下使用扩展头.
 * 默认启用.
 * @param use_extended_header_for_non_verbose Use extended header for non verbose mode if true
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_use_extended_header_for_non_verbose(int8_t use_extended_header_for_non_verbose);

/**
 * 发送会话id配置.
 * 默认启用.
 * @param with_session_id Send session id in each message if enabled
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_with_session_id(int8_t with_session_id);

/**
 * 发送时间戳配置.
 * 默认启用.
 * @param with_timestamp Send timestamp id in each message if enabled
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_with_timestamp(int8_t with_timestamp);

/**
 * 发送ECU id配置.
 * 默认启用.
 * @param with_ecu_id Send ecu id in each message if enabled
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_with_ecu_id(int8_t with_ecu_id);

/**
 * 设置应用程序的最大日志级别和跟踪状态
 *
 * @param loglevel This is the log level to be set for the whole application
 * @param tracestatus This is the trace status to be set for the whole application
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_set_application_ll_ts_limit(DltLogLevelType loglevel, DltTraceStatusType tracestatus);


/**
* @brief根据环境给出的值调整日志级别

*遍历项目集合，找到最佳匹配。
*对于任何匹配的项，选择具有最高优先级的项，然后
返回* log-level。
*
*优先次序确定如下:
* -没有快速，没有快速，只有在item中给出的所有:与prio 1一起使用
* -没有快速，ctid匹配:使用1与prio 2
* -无ctid，快速匹配:使用l与prio 3
* -快速，ctid匹配:使用1与prio 4
 *
 * @param ll_set
 * @param apid
 * @param ctid
 * @param ll
 * If no item matches or in case of error, the original log-level (\param ll) is returned
 */
int dlt_env_adjust_ll_from_env(dlt_env_ll_set const *const ll_set,
                               char const *const apid,
                               char const *const ctid,
                               int const ll);

/**
 @brief从给定字符串中提取日志级别设置
*
*扫描参数 env的设置，如apid:ctid:log-level并存储它们
*在给定的参数ll_set
*
 * @param env 引用要解析的字符串，解析后env将指向最后一个解析字符之后
 * @param ll_set 从给定字符串中提取的日志级集合
 *
 * @return 0 on success
 * @return -1 on failure
 */
int dlt_env_extract_ll_set(char **const env, dlt_env_ll_set *const ll_set);

void dlt_env_free_ll_set(dlt_env_ll_set *const ll_set);

/**
 * 启用本地打印消息
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_enable_local_print(void);

/**
 * 关闭本地打印消息
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_disable_local_print(void);

/**
 *将以空结尾的ASCII字符串写入DLT日志消息。
 * @param handle pointer to an object containing information about one special logging context
 * @param loglevel this is the current log level of the log message to be sent
 * @param text pointer to the ASCII string written into log message containing null termination.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_log_string(DltContext *handle, DltLogLevelType loglevel, const char *text);

/**
 * 将以空结尾的ASCII字符串和整数值写入DLT日志消息。
 * @param handle pointer to an object containing information about one special logging context
 * @param loglevel this is the current log level of the log message to be sent
 * @param text pointer to the ASCII string written into log message containing null termination.
 * @param data integer value written into the log message
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_log_string_int(DltContext *handle, DltLogLevelType loglevel, const char *text, int data);

/**
 * Write a null terminated ASCII string and an unsigned integer value into a DLT log message.
 * @param handle pointer to an object containing information about one special logging context
 * @param loglevel this is the current log level of the log message to be sent
 * @param text pointer to the ASCII string written into log message containing null termination.
 * @param data unsigned integer value written into the log message
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_log_string_uint(DltContext *handle, DltLogLevelType loglevel, const char *text, unsigned int data);

/**
 * Write an integer value into a DLT log message.
 * @param handle pointer to an object containing information about one special logging context
 * @param loglevel this is the current log level of the log message to be sent
 * @param data integer value written into the log message
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_log_int(DltContext *handle, DltLogLevelType loglevel, int data);

/**
 * Write an unsigned integer value into a DLT log message.
 * @param handle pointer to an object containing information about one special logging context
 * @param loglevel this is the current log level of the log message to be sent
 * @param data unsigned integer value written into the log message
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_log_uint(DltContext *handle, DltLogLevelType loglevel, unsigned int data);

/**
 * Write an unsigned integer value into a DLT log message.
 * @param handle pointer to an object containing information about one special logging context
 * @param loglevel this is the current log level of the log message to be sent
 * @param data pointer to the parameter written into log message.
 * @param length length in bytes of the parameter written into log message.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_log_raw(DltContext *handle, DltLogLevelType loglevel, void *data, uint16_t length);

/**
 * 将标记消息写入DLT.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_log_marker();

/**
 获取守护进程和应用程序之间共享内存缓冲区的总大小和可用大小。
*此信息用于控制应用程序和守护进程之间的流控制。
例如，只有50%的缓冲区应该用于文件传输。
 * @param total_size total size of buffer in bytes
 * @param used_size used size of buffer in bytes
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_check_buffer(int *total_size, int *used_size);

/**
 *尝试在用户缓冲区中重新发送日志消息。如果dlt_uptime大于
* dlt_uptime() + DLT_USER_ATEXIT_RESEND_BUFFER_EXIT_TIMEOUT。重发之间的暂停
尝试次数可以用DLT_USER_ATEXIT_RESEND_BUFFER_SLEEP来定义
 * @return number of messages in the user buffer
 */
int dlt_user_atexit_blow_out_user_buffer(void);

/**
 * Try to resend log message in the user buffer.
 * @return Value from DltReturnValue enum
 */
DltReturnValue dlt_user_log_resend_buffer(void);

/**
*检查日志功能传递的日志级别，是否为该上下文启用。
这个函数可以被应用程序在生成日志之前调用。
*在写入新的日志消息之前也会调用。
 * @param handle pointer to an object containing information about one special logging context
 * @param loglevel this is the current log level of the log message to be sent
 * @return Value from DltReturnValue enum, DLT_RETURN_TRUE if log level is enabled
 */
static inline DltReturnValue dlt_user_is_logLevel_enabled(DltContext *handle, DltLogLevelType loglevel)
{
    if ((loglevel < DLT_LOG_DEFAULT) || (loglevel >= DLT_LOG_MAX))
        return DLT_RETURN_WRONG_PARAMETER;

    if ((handle == NULL) || (handle->log_level_ptr == NULL))
        return DLT_RETURN_WRONG_PARAMETER;

    if ((loglevel <= (DltLogLevelType)(*(handle->log_level_ptr))) && (loglevel != DLT_LOG_OFF))
        return DLT_RETURN_TRUE;

    return DLT_RETURN_LOGGING_DISABLED;
}

#   ifdef DLT_TEST_ENABLE
void dlt_user_test_corrupt_user_header(int enable);
void dlt_user_test_corrupt_message_size(int enable, int16_t size);
#   endif /* DLT_TEST_ENABLE */

#   ifdef __cplusplus
}
#   endif

/**
 \}
 */

#endif /* DLT_USER_H */