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
 * \file dlt_user_macros.h
 */

/*******************************************************************************
**                                                                            **
**  SRC-MODULE: dlt_user_macros.h                                             **
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
 * $LastChangedRevision: 1515 $
 * $LastChangedDate: 2010-12-13 09:18:54 +0100 (Mon, 13 Dec 2010) $
 * $LastChangedBy$
 * Initials    Date         Comment
 * aw          13.01.2010   initial
 */

#ifndef DLT_USER_MACROS_H
#define DLT_USER_MACROS_H

#include "dlt_version.h"
#include "dlt_types.h"

/**
 * \defgroup userapi DLT User API
 * \addtogroup userapi
 \{
 */

/**************************************************************************************************
* The folowing macros define a macro interface for DLT
**************************************************************************************************/

/**
 * 为新上下文创建对象。
*这个宏每次必须首先调用。
 * @param CONTEXT object containing information about one special logging context
 * @note 为了避免MISRA警告“Null语句位于其他代码或注释附近”
*使用宏时去掉分号。
*示例:DLT_DECLARE_CONTEXT(hContext)
 */
#define DLT_DECLARE_CONTEXT(CONTEXT) \
    DltContext CONTEXT;

/**
 *使用在其他模块中创建的新上下文的对象。
*这个宏必须首先调用。
 * @param CONTEXT object containing information about one special logging context
 * @note 为了避免MISRA警告“Null语句位于其他代码或注释附近”
*使用宏时去掉分号。
*示例:DLT_IMPORT_CONTEXT(hContext)
 */
#define DLT_IMPORT_CONTEXT(CONTEXT) \
    extern DltContext CONTEXT;

/**
 * 注册应用程序
 * @param APPID application id with maximal four characters
 * @param DESCRIPTION ASCII string containing description
 */
#define DLT_REGISTER_APP(APPID, DESCRIPTION) do { \
        (void)dlt_check_library_version(_DLT_PACKAGE_MAJOR_VERSION, _DLT_PACKAGE_MINOR_VERSION); \
        (void)dlt_register_app(APPID, DESCRIPTION); } while (0)


/**
 * 注销应用程序.
 */
#define DLT_UNREGISTER_APP() do { \
        (void)dlt_unregister_app(); } while (0)

/**
 * 注销应用程序并刷新在启动缓冲区中缓冲的日志(如果有的话)。
 */
#define DLT_UNREGISTER_APP_FLUSH_BUFFERED_LOGS() do { \
        (void)dlt_unregister_app_flush_buffered_logs(); } while (0)

/**
 * 获取应用程序ID。
 * @Param APPID character pointer of minimum 4 bytes
 */
#define DLT_GET_APPID(APPID) do{\
    dlt_get_appid(APPID);} while(0)

/**
 * 注册上下文(具有默认日志级别和默认跟踪状态)
 * @param CONTEXT object containing information about one special logging context
 * @param CONTEXTID context id with maximal four characters
 * @param DESCRIPTION ASCII string containing description
 */
#define DLT_REGISTER_CONTEXT(CONTEXT, CONTEXTID, DESCRIPTION) do { \
        (void)dlt_register_context(&(CONTEXT), CONTEXTID, DESCRIPTION); } while (0)

/**
 * 使用预定义的日志级别和预定义的跟踪状态注册上下文。
 * @param CONTEXT object containing information about one special logging context
 * @param CONTEXTID context id with maximal four characters
 * @param DESCRIPTION ASCII string containing description
 * @param LOGLEVEL log level to be pre-set for this context
 * (DLT_LOG_DEFAULT is not allowed here)
 * @param TRACESTATUS trace status to be pre-set for this context
 * (DLT_TRACE_STATUS_DEFAULT is not allowed here)
 */
#define DLT_REGISTER_CONTEXT_LL_TS(CONTEXT, CONTEXTID, DESCRIPTION, LOGLEVEL, TRACESTATUS) do { \
        (void)dlt_register_context_ll_ts(&(CONTEXT), CONTEXTID, DESCRIPTION, LOGLEVEL, TRACESTATUS); } while (0)

/**
 * 注册上下文(使用默认日志级别和默认跟踪状态以及日志级别更改回调)
 * @param CONTEXT object containing information about one special logging context
 * @param CONTEXTID context id with maximal four characters
 * @param DESCRIPTION ASCII string containing description
 * @param CBK log level change callback to be registered
 */
#define DLT_REGISTER_CONTEXT_LLCCB(CONTEXT, CONTEXTID, DESCRIPTION, CBK) do { \
        (void)dlt_register_context_llccb(&(CONTEXT), CONTEXTID, DESCRIPTION, CBK); } while (0)

/**
 * 注销上下文
 * @param CONTEXT object containing information about one special logging context
 */
#define DLT_UNREGISTER_CONTEXT(CONTEXT) do { \
        (void)dlt_unregister_context(&(CONTEXT)); } while (0)

/**
 * 收到注入消息时调用注册回调函数
 * @param CONTEXT object containing information about one special logging context
 * @param SERVICEID service id of the injection message
 * @param CALLBACK function pointer to callback function
 */
#define DLT_REGISTER_INJECTION_CALLBACK(CONTEXT, SERVICEID, CALLBACK) do { \
        (void)dlt_register_injection_callback(&(CONTEXT), SERVICEID, CALLBACK); } while (0)

/**
 * 收到注入消息时调用注册回调函数（带id）
 * @param CONTEXT object containing information about one special logging context
 * @param SERVICEID service id of the injection message
 * @param CALLBACK function pointer to callback function
 * @param PRIV_DATA data specific to context
 */
#define DLT_REGISTER_INJECTION_CALLBACK_WITH_ID(CONTEXT, SERVICEID, CALLBACK, PRIV_DATA) do { \
        (void)dlt_register_injection_callback_with_id(&(CONTEXT), SERVICEID, CALLBACK, PRIV_DATA); } while (0)

/**
 * 更改上下文的日志级别时调用的注册回调函数
 * @param CONTEXT object containing information about one special logging context
 * @param CALLBACK function pointer to callback function
 */
#define DLT_REGISTER_LOG_LEVEL_CHANGED_CALLBACK(CONTEXT, CALLBACK) do { \
        (void)dlt_register_log_level_changed_callback(&(CONTEXT), CALLBACK); } while (0)

/**
 * 发送带有可变消息列表的日志消息(用于详细模式)
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log message
 * @param ... variable list of arguments
 * @note 为了避免MISRA警告“逗号操作符已在for语句外使用”
*使用分号而不是逗号来分隔__VA_ARGS__。
*示例:DLT_LOG_ID_TS(hContext, DLT_LOG_INFO, 0x1234, timestamp, DLT_STRING("Hello world");DLT_INT (123));
 */
#ifdef _MSC_VER
/* DLT_LOG is not supported by MS Visual C++ */
/* use function interface instead            */
#else
#   define DLT_LOG(CONTEXT, LOGLEVEL, ...) \
    do { \
        DltContextData log_local; \
        int dlt_local; \
        dlt_local = dlt_user_log_write_start(&CONTEXT, &log_local, LOGLEVEL); \
        if (dlt_local == DLT_RETURN_TRUE) \
        { \
            __VA_ARGS__; \
            (void)dlt_user_log_write_finish(&log_local); \
        } \
    } while (0)
#endif

/**
 * 发送带有可变消息列表的日志消息(用于详细模式带时间戳)
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log message
 * @param TS timestamp to be used for log message
 * @param ... variable list of arguments
 * @note 为了避免MISRA警告“逗号操作符已在for语句外使用”
*使用分号而不是逗号来分隔__VA_ARGS__。
*示例:DLT_LOG_ID_TS(hContext, DLT_LOG_INFO, 0x1234, timestamp, DLT_STRING("Hello world");DLT_INT (123));
 */
#ifdef _MSC_VER
/* DLT_LOG_TS is not supported by MS Visual C++ */
/* use function interface instead            */
#else
#   define DLT_LOG_TS(CONTEXT, LOGLEVEL, TS, ...) \
    do { \
        DltContextData log_local; \
        int dlt_local; \
        dlt_local = dlt_user_log_write_start(&CONTEXT, &log_local, LOGLEVEL); \
        if (dlt_local == DLT_RETURN_TRUE) \
        { \
            __VA_ARGS__; \
            log_local.use_timestamp = DLT_USER_TIMESTAMP; \
            log_local.user_timestamp = (uint32_t) TS; \
            (void)dlt_user_log_write_finish(&log_local); \
        } \
    } while (0)
#endif

/**
 * 发送带有可变消息列表的日志消息(用于非详细模式)
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log message
 * @param MSGID the message id of log message
 * @param ... variable list of arguments
 * calls to DLT_STRING(), DLT_BOOL(), DLT_FLOAT32(), DLT_FLOAT64(),
 * DLT_INT(), DLT_UINT(), DLT_RAW()
 * @note 为了避免MISRA警告“逗号操作符已在for语句外使用”
*使用分号而不是逗号来分隔__VA_ARGS__。
*示例:DLT_LOG_ID_TS(hContext, DLT_LOG_INFO, 0x1234, timestamp, DLT_STRING("Hello world");DLT_INT (123));
 */
#ifdef _MSC_VER
/* DLT_LOG_ID is not supported by MS Visual C++ */
/* use function interface instead               */
#else
#   define DLT_LOG_ID(CONTEXT, LOGLEVEL, MSGID, ...) \
    do { \
        DltContextData log_local; \
        int dlt_local; \
        dlt_local = dlt_user_log_write_start_id(&CONTEXT, &log_local, LOGLEVEL, MSGID); \
        if (dlt_local == DLT_RETURN_TRUE) \
        { \
            __VA_ARGS__; \
            (void)dlt_user_log_write_finish(&log_local); \
        } \
    } while (0)
#endif

/**
 * 发送带有可变消息列表的日志消息(用于非详细模式带时间戳)
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log message
 * @param MSGID the message id of log message
 * @param TS timestamp to be used for log message
 * @param ... variable list of arguments
 * calls to DLT_STRING(), DLT_BOOL(), DLT_FLOAT32(), DLT_FLOAT64(),
 * DLT_INT(), DLT_UINT(), DLT_RAW()
 * @note 为了避免MISRA警告“逗号操作符已在for语句外使用”
*使用分号而不是逗号来分隔__VA_ARGS__。
*示例:DLT_LOG_ID_TS(hContext, DLT_LOG_INFO, 0x1234, timestamp, DLT_STRING("Hello world");DLT_INT (123));
 */
#ifdef _MSC_VER
/* DLT_LOG_ID_TS is not supported by MS Visual C++ */
/* use function interface instead               */
#else
#   define DLT_LOG_ID_TS(CONTEXT, LOGLEVEL, MSGID, TS, ...) \
    do { \
        DltContextData log_local; \
        int dlt_local; \
        dlt_local = dlt_user_log_write_start_id(&CONTEXT, &log_local, LOGLEVEL, MSGID); \
        if (dlt_local == DLT_RETURN_TRUE) \
        { \
            __VA_ARGS__; \
            log_local.use_timestamp = DLT_USER_TIMESTAMP; \
            log_local.user_timestamp = (uint32_t) TS; \
            (void)dlt_user_log_write_finish(&log_local); \
        } \
    } while (0)
#endif

/**
 * 在日志信息中添加字符串参数。
 * @param TEXT ASCII string
 */
#define DLT_STRING(TEXT) \
    (void)dlt_user_log_write_string(&log_local, TEXT)

/**
 * 将给定长度的字符串参数添加到日志消息中。
@a TEXT中的字符串不需要以空结尾，但是
复制的字符串将在其目的地以空结束
*在消息缓冲区中。
 * @param TEXT ASCII string
 * @param LEN length in bytes to take from @a TEXT
 */
#define DLT_SIZED_STRING(TEXT, LEN) \
    (void)dlt_user_log_write_sized_string(&log_local, TEXT, LEN)

/**
 * 在日志消息中添加常量字符串参数。
 * @param TEXT Constant ASCII string
 */
#define DLT_CSTRING(TEXT) \
    (void)dlt_user_log_write_constant_string(&log_local, TEXT)

/**
 * 将给定长度的常量字符串参数添加到日志消息中。
@a TEXT中的字符串不需要以空结尾，但是
复制的字符串将在其目的地以空结束
*在消息缓冲区中。
 * @param TEXT Constant ASCII string
 * @param LEN length in bytes to take from @a TEXT
 */
#define DLT_SIZED_CSTRING(TEXT, LEN) \
    (void)dlt_user_log_write_sized_constant_string(&log_local, TEXT, LEN)

/**
 * 在日志信息中增加utf8编码的字符串参数。
 * @param TEXT UTF8-encoded string
 */
#define DLT_UTF8(TEXT) \
    (void)dlt_user_log_write_utf8_string(&log_local, TEXT)

/**
 * 将具有给定长度的utf8编码字符串参数添加到日志消息中。
@a TEXT中的字符串不需要以空结尾，但是
复制的字符串将在其目的地以空结束
*在消息缓冲区中。
 * @param TEXT UTF8-encoded string
 * @param LEN length in bytes to take from @a TEXT
 */
#define DLT_SIZED_UTF8(TEXT, LEN) \
    (void)dlt_user_log_write_sized_utf8_string(&log_local, TEXT, LEN)

/**
 * 在日志消息中添加恒定的utf8编码字符串参数。
 * @param TEXT Constant UTF8-encoded string
 */
#define DLT_CUTF8(TEXT) \
    (void)dlt_user_log_write_constant_utf8_string(&log_local, TEXT)

/**
 * 向日志消息添加具有给定长度的utf8编码的常量字符串参数。
@a TEXT中的字符串不需要以空结尾，但是
复制的字符串将在其目的地以空结束
*在消息缓冲区中.
 * @param TEXT Constant UTF8-encoded string
 * @param LEN length in bytes to take from @a TEXT
 */
#define DLT_SIZED_CUTF8(TEXT, LEN) \
    (void)dlt_user_log_write_sized_constant_utf8_string(&log_local, TEXT, LEN)

/**
 * 在日志消息中添加带有“name”属性的字符串参数.
 * @param TEXT ASCII string
 * @param NAME "name" attribute
 */
#define DLT_STRING_ATTR(TEXT, NAME) \
    (void)dlt_user_log_write_string_attr(&log_local, TEXT, NAME)

/**
 * 向日志消息添加具有给定长度和“name”属性的字符串参数。
@a TEXT中的字符串不需要以空结尾，但是
复制的字符串将在其目的地以空结束
*在消息缓冲区中.
 * @param TEXT ASCII string
 * @param LEN length in bytes to take from @a TEXT
 * @param NAME "name" attribute
 */
#define DLT_SIZED_STRING_ATTR(TEXT, LEN, NAME) \
    (void)dlt_user_log_write_sized_string_attr(&log_local, TEXT, LEN, NAME)

/**
 * 在日志消息中添加带有“name”属性的常量字符串参数。
 * @param TEXT Constant ASCII string
 * @param NAME "name" attribute
 */
#define DLT_CSTRING_ATTR(TEXT, NAME) \
    (void)dlt_user_log_write_constant_string_attr(&log_local, TEXT, NAME)

/**
 *向日志消息添加具有给定长度和“name”属性的常量字符串参数。
@a TEXT中的字符串不需要以空结尾，但是
复制的字符串将在其目的地以空结束
*在消息缓冲区中.
 * @param TEXT Constant ASCII string
 * @param LEN length in bytes to take from @a TEXT
 * @param NAME "name" attribute
 */
#define DLT_SIZED_CSTRING_ATTR(TEXT, LEN, NAME) \
    (void)dlt_user_log_write_sized_constant_string_attr(&log_local, TEXT, LEN, NAME)

/**
 * 在日志消息中添加带有“name”属性的utf8编码字符串参数.
 * @param TEXT UTF8-encoded string
 * @param NAME "name" attribute
 */
#define DLT_UTF8_ATTR(TEXT, NAME) \
    (void)dlt_user_log_write_utf8_string_attr(&log_local, TEXT, NAME)

/**
 * 将具有给定长度和“name”属性的utf8编码字符串参数添加到日志消息中。
@a TEXT中的字符串不需要以空结尾，但是
复制的字符串将在其目的地以空结束
*在消息缓冲区中。
 * @param TEXT UTF8-encoded string
 * @param LEN length in bytes to take from @a TEXT
 * @param NAME "name" attribute
 */
#define DLT_SIZED_UTF8_ATTR(TEXT, LEN, NAME) \
    (void)dlt_user_log_write_sized_utf8_string_attr(&log_local, TEXT, LEN, ATTR)

/**
 * 将带有“name”属性的utf8编码字符串常量参数添加到日志消息中
 * @param TEXT Constant UTF8-encoded string
 * @param NAME "name" attribute
 */
#define DLT_CUTF8_ATTR(TEXT, NAME) \
    (void)dlt_user_log_write_constant_utf8_string_attr(&log_local, TEXT, NAME)

/**
 * 将具有给定长度和“name”属性的恒定utf8编码字符串参数添加到日志消息中。
@a TEXT中的字符串不需要以空结尾，但是
复制的字符串将在其目的地以空结束
*在消息缓冲区中.
 * @param TEXT Constant UTF8-encoded string
 * @param LEN length in bytes to take from @a TEXT
 * @param NAME "name" attribute
 */
#define DLT_SIZED_CUTF8_ATTR(TEXT, LEN, NAME) \
    (void)dlt_user_log_write_sized_constant_utf8_string_attr(&log_local, TEXT, LEN, NAME)

/**
 * 在日志消息中添加布尔参数。
 * @param BOOL_VAR Boolean value (mapped to uint8)
 */
#define DLT_BOOL(BOOL_VAR) \
    (void)dlt_user_log_write_bool(&log_local, BOOL_VAR)

/**
 * 向日志消息添加带有“name”属性的布尔参数.
 * @param BOOL_VAR Boolean value (mapped to uint8)
 * @param NAME "name" attribute
 */
#define DLT_BOOL_ATTR(BOOL_VAR, NAME) \
    (void)dlt_user_log_write_bool_attr(&log_local, BOOL_VAR, NAME)

/**
 * 在日志信息中增加float32参数
 * @param FLOAT32_VAR Float32 value (mapped to float)
 */
#define DLT_FLOAT32(FLOAT32_VAR) \
    (void)dlt_user_log_write_float32(&log_local, FLOAT32_VAR)

/**
 * 在日志信息中增加float64参数
 * @param FLOAT64_VAR Float64 value (mapped to double)
 */
#define DLT_FLOAT64(FLOAT64_VAR) \
    (void)dlt_user_log_write_float64(&log_local, FLOAT64_VAR)

/**
 * 在日志信息中增加带属性的float32参数。
 * @param FLOAT32_VAR Float32 value (mapped to float)
 * @param NAME "name" attribute
 * @param UNIT "unit" attribute
 */
#define DLT_FLOAT32_ATTR(FLOAT32_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_float32_attr(&log_local, FLOAT32_VAR, NAME, UNIT)

/**
 * 在日志信息中增加带属性的float64参数。.
 * @param FLOAT64_VAR Float64 value (mapped to double)
 * @param NAME "name" attribute
 * @param UNIT "unit" attribute
 */
#define DLT_FLOAT64_ATTR(FLOAT64_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_float64_attr(&log_local, FLOAT64_VAR, NAME, UNIT)

/**
 * 在日志信息中增加整型参数。
 * @param INT_VAR integer value
 */
#define DLT_INT(INT_VAR) \
    (void)dlt_user_log_write_int(&log_local, INT_VAR)

#define DLT_INT8(INT_VAR) \
    (void)dlt_user_log_write_int8(&log_local, INT_VAR)

#define DLT_INT16(INT_VAR) \
    (void)dlt_user_log_write_int16(&log_local, INT_VAR)

#define DLT_INT32(INT_VAR) \
    (void)dlt_user_log_write_int32(&log_local, INT_VAR)

#define DLT_INT64(INT_VAR) \
    (void)dlt_user_log_write_int64(&log_local, INT_VAR)

/**
 *在日志消息中添加带属性的整数参数。
 * @param INT_VAR integer value
 * @param NAME "name" attribute
 * @param UNIT "unit" attribute
 */
#define DLT_INT_ATTR(INT_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_int_attr(&log_local, INT_VAR, NAME, UNIT)

#define DLT_INT8_ATTR(INT_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_int8_attr(&log_local, INT_VAR, NAME, UNIT)

#define DLT_INT16_ATTR(INT_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_int16_attr(&log_local, INT_VAR, NAME, UNIT)

#define DLT_INT32_ATTR(INT_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_int32_attr(&log_local, INT_VAR, NAME, UNIT)

#define DLT_INT64_ATTR(INT_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_int64_attr(&log_local, INT_VAR, NAME, UNIT)

/**
 * 在日志消息中添加无符号整数参数。
 * @param UINT_VAR unsigned integer value
 */
#define DLT_UINT(UINT_VAR) \
    (void)dlt_user_log_write_uint(&log_local, UINT_VAR)

#define DLT_UINT8(UINT_VAR) \
    (void)dlt_user_log_write_uint8(&log_local, UINT_VAR)

#define DLT_UINT16(UINT_VAR) \
    (void)dlt_user_log_write_uint16(&log_local, UINT_VAR)

#define DLT_UINT32(UINT_VAR) \
    (void)dlt_user_log_write_uint32(&log_local, UINT_VAR)

#define DLT_UINT64(UINT_VAR) \
    (void)dlt_user_log_write_uint64(&log_local, UINT_VAR)

/**
 * 向日志消息添加带属性的无符号整数参数。
 * @param UINT_VAR unsigned integer value
 * @param NAME "name" attribute
 * @param UNIT "unit" attribute
 */
#define DLT_UINT_ATTR(UINT_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_uint_attr(&log_local, UINT_VAR, NAME, UNIT)

#define DLT_UINT8_ATTR(UINT_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_uint8_attr(&log_local, UINT_VAR, NAME, UNIT)

#define DLT_UINT16_ATTR(UINT_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_uint16_attr(&log_local, UINT_VAR, NAME, UNIT)

#define DLT_UINT32_ATTR(UINT_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_uint32_attr(&log_local, UINT_VAR, NAME, UNIT)

#define DLT_UINT64_ATTR(UINT_VAR, NAME, UNIT) \
    (void)dlt_user_log_write_uint64_attr(&log_local, UINT_VAR, NAME, UNIT)

/**
 * 向日志消息中添加二进制内存块。
 * @param BUF pointer to memory block
 * @param LEN length of memory block
 */
#define DLT_RAW(BUF, LEN) \
    (void)dlt_user_log_write_raw(&log_local, BUF, LEN)
#define DLT_HEX8(UINT_VAR) \
    (void)dlt_user_log_write_uint8_formatted(&log_local, UINT_VAR, DLT_FORMAT_HEX8)
#define DLT_HEX16(UINT_VAR) \
    (void)dlt_user_log_write_uint16_formatted(&log_local, UINT_VAR, DLT_FORMAT_HEX16)
#define DLT_HEX32(UINT_VAR) \
    (void)dlt_user_log_write_uint32_formatted(&log_local, UINT_VAR, DLT_FORMAT_HEX32)
#define DLT_HEX64(UINT_VAR) \
    (void)dlt_user_log_write_uint64_formatted(&log_local, UINT_VAR, DLT_FORMAT_HEX64)
#define DLT_BIN8(UINT_VAR) \
    (void)dlt_user_log_write_uint8_formatted(&log_local, UINT_VAR, DLT_FORMAT_BIN8)
#define DLT_BIN16(UINT_VAR) \
    (void)dlt_user_log_write_uint16_formatted(&log_local, UINT_VAR, DLT_FORMAT_BIN16)

/**
 * 在日志消息中添加带有“name”属性的二进制内存块.
 * @param BUF pointer to memory block
 * @param LEN length of memory block
 * @param NAME "name" attribute
 */
#define DLT_RAW_ATTR(BUF, LEN, NAME) \
    (void)dlt_user_log_write_raw_attr(&log_local, BUF, LEN, NAME)

/**
 * 用于打印指针的独立于体系结构的宏
 */
#define DLT_PTR(PTR_VAR) \
    (void)dlt_user_log_write_ptr(&log_local, PTR_VAR)

/**
 * 跟踪网络信息
 * @param CONTEXT object containing information about one special logging context
 * @param TYPE type of network trace message
 * @param HEADERLEN length of network message header
 * @param HEADER pointer to network message header
 * @param PAYLOADLEN length of network message payload
 * @param PAYLOAD pointer to network message payload
 */
#define DLT_TRACE_NETWORK(CONTEXT, TYPE, HEADERLEN, HEADER, PAYLOADLEN, PAYLOAD) \
    do { \
        if ((CONTEXT).trace_status_ptr && *((CONTEXT).trace_status_ptr) == DLT_TRACE_STATUS_ON) \
        { \
            (void)dlt_user_trace_network(&(CONTEXT), TYPE, HEADERLEN, HEADER, PAYLOADLEN, PAYLOAD); \
        } \
    } while (0)

/**
 * 跟踪网络消息，允许截断
 * @param CONTEXT object containing information about one special logging context
 * @param TYPE type of network trace message
 * @param HEADERLEN length of network message header
 * @param HEADER pointer to network message header
 * @param PAYLOADLEN length of network message payload
 * @param PAYLOAD pointer to network message payload
 */
#define DLT_TRACE_NETWORK_TRUNCATED(CONTEXT, TYPE, HEADERLEN, HEADER, PAYLOADLEN, PAYLOAD) \
    do { \
        if ((CONTEXT).trace_status_ptr && *((CONTEXT).trace_status_ptr) == DLT_TRACE_STATUS_ON) \
        { \
            (void)dlt_user_trace_network_truncated(&(CONTEXT), TYPE, HEADERLEN, HEADER, PAYLOADLEN, PAYLOAD, 1); \
        } \
    } while (0)

/**
 * 跟踪网络消息，分段超长消息
 * @param CONTEXT object containing information about one special logging context
 * @param TYPE type of network trace message
 * @param HEADERLEN length of network message header
 * @param HEADER pointer to network message header
 * @param PAYLOADLEN length of network message payload
 * @param PAYLOAD pointer to network message payload
 */
#define DLT_TRACE_NETWORK_SEGMENTED(CONTEXT, TYPE, HEADERLEN, HEADER, PAYLOADLEN, PAYLOAD) \
    do { \
        if ((CONTEXT).trace_status_ptr && *((CONTEXT).trace_status_ptr) == DLT_TRACE_STATUS_ON) \
        { \
            (void)dlt_user_trace_network_segmented(&(CONTEXT), TYPE, HEADERLEN, HEADER, PAYLOADLEN, PAYLOAD); \
        } \
    } while (0)

/**
 * 发送带有字符串参数的日志消息。
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log message
 * @param TEXT ASCII string
 */
#define DLT_LOG_STRING(CONTEXT, LOGLEVEL, TEXT) \
    do { \
        if (dlt_user_is_logLevel_enabled(&CONTEXT, LOGLEVEL) == DLT_RETURN_TRUE) \
        { \
            (void)dlt_log_string(&(CONTEXT), LOGLEVEL, TEXT); \
        } \
    } while (0)

/**
 * 发送带有字符串参数和整数参数的日志消息。
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log messages
 * @param TEXT ASCII string
 * @param INT_VAR integer value
 */
#define DLT_LOG_STRING_INT(CONTEXT, LOGLEVEL, TEXT, INT_VAR) \
    do { \
        if (dlt_user_is_logLevel_enabled(&CONTEXT, LOGLEVEL) == DLT_RETURN_TRUE) \
        { \
            (void)dlt_log_string_int(&(CONTEXT), LOGLEVEL, TEXT, INT_VAR); \
        } \
    } while (0)

/**
 * 发送带有字符串参数和无符号整数参数的日志消息。
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log message
 * @param TEXT ASCII string
 * @param UINT_VAR unsigned integer value
 */
#define DLT_LOG_STRING_UINT(CONTEXT, LOGLEVEL, TEXT, UINT_VAR) \
    do { \
        if (dlt_user_is_logLevel_enabled(&CONTEXT, LOGLEVEL) == DLT_RETURN_TRUE) \
        { \
            (void)dlt_log_string_uint(&(CONTEXT), LOGLEVEL, TEXT, UINT_VAR); \
        } \
    } while (0)

/**
 * 发送带有无符号整数参数的日志消息。
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log message
 * @param UINT_VAR unsigned integer value
 */
#define DLT_LOG_UINT(CONTEXT, LOGLEVEL, UINT_VAR) \
    do { \
        if (dlt_user_is_logLevel_enabled(&CONTEXT, LOGLEVEL) == DLT_RETURN_TRUE) \
        { \
            (void)dlt_log_uint(&(CONTEXT), LOGLEVEL, UINT_VAR); \
        } \
    } while (0)

/**
 * 发送带整型参数的日志消息。
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log message
 * @param INT_VAR integer value
 */
#define DLT_LOG_INT(CONTEXT, LOGLEVEL, INT_VAR) \
    do { \
        if (dlt_user_is_logLevel_enabled(&CONTEXT, LOGLEVEL) == DLT_RETURN_TRUE) \
        { \
            (void)dlt_log_int(&(CONTEXT), LOGLEVEL, INT_VAR); \
        } \
    } while (0)

/**
 * 发送带有二进制内存块的日志消息。
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log message
 * @param BUF pointer to memory block
 * @param LEN length of memory block
 */
#define DLT_LOG_RAW(CONTEXT, LOGLEVEL, BUF, LEN) \
    do { \
        if (dlt_user_is_logLevel_enabled(&CONTEXT, LOGLEVEL) == DLT_RETURN_TRUE) \
        { \
            (void)dlt_log_raw(&(CONTEXT), LOGLEVEL, BUF, LEN); \
        } \
    } while (0)

/**
 * 发送带有标记的日志消息。
 */
#define DLT_LOG_MARKER() \
    do { \
        (void)dlt_log_marker(); \
    } while (0)

/**
 * 选择详细模式
 *
 */
#define DLT_VERBOSE_MODE() do { \
        (void)dlt_verbose_mode(); } while (0)

/**
 * 选择非详细模式
 *
 */
#define DLT_NONVERBOSE_MODE() do { \
        (void)dlt_nonverbose_mode(); } while (0)

/**
 * 设置应用程序的最大日志级别和跟踪状态
 *
 * @param LOGLEVEL This is the log level to be set for the whole application
 * @param TRACESTATUS This is the trace status to be set for the whole application
 */
#define DLT_SET_APPLICATION_LL_TS_LIMIT(LOGLEVEL, TRACESTATUS) do { \
        (void)dlt_set_application_ll_ts_limit(LOGLEVEL, TRACESTATUS); } while (0)

/**
 * 打开本地打印模式
 *
 */
#define DLT_ENABLE_LOCAL_PRINT() do { \
        (void)dlt_enable_local_print(); } while (0)

/**
 * 关闭本地打印模式
 *
 */
#define DLT_DISABLE_LOCAL_PRINT() do { \
        (void)dlt_disable_local_print(); } while (0)

/**
 * 检查日志级别是否开启
 *
 * @param CONTEXT object containing information about one special logging context
 * @param LOGLEVEL the log level of the log message
 */
#define DLT_IS_LOG_LEVEL_ENABLED(CONTEXT, LOGLEVEL) \
    (dlt_user_is_logLevel_enabled(&CONTEXT, LOGLEVEL) == DLT_RETURN_TRUE)

/**
 \}
 */

#endif /* DLT_USER_MACROS_H */
