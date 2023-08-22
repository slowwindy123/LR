/*! 
*   @file logging.cpp
 *  @brief Source file for implementing log API.
 *  
 *  Copyright (c) by Hinge-Tech R&D Center.\n
 *  Description: implement the base of log API for user\n
 *  This is a c style API
 *               
 * 
 *  @author Rimon Chen
 *  @version 1.0
 *  @date 2019-07-05
 *  
 *  History:\n
 *  author          time                version         desc\n
 *  Rimon           2019-07-05          1.0             build this moudle\n
 */
#include <dlt/dlt.h>

#include "ara/log/logging.h"
#include "ara/log/logmanager.h"

namespace ara {
namespace log {

namespace {
// To prevent misuse of the API (calling functions on the LogManager::instance() without previous
// InitLogging() call on it), this variable must be checked in every function that calls LogManager::instance().
LogLevel g_defLogLevel{LogLevel::kVerbose};
}

void InitLogging(std::string appId,
                 std::string appDescription,
                 LogLevel appDefLogLevel,
                 LogMode logMode,
                 const char* directoryPath ) noexcept
{
    g_defLogLevel = appDefLogLevel;
    LogManager::instance(std::move(appId),
                         std::move(appDescription),
                         appDefLogLevel,
                         logMode,
                         directoryPath);
}

Logger& CreateLogger(std::string ctxId, std::string ctxDescription, LogLevel ctxDefLogLevel/*=LogLevel::kVerbose*/) noexcept
{
    LogLevel defLogLevel = (g_defLogLevel < ctxDefLogLevel ? g_defLogLevel : ctxDefLogLevel);
    return LogManager::createLogContext(std::move(ctxId), std::move(ctxDescription), defLogLevel);
}

static_assert(static_cast<DltLogLevelType>(LogLevel::kOff) == DLT_LOG_OFF, "LogLevel::kOff does not match DltLogLevelType::DLT_LOG_OFF");
static_assert(static_cast<DltLogLevelType>(LogLevel::kFatal) == DLT_LOG_FATAL, "LogLevel::kFatal does not match DltLogLevelType::DLT_LOG_FATAL");
static_assert(static_cast<DltLogLevelType>(LogLevel::kError) == DLT_LOG_ERROR, "LogLevel::kError does not match DltLogLevelType::DLT_LOG_ERROR");
static_assert(static_cast<DltLogLevelType>(LogLevel::kWarn) == DLT_LOG_WARN, "LogLevel::kWarn does not match DltLogLevelType::DLT_LOG_WARN");
static_assert(static_cast<DltLogLevelType>(LogLevel::kInfo) == DLT_LOG_INFO, "LogLevel::kInfo does not match DltLogLevelType::DLT_LOG_INFO");
static_assert(static_cast<DltLogLevelType>(LogLevel::kDebug) == DLT_LOG_DEBUG, "LogLevel::kDebug does not match DltLogLevelType::DLT_LOG_DEBUG");
static_assert(static_cast<DltLogLevelType>(LogLevel::kVerbose) == DLT_LOG_VERBOSE, "LogLevel::kVerbose does not match DltLogLevelType::DLT_LOG_VERBOSE");

static_assert(static_cast<DltReturnValue>(internal::LogReturnValue::kReturnLoggingDisabled) == DLT_RETURN_LOGGING_DISABLED,
              "internal::LogReturnValue::kReturnLoggingDisabled does not match DltReturnValue::DLT_RETURN_LOGGING_DISABLED");
static_assert(static_cast<DltReturnValue>(internal::LogReturnValue::kReturnUserBufferFull) == DLT_RETURN_USER_BUFFER_FULL,
              "internal::LogReturnValue::kReturnUserBufferFull does not match DltReturnValue::DLT_RETURN_USER_BUFFER_FULL");
static_assert(static_cast<DltReturnValue>(internal::LogReturnValue::kReturnWrongParameter) == DLT_RETURN_WRONG_PARAMETER,
              "internal::LogReturnValue::kReturnWrongParameter does not match DltReturnValue::DLT_RETURN_WRONG_PARAMETER");
static_assert(static_cast<DltReturnValue>(internal::LogReturnValue::kReturnBufferFull) == DLT_RETURN_BUFFER_FULL,
              "internal::LogReturnValue::kReturnBufferFull does not match DltReturnValue::DLT_RETURN_BUFFER_FULL");
static_assert(static_cast<DltReturnValue>(internal::LogReturnValue::kReturnPipeFull) == DLT_RETURN_PIPE_FULL,
              "internal::LogReturnValue::kReturnPipeFUll does not match DltReturnValue::DLT_RETURN_PIPE_FULL");
static_assert(static_cast<DltReturnValue>(internal::LogReturnValue::kReturnPipeError) == DLT_RETURN_PIPE_ERROR,
              "internal::LogReturnValue::kReturnPipeError does not match DltReturnValue::DLT_RETURN_PIPE_ERROR");
static_assert(static_cast<DltReturnValue>(internal::LogReturnValue::kReturnError) == DLT_RETURN_ERROR,
              "internal::LogReturnValue::kReturnError does not match DltReturnValue::DLT_RETURN_ERROR");
static_assert(static_cast<DltReturnValue>(internal::LogReturnValue::kReturnOk) == DLT_RETURN_OK,
              "internal::LogReturnValue::kReturnOk does not match DltReturnValue::DLT_RETURN_OK");
static_assert(static_cast<DltReturnValue>(internal::LogReturnValue::kReturnTrue) == DLT_RETURN_TRUE,
              "internal::LogReturnValue::kReturnTrue does not match DltReturnValue::DLT_RETURN_TRUE");













// void LogForkPrepare()
// {
//     dlt_fork_pre_fork_handler();
// }
// void LogForkParent()
// {
//     dlt_fork_parent_fork_handler();
// }
// void LogForkChild()
// {
//     dlt_fork_child_fork_handler();
// }

} // namespace log
} // namespace ara
