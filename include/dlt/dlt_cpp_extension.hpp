/*
 * SPDX license identifier: MPL-2.0
 *
 * Copyright (C) 2015  Intel Corporation
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
 * \author Stefan Vacek <stefan.vacek@intel.com> Intel Corporation
 *
 * \copyright Copyright © 2015 Intel Corporation. \n
 * License MPL-2.0: Mozilla Public License version 2.0 http://mozilla.org/MPL/2.0/.
 *
 * \file dlt_cpp_extension.hpp
*/

#ifndef DLT_CPP_EXTENSION_HPP
#define DLT_CPP_EXTENSION_HPP

#include <string>
#include <vector>
#include <list>
#include <map>

#include "dlt.h"

template<typename T>
int32_t logToDlt(DltContextData &log, T const &value) = delete;

template<>
inline int32_t logToDlt(DltContextData &log, int8_t const &value)
{
    return dlt_user_log_write_int8(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, int16_t const &value)
{
    return dlt_user_log_write_int16(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, int32_t const &value)
{
    return dlt_user_log_write_int32(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, int64_t const &value)
{
    return dlt_user_log_write_int64(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, uint8_t const &value)
{
    return dlt_user_log_write_uint8(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, uint16_t const &value)
{
    return dlt_user_log_write_uint16(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, uint32_t const &value)
{
    return dlt_user_log_write_uint32(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, uint64_t const &value)
{
    return dlt_user_log_write_uint64(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, float32_t const &value)
{
    return dlt_user_log_write_float32(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, double const &value)
{
    return dlt_user_log_write_float64(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, bool const &value)
{
    return dlt_user_log_write_bool(&log, value);
}

static inline int32_t logToDlt(DltContextData &log, char const * const value)
{
    return dlt_user_log_write_utf8_string(&log, value);
}

static inline int32_t logToDlt(DltContextData &log, char * const value)
{
    return dlt_user_log_write_utf8_string(&log, value);
}

template<>
inline int32_t logToDlt(DltContextData &log, std::string const &value)
{
    return dlt_user_log_write_utf8_string(&log, value.c_str());
}

/* stl types */
template<>
int32_t logToDlt(DltContextData &log, std::string const &value);

template<typename _Tp, typename _Alloc = std::allocator<_Tp>>
static inline int32_t logToDlt(DltContextData &log, std::vector<_Tp, _Alloc> const & value)
{
    int result = 0;

    for (auto elem : value)
        result += logToDlt(log, elem);

    if (result != 0)
        result = -1;

    return result;
}

template<typename _Tp, typename _Alloc = std::allocator<_Tp>>
static inline int32_t logToDlt(DltContextData &log, std::list<_Tp, _Alloc> const & value)
{
    int result = 0;

    for (auto elem : value)
        result += logToDlt(log, elem);

    if (result != 0)
        result = -1;

    return result;
}

template<typename _Key, typename _Tp, typename _Compare = std::less<_Key>,
                typename _Alloc = std::allocator<std::pair<const _Key, _Tp>>>
static inline int32_t logToDlt(DltContextData &log, std::map<_Key, _Tp, _Compare, _Alloc> const & value)
{
    int result = 0;

    for (auto elem : value)
    {
        result += logToDlt(log, elem.first);
        result += logToDlt(log, elem.second);
    }

    if (result != 0)
        result = -1;

    return result;
}

//可变函数使用C11标准
template<typename First>
static inline int32_t logToDltVariadic(DltContextData &log, First const &valueA)
{
    return logToDlt(log, valueA);
}

template<typename First, typename ... Rest>
static inline int32_t logToDltVariadic(DltContextData &log, First const &valueA, const Rest&... valueB)
{
    int result = logToDlt(log, valueA) + logToDltVariadic(log, valueB...);

    if (result != 0)
        result = -1;

    return result;
}

/**
 * @brief 宏用于编写具有可变数量参数的日志消息，而无需指定日志数据的类型
*该宏可以用于任何提供logToDlt函数的类型。
 *
 * Example:
 * DLT_LOG_CXX(dltContext, DLT_LV_X, "text", valueA, valueB, ...)
 */
#define DLT_LOG_CXX(CONTEXT, LOGLEVEL, ...)\
    do\
    {\
        DltContextData log;\
        if (dlt_user_log_write_start(&CONTEXT,&log,LOGLEVEL)>0)\
        {\
            logToDltVariadic(log, ##__VA_ARGS__);\
            dlt_user_log_write_finish(&log);\
        }\
    }\
    while(0)

/**
 * @brief 宏用于编写具有可变数量参数的日志消息，而无需指定日志数据的类型。

*该宏可以用于任何提供logToDlt函数的类型。
*这包括代码生成的所有类型。

*这个宏类似于 DLT_LOG_CXX。但是，它将当前函数名添加为第一个日志参数。
 *
 * Example:
 * DLT_LOG_FCN_CXX(dltContext, DLT_LV_X, "text", valueA, valueB, ...)
 */
#define DLT_LOG_FCN_CXX(CONTEXT, LOGLEVEL, ...) \
    do\
    {\
        DltContextData log;\
        if (dlt_user_log_write_start(&CONTEXT, &log, LOGLEVEL) > 0)\
        {\
            dlt_user_log_write_string(&log, __PRETTY_FUNCTION__);\
            logToDltVariadic(log, ##__VA_ARGS__);\
            dlt_user_log_write_finish(&log);\
        }\
  }\
  while(0)

#endif /* DLT_CPP_EXTENSION_HPP */
