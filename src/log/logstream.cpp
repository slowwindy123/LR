/*! 
 *   @file logstream.cpp
 *  @brief Source file for implementing class LogStream.
 *  
 *  Copyright (c) by Hinge-Tech R&D Center.\n
 *  Description: Implement the base of class LogStream for user\n
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
#include "ara/log/logstream.h"

#include "ara/log/common.h"
#include <dlt/dlt.h>
#include <string.h>

#include "ara/log/logger.h"


namespace ara {
namespace log {

/*! 
     *  @brief create a buffer for the logData, make a gloabl variable to get the manager status of init.
	 *  
	 *  @var g_LoggingInit  the gloabl variable.
     *  @var logBuffer      the buffer for save the logData.
	 *  @var g_BufferSize   the buffer size.get it from the configura file by  logmanager.
*/
bool g_LoggingInit = false;
int g_BufferSize = -1;
std::deque<void*> g_LogBuffer;

/*! @brief length of dlt log message  */    
size_t g_LogLength = DLT_USER_BUF_MAX_SIZE;

LogStream::LogStream()
    : logLocalData_(0), logRet_(internal::LogReturnValue::kReturnOk)
{
    ;
}
LogStream::LogStream(LogLevel logLevel, Logger& logger) noexcept
{
    logLocalData_ = static_cast<void*>(new DltContextData);
    logRet_ = static_cast<internal::LogReturnValue>(
        dlt_user_log_write_start(
            static_cast<DltContext*>(logger.getContext()),
            static_cast<DltContextData*>(logLocalData_),
            static_cast<DltLogLevelType>(logLevel)));
}

LogStream::LogStream(LogLevel logLevel, Logger& logger, uint32_t& id) noexcept
{
    logLocalData_ = static_cast<void*>(new DltContextData);
    logRet_ = static_cast<internal::LogReturnValue>(
        dlt_user_log_write_start_id(
            static_cast<DltContext*>(logger.getContext()),
            static_cast<DltContextData*>(logLocalData_),
            static_cast<DltLogLevelType>(logLevel),
            id)
            );
}

LogStream::LogStream(const LogStream &other)
{
    logRet_ = internal::LogReturnValue::kReturnOk;
    logLocalData_ = static_cast<void*>(new DltContextData);
    DltContextData* otherDltContextData = static_cast<DltContextData*>(other.logLocalData_);
    if (otherDltContextData)
    {
        logRet_ = static_cast<internal::LogReturnValue>(
            dlt_user_log_write_start(
                otherDltContextData->handle,
                static_cast<DltContextData*>(logLocalData_),
                static_cast<DltLogLevelType>(otherDltContextData->log_level)));
    }
}

LogStream& LogStream::operator=(const LogStream&other)
{
    if (logLocalData_ && static_cast<DltContextData*>(logLocalData_)->size > 0) 
    {
        (void) dlt_user_log_write_finish(static_cast<DltContextData*>(logLocalData_));
    }
    if(static_cast<DltContextData*>(logLocalData_)){
        delete static_cast<DltContextData*>(logLocalData_);
    }
    

    logRet_ = internal::LogReturnValue::kReturnOk;
    logLocalData_ = static_cast<void*>(new DltContextData);
    DltContextData* otherDltContextData = static_cast<DltContextData*>(other.logLocalData_);
    if (otherDltContextData)
    {
        logRet_ = static_cast<internal::LogReturnValue>(
            dlt_user_log_write_start(
                otherDltContextData->handle,
                static_cast<DltContextData*>(logLocalData_),
                static_cast<DltLogLevelType>(otherDltContextData->log_level)));
    }

    return *this;
}

LogStream::LogStream(LogStream&&other)
{
    logLocalData_ = other.logLocalData_;
    logRet_ = other.logRet_;
    other.logLocalData_ = 0;
}

LogStream& LogStream::operator=(LogStream&&other)
{
    if (logLocalData_ && static_cast<DltContextData*>(logLocalData_)->size > 0) 
    {
        (void) dlt_user_log_write_finish(static_cast<DltContextData*>(logLocalData_));
    }
    if(static_cast<DltContextData*>(logLocalData_)){
        delete static_cast<DltContextData*>(logLocalData_);
    }
   

    logLocalData_ = other.logLocalData_;
    logRet_ = other.logRet_;
    other.logLocalData_ = 0;

    return *this;
}

LogStream::~LogStream()
{
    if (logLocalData_ && static_cast<DltContextData*>(logLocalData_)->size > 0) {
        (void) dlt_user_log_write_finish(static_cast<DltContextData*>(logLocalData_));
    }
    if(static_cast<DltContextData*>(logLocalData_)){
        delete static_cast<DltContextData*>(logLocalData_);
    }
    
}

void LogStream::Flush() noexcept
{
    if(!g_LoggingInit){
        if((size_t)g_BufferSize > g_LogBuffer.size()){
            DltContextData* tempBuffer = new DltContextData;
            *tempBuffer = *(static_cast<DltContextData*>(logLocalData_));
            g_LogBuffer.push_back(static_cast<void*>(tempBuffer));
            return ;
        }
        else{
            g_LogBuffer.pop_front();
             DltContextData* tempBuffer = new DltContextData;
            *tempBuffer = *(static_cast<DltContextData*>(logLocalData_));
            g_LogBuffer.push_back(static_cast<void*>(tempBuffer));
            return ;
        }
    }else if(!g_LogBuffer.empty()){
        for(auto& it:g_LogBuffer){
            (void) dlt_user_log_write_finish(static_cast<DltContextData*>(it));
            if(static_cast<DltContextData*>(it)){
                delete static_cast<DltContextData*>(it);
            } 
        }
        return ;
    }
    else{
        if (logLocalData_ && static_cast<DltContextData*>(logLocalData_)->size > 0) 
        {
            (void) dlt_user_log_write_finish(static_cast<DltContextData*>(logLocalData_));
        }
        logRet_ = static_cast<internal::LogReturnValue>(dlt_user_log_write_start(
        static_cast<DltContextData*>(logLocalData_)->handle,
        static_cast<DltContextData*>(logLocalData_),
        static_cast<DltLogLevelType>(static_cast<DltContextData*>(logLocalData_)->log_level)));
        return ;
    }
}

LogStream& LogStream::WithLocation (core::StringView file, int line) noexcept
{
    *this<< file << line;
    return *this;
}

LogStream& LogStream::operator<<(const ara::core::StringView value) noexcept
{
    char* buffer = (char*)const_cast<void *>((const void*)value.data());
    
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        int32_t sizeToSend = value.length();
        int32_t sizeHaveSent = 0;

        while (sizeHaveSent < sizeToSend)
        {
            int32_t currSize = plogdata->size + 1 + sizeof(uint16_t) + sizeof(uint32_t);
            int32_t leftSize = g_LogLength - currSize;
            if (leftSize <= 0) // left spaces but not enough
            {
                Flush();
                continue;
            }
            char tmp[DLT_USER_BUF_MAX_SIZE] = {0};
            strncpy(tmp, buffer + sizeHaveSent, leftSize);
            DltReturnValue ret = dlt_user_log_write_utf8_string(plogdata, tmp);
            currSize += strlen(tmp);
            sizeHaveSent += strlen(tmp);
            if ((size_t)currSize == g_LogLength )
            {
                Flush();
            }
            if (ret < DLT_RETURN_OK)
            {
                break;
            }
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(core::Span< const core::Byte > value) noexcept
{
    void* buffer = const_cast<void *>((const void*)value.data());
    
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        int32_t sizeToSend = value.size_bytes();
        int32_t sizeHaveSent = 0;
        while (sizeHaveSent < sizeToSend)
        {
            int32_t currSize = plogdata->size + sizeof(size_t) + sizeof(uint32_t);
            int32_t leftSize = g_LogLength - currSize;
            if (leftSize <= 0) // left spaces but not enough
            {
                Flush();
                continue;
            }
            char tmp[DLT_USER_BUF_MAX_SIZE] = {0};
            int tmpsize = sizeToSend - sizeHaveSent;
            tmpsize = (tmpsize < leftSize ? tmpsize : leftSize);
            char *ptr = (char*)buffer + sizeHaveSent;
            memcpy(tmp, ptr, tmpsize);
            DltReturnValue ret = dlt_user_log_write_raw_formatted(
            plogdata, (void*)tmp, tmpsize, DLT_FORMAT_DEFAULT);
            currSize += tmpsize;
            sizeHaveSent += tmpsize;
            if ((size_t)currSize == g_LogLength )
            {
                Flush();
            }
            if (ret < DLT_RETURN_OK)
            {
                break;
            }
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(bool value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk)
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(uint8_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_bool(plogdata, value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret) // clean and re-try
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(uint8_t value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(uint8_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_uint8(plogdata, value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(uint16_t value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(uint16_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_uint16(plogdata, value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(uint32_t value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(uint32_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_uint32(plogdata, value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(uint64_t value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(uint64_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_uint64(plogdata, value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(int8_t value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(uint64_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_int8(static_cast<DltContextData*>(logLocalData_), value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(int16_t value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(int16_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_int16(plogdata, value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(int32_t value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(int32_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_int32(plogdata, value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(int64_t value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(int64_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_int64(plogdata, value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(float value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(float32_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_float32(plogdata, value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(double value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        if ( g_LogLength > 0 && (plogdata->size+sizeof(float64_t) + sizeof(uint32_t)) > g_LogLength )
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_float64(plogdata, value);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(const char* const value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        int32_t sizeToSend = strlen(value);
        int32_t sizeHaveSent = 0;

        while (sizeHaveSent < sizeToSend)
        {
            int32_t currSize = plogdata->size + 1 + sizeof(uint16_t) + sizeof(uint32_t);
            int32_t leftSize = g_LogLength - currSize;
            if (leftSize <= 0) // left spaces but not enough
            {
                Flush();
                continue;
            }
            char tmp[DLT_USER_BUF_MAX_SIZE] = {0};
            strncpy(tmp, value + sizeHaveSent, leftSize);
            DltReturnValue ret = dlt_user_log_write_utf8_string(plogdata, tmp);
            currSize += strlen(tmp);
            sizeHaveSent += strlen(tmp);
            if ((size_t)currSize == g_LogLength )
            {
                Flush();
            }
            if (ret < DLT_RETURN_OK)
            {
                break;
            }
        }
    }
    return *this;
}


LogStream& LogStream::operator<<(const LogRawBuffer& value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        int32_t sizeToSend = value.size;
        int32_t sizeHaveSent = 0;
        while (sizeHaveSent < sizeToSend)
        {
            int32_t currSize = plogdata->size + sizeof(uint16_t) + sizeof(uint32_t);
            int32_t leftSize = g_LogLength - currSize;
            if (leftSize <= 0) // left spaces but not enough
            {
                Flush();
                continue;
            }
            char tmp[DLT_USER_BUF_MAX_SIZE] = {0};
            int tmpsize = sizeToSend - sizeHaveSent;
            tmpsize = (tmpsize < leftSize ? tmpsize : leftSize);
            char *ptr = (char*)value.buffer + sizeHaveSent;
            memcpy(tmp, ptr, tmpsize);
            DltReturnValue ret = dlt_user_log_write_raw_formatted(
            plogdata, (void*)tmp, tmpsize, DLT_FORMAT_DEFAULT);
            currSize += tmpsize;
            sizeHaveSent += tmpsize;
            if ((size_t)currSize == g_LogLength )
            {
                Flush();
            }
            if (ret < DLT_RETURN_OK)
            {
                break;
            }
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(const LogHex8& value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        uint16_t arg_size = plogdata->size + sizeof(uint8_t) + sizeof(uint32_t);
        if ( g_LogLength > 0 && (arg_size > g_LogLength ))
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_uint8_formatted(plogdata, value.value, DLT_FORMAT_HEX8);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(const LogHex16& value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        uint16_t arg_size = plogdata->size + sizeof(uint16_t) + sizeof(uint32_t);
        if ( g_LogLength > 0 && (arg_size > g_LogLength ))
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_uint16_formatted(plogdata, value.value, DLT_FORMAT_HEX16);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(const LogHex32& value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        uint16_t arg_size = plogdata->size + sizeof(uint32_t) + sizeof(uint32_t);
        if ( g_LogLength > 0 && (arg_size > g_LogLength ))
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_uint32_formatted(plogdata, value.value, DLT_FORMAT_HEX32);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(const LogHex64& value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        uint16_t arg_size = plogdata->size + sizeof(uint64_t) + sizeof(uint32_t);
        if ( g_LogLength > 0 && (arg_size > g_LogLength ))
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_uint64_formatted(plogdata, value.value, DLT_FORMAT_HEX64);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(const LogBin8& value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        uint16_t arg_size = plogdata->size + sizeof(uint8_t) + sizeof(uint32_t);
        if ( g_LogLength > 0 && (arg_size > g_LogLength ))
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_uint8_formatted(plogdata, value.value, DLT_FORMAT_BIN8);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}

LogStream& LogStream::operator<<(const LogBin16& value) noexcept
{
    if (logRet_ > internal::LogReturnValue::kReturnOk) 
    {
        DltContextData* plogdata = static_cast<DltContextData*>(logLocalData_);
        uint16_t arg_size = plogdata->size + sizeof(uint16_t) + sizeof(uint32_t);
        if ( g_LogLength > 0 && (arg_size > g_LogLength ))
        {
            Flush();
        }
        DltReturnValue ret = dlt_user_log_write_uint16_formatted(plogdata, value.value, DLT_FORMAT_BIN16);
        if (DLT_RETURN_USER_BUFFER_FULL == ret)
        {
            Flush();
            (*this) << value;
        }
    }
    return *this;
}


LogStream& operator<<(LogStream& out, LogLevel value) noexcept
{
    switch (value) 
    {
        case LogLevel::kOff:
            return (out << "Off");
        case LogLevel::kFatal:
            return (out << "Fatal");
        case LogLevel::kError:
            return (out << "Error");
        case LogLevel::kWarn:
            return (out << "Warn");
        case LogLevel::kInfo:
            return (out << "Info");
        case LogLevel::kDebug:
            return (out << "Debug");
        case LogLevel::kVerbose:
            return (out << "Verbose");
        case default:
            return (out << static_cast<typename std::underlying_type<LogLevel>::type>(value));
    }
}


void TypeFunc(DltContextData *log, float data, const char *name, const char *unit){
    (void)dlt_user_log_write_float32_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, double data, const char *name, const char *unit){
     (void)dlt_user_log_write_float64_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, uint8_t data, const char *name, const char *unit){
    (void)dlt_user_log_write_uint8_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, bool data, const char *name, const char *unit){
    (void)dlt_user_log_write_uint8_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, uint16_t data, const char *name, const char *unit){
    (void)dlt_user_log_write_uint16_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, uint32_t data, const char *name, const char *unit){
    (void)dlt_user_log_write_uint32_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, uint64_t data, const char *name, const char *unit){
    (void)dlt_user_log_write_uint64_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, int8_t data, const char *name, const char *unit){
    (void)dlt_user_log_write_int8_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, char data, const char *name, const char *unit){
    (void)dlt_user_log_write_int8_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, int16_t data, const char *name, const char *unit){
    (void)dlt_user_log_write_int16_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, int32_t data, const char *name, const char *unit){
    (void)dlt_user_log_write_int32_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, long long data, const char *name, const char *unit){
    (void)dlt_user_log_write_int64_attr(log, data, name, unit);
}
void TypeFunc(DltContextData *log, const char* const data, const char *name, const char *unit){
    (void)dlt_user_log_write_string(log, data);
}
void TypeFunc(DltContextData *log, core::StringView& data, const char *name, const char *unit){
    char* buffer = (char*)const_cast<void *>((const void*)data.data());
    (void)dlt_user_log_write_sized_string(log, buffer, data.size());
}
void TypeFunc(DltContextData *log, core::Span<const core::Byte>& data, const char *name, const char *unit){
    void* buffer = const_cast<void*>((const void*)data.data());
    (void)dlt_user_log_write_raw_formatted_attr(log, buffer, data.size_bytes(), DLT_FORMAT_DEFAULT,name);
}


} // namespace log
} // namespace ara
