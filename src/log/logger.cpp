/*! 
 *  @file logger.cpp
 *  @brief Source file for implementing class Logger.
 *  
 *  Copyright (c) by Hinge-Tech R&D Center.\n
 *  Description: implement class Logger
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
#include <iostream>
#include <memory>
#include <stdarg.h>

#include <dlt/dlt.h>

#include "ara/log/logger.h"
#include "ara/log/utility.h"

namespace ara 
{
namespace log 
{

/*!
 *  @brief The implement class of Logger 
 */
class Logger::Impl
{
public:
	/*! 
     *  @brief Construct a logger context for Logger::Impl.
	 *  
	 *  @param _parent the pointer of Logger object
     *  @param ctxId the ID of logger context
     *  @param ctxDescription the ID of logger conext description
     *  @param ctxDefLogLevel the default log loevel
	 *  
	 *  @see Logger::Logger
	 */
    Impl(Logger* _parent, const std::string& ctxId, const std::string& ctxDescription, LogLevel ctxDefLogLevel) :
        parent(_parent)
    {
        const std::string id{internal::dltId(ctxId)};
        LoggerLevel = (DltLogLevelType)ctxDefLogLevel;
        LoggerTraceStatus = DLT_TRACE_STATUS_OFF;
        DltReturnValue ret = dlt_register_context_ll_ts(&dltContext_,
                                                        id.c_str(),
                                                        ctxDescription.c_str(),
                                                        static_cast<DltLogLevelType>(ctxDefLogLevel),
                                                        DLT_TRACE_STATUS_OFF);

        if (ret < DltReturnValue::DLT_RETURN_OK) {
            std::string err_msg = "logging: unable to register context [" + id + "] to DLT back-end. Error-code: " + std::to_string(ret);
            DLT_LOG(dltContext_, DLT_LOG_ERROR, DLT_STRING(err_msg.c_str()));
        }
        else {
            isRegistered_ = true;
        }
    };

	/*! 
     *  @brief Destruct a logger context for Logger::Impl.
	 *  
	 */
    ~Impl()
    {
        unregisterBackends();
    };

	/*! 
     *  @brief Ask for when this Logger will ouput messages for the specified log loevel;
	 *  
     *  @return Return whether this Logger will output messages for the specified log level.
     *   - true if this Logger will output messages
     *   - false if this Logger will not output messages
	 */
    bool IsEnabled(LogLevel logLevel) const noexcept
    {
        return (dlt_user_is_logLevel_enabled(const_cast<DltContext*>(&dltContext_),
                                             static_cast<DltLogLevelType>(logLevel)) == DLT_RETURN_TRUE);
    }


     /*! 
     *  @brief Log message with a programmatically determined log level can be written.
     * 
     *  @param logLevel the log level to use for this LogStream instance  
     *  
     *  @return a new LogStream instance with the given log level
     *  
	 *  
	 */
    //TODO
    LogStream WithLevel (LogLevel logLevel) const noexcept
    {
        return LogStream{logLevel, *parent};
    }

	/*! 
     *  @brief Create a LogStream object for FATAL messages. 
     *  @return LogStream object of FATAL severity.
	 */
    LogStream LogFatal() noexcept
    {
        return LogStream{LogLevel::kFatal, *parent};
    }

	/*! 
     *  @brief Create a LogStream object for ERROR messages. 
     *  @return LogStream object of ERROR severity.
	 */  
    LogStream LogError() noexcept
    {
        return LogStream{LogLevel::kError, *parent};
    }

	/*! 
     *  @brief Create a LogStream object for WARN messages. 
     *  @return LogStream object of WARN severity.
	 */  
    LogStream LogWarn() noexcept
    {
        return LogStream{LogLevel::kWarn, *parent};
    }

	/*! 
     *  @brief Create a LogStream object for INFO messages. 
     *  @return LogStream object of INFO severity.
	 */  
    LogStream LogInfo() noexcept
    {
        return LogStream{LogLevel::kInfo, *parent};
    }

	/*! 
     *  @brief Create a LogStream object for DEBUG messages. 
     *  @return LogStream object of DEBUG severity.
	 */  
    LogStream LogDebug() noexcept
    {
        return LogStream{LogLevel::kDebug, *parent};
    }

	/*! 
     *  @brief Create a LogStream object for VERBOSE messages. 
     *  @return LogStream object of VERBOSE severity.
	 */ 
    LogStream LogVerbose() noexcept
    {
        return LogStream{LogLevel::kVerbose, *parent};
    }

	/*! 
     *  @brief Unregister Logger context from back-end;
     * 
	 */
    void unregisterBackends() noexcept
    {
        // Single backend supported from now
        unregisterFromDlt();
    }

	/*! 
     *  @brief Unregister Logger context from back-end;
	 */
    void unregisterFromDlt() noexcept
    {
        if (isRegistered_)
        {
            isRegistered_ = false;
            (void) dlt_unregister_context(&dltContext_);
        }
    }

	/*! 
     *  @brief get the ID of Logger context.
     *  @return Return the ID of Logger context
     */ 
    std::string getId() const noexcept
    {
        // string ctor usage might not be no-throw guaranteed - implementation not to be used for series SW
        return std::string(dltContext_.contextID, DLT_ID_SIZE).c_str();
    }

	/*! 
     *  @brief Get the logger context from back-end
     *  @return Return the pointer of logger context from back-end.
     *   - non-pointer if this Logger is initiated rightly
     *   - nullptr otherwise
     */ 
    void* getContext() noexcept
    {
        return &dltContext_;
    }

    /**
     * trace network messages, allowing truncation
     * @param nw_trace_type type of network trace message
     * @param header_len length of network message header
     * @param header pointer to network message header
     * @param payload_len length of network message payload
     * @param payload pointer to network message payload
     * @param allow_truncate default false, it could truncate if true 
     */
    void traceNetworkTruncated( DltNetworkTraceType nw_trace_type,
                                uint16_t header_len,
                                void *header,
                                uint16_t payload_len,
                                void *payload,
                                int allow_truncate = 0)
    {
        (void)dlt_set_application_ll_ts_limit(LoggerLevel, DLT_TRACE_STATUS_ON);
        LoggerTraceStatus = DLT_TRACE_STATUS_ON;
        if (dltContext_.trace_status_ptr && *(dltContext_.trace_status_ptr) == DLT_TRACE_STATUS_ON) 
        { 
            (void)dlt_user_trace_network_truncated(&dltContext_, nw_trace_type, header_len, header, payload_len, payload, allow_truncate); 
        } 
    }

    /**
     * Track network messages and segment long messages
     * @param nw_trace_type type of network trace message
     * @param header_len length of network message header
     * @param header pointer to network message header
     * @param payload_len length of network message payload
     * @param payload pointer to network message payload
     */
    void traceNetworkSegmented( DltNetworkTraceType nw_trace_type,
                                uint16_t header_len,
                                void *header,
                                uint16_t payload_len,
                                void *payload)
    {
        (void)dlt_set_application_ll_ts_limit(LoggerLevel, DLT_TRACE_STATUS_ON);
        LoggerTraceStatus = DLT_TRACE_STATUS_ON;
        if (dltContext_.trace_status_ptr && *(dltContext_.trace_status_ptr) == DLT_TRACE_STATUS_ON) 
        { 
            (void)dlt_user_trace_network_segmented(&dltContext_, nw_trace_type, header_len, header, payload_len, payload); 
        } 
    }

    DltLogLevelType LoggerLevel;
    DltTraceStatusType LoggerTraceStatus;

    Logger*           parent;                /*!<  @brief the pointer to Logger */

    std::atomic<bool> isRegistered_{false};  /*!<  @brief whether registed or not */
    DltContext        dltContext_{};         /*!<  @brief dlt context at back-end */
};

//-----------------------------------------------------------------------------------------------------------

Logger::Logger(const std::string& ctxId, const std::string& ctxDescription, LogLevel ctxDefLogLevel)
: pImpl{std::make_unique<Impl>(this, ctxId, ctxDescription, ctxDefLogLevel)} {}

Logger::~Logger()
{
    for (std::map<LogLevel, LogStream*>::iterator iter = m_logstreams.begin(); m_logstreams.end() != iter; ++iter)
    {
        delete (iter->second);
    }

    unregisterBackends();
}

void Logger::unregisterBackends() noexcept
{
    pImpl->unregisterBackends();
}

std::string Logger::getId() const noexcept
{
    return pImpl->getId();
}

LogStream Logger::WithLevel (LogLevel logLevel) const noexcept
{
    return pImpl->WithLevel(logLevel);
}

LogStream Logger::LogFatal() noexcept
{
    return pImpl->LogFatal();
}

LogStream Logger::LogError() noexcept
{
    return pImpl->LogError();
}

LogStream Logger::LogWarn() noexcept
{
    return pImpl->LogWarn();
}

LogStream Logger::LogInfo() noexcept
{
    return pImpl->LogInfo();
}

LogStream Logger::LogDebug() noexcept
{
    return pImpl->LogDebug();
}

LogStream Logger::LogVerbose() noexcept
{
    return pImpl->LogVerbose();
}

bool Logger::IsEnabled(LogLevel logLevel) const noexcept
{
    return pImpl->IsEnabled(logLevel);
}

void* Logger::getContext()
{
    return pImpl->getContext();
}

void Logger::traceNetworkTruncated(DltNetworkTraceType nw_trace_type,
                                uint16_t header_len,
                                void *header,
                                uint16_t payload_len,
                                void *payload,
                                int allow_truncate)
{
    pImpl->traceNetworkTruncated(nw_trace_type, header_len,  header,  payload_len,  payload,  allow_truncate);
}

void Logger::traceNetworkSegmented(DltNetworkTraceType nw_trace_type,
                                uint16_t header_len,
                                void *header,
                                uint16_t payload_len,
                                void *payload)
{
    pImpl->traceNetworkSegmented(nw_trace_type, header_len, header,  payload_len,  payload);
}

void Logger::syslog(LogLevel loglv, const std::string& str)
{
    syslog(loglv, str.c_str());
}

void Logger::syslog(LogLevel loglv, const char* fmt,...)
{
    char str[1390] = {0};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(str, 1390, fmt, ap);
    va_end(ap);

    LogStream *out = m_logstreams[loglv];
    if (nullptr == out)
    {
        out = new LogStream(loglv, (*this));
        m_logstreams.insert(std::pair<LogLevel, LogStream*>(loglv, out));
    }

    (*out) << (const char* const)str;
    out->Flush();
}


void Logger::setTraceStatus(DltTraceStatusType LoggerTraceStatus)
{
    (void)dlt_set_application_ll_ts_limit(pImpl->LoggerLevel, LoggerTraceStatus);
     pImpl->LoggerTraceStatus = LoggerTraceStatus;
}

DltTraceStatusType Logger::getTraceStatus()
{
    return  pImpl->LoggerTraceStatus;
}

void Logger::setMaxLogLevel(DltLogLevelType loglv)
{
    (void)dlt_set_application_ll_ts_limit(loglv,  pImpl->LoggerTraceStatus);
}

DltLogLevelType Logger::getMaxLogLevel()
{
    return pImpl->LoggerLevel;
}

} // namespace log
} // namespace ara
