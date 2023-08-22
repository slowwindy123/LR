/*! 
 *  @file logmanager.cpp
 *  @brief Source file for implementing class LogManager.
 *  
 *  Copyright (c) by Hinge-Tech R&D Center.\n
 *  Description: implement class LogManager
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
#include "ara/log/logmanager.h"

#include <sys/stat.h>
#include <iostream>
#include <string.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <type_traits>

#include "dlt/dlt.h"
#include "dlt/dlt_common.h"
#include "ara/log/utility.h"

//TODO should use th 3rdlib
#include<boost/property_tree/ptree.hpp>
#include<boost/typeof/typeof.hpp>
#include<boost/property_tree/json_parser.hpp>
#include<boost/filesystem.hpp>
#include<boost/filesystem/fstream.hpp>

#include "timesync.h"


namespace ara 
{
namespace log 
{
/*! @brief length of dlt log message  */    
extern int64_t g_LogLength;

namespace 
{
constexpr char internCtxId[] = "INTM";
static Logger* logINT()
{
    static Logger s_logINT{internCtxId, "logging API internal context", LogLevel::kVerbose};
    g_LoggingInit = true; 
    return &s_logINT;
};

TimeSync* g_timesync = nullptr;
std::mutex g_mutex_logContexts;
std::unordered_map<std::string, std::unique_ptr<Logger>> g_logContexts;
} // 



LogManager::LogManager(const std::string& appId,
                       const std::string& description,
                       LogLevel logLevel,
                       LogMode logMode,
                       const char* Path) noexcept
{
    
    std::string timesync;
    uint32_t fileCount = 5;
    uint32_t fileSize =  1073741824; // 1024 *1024 * 1024 = 1073741824
    const std::string directoryPath = Path;

    try {
        namespace fs = boost::filesystem;
        namespace pt = boost::property_tree;
        using namespace std;

        std::ifstream ifstrema("/tmp/Log_LT.conf", std::ios_base::in);
        pt::ptree strTree;
        read_json(ifstrema, strTree);

        timesync = strTree.get<string>("TimeSyncModuleInstantiation.TimeBaseResource");
        fileCount = strTree.get<int>("DltFile.FileCount");
        fileSize = strTree.get<int>("DltFile.FileSize");
        g_LogLength = strTree.get<int>("Log.LogLength");
        g_BufferSize = strTree.get<int>("BufferSize");
    }
    catch (std::exception& e) {
        printf("Parse /tmp/Lg_LT.conf:  %s\n",e.what());
    }


     DltReturnValue ret{DltReturnValue::DLT_RETURN_OK};
    const std::string id{internal::dltId(appId)};

    if (logMode & LogMode::kFile)
    {
        if ((ret = dlt_init_file_dir(Path, fileCount, fileSize)) < DltReturnValue::DLT_RETURN_OK) {
            printf("logging: [ %s ] unable to init file log mode in DLT back-end!\n", id.c_str());
        }
        else{
                printf("Enabled File: %s output.\n", directoryPath.c_str());
        }
    }


//--------------------------------------------
    logINT();

    if (logMode & LogMode::kRemote) {
        dlt_enable_daemon();
        logINT()->LogInfo() << "Enabled Remote output.";
    }
    else{
        dlt_disable_daemon();
    }

    if (dlt_check_library_version(_DLT_PACKAGE_MAJOR_VERSION, _DLT_PACKAGE_MINOR_VERSION) <
        DltReturnValue::DLT_RETURN_OK) {
        logINT()->LogError() << "logging: mismatch of DLT back-end version. Further behavior is undefined!";
    }


    ret = dlt_register_app(id.c_str(), description.c_str());
    if (ret < DltReturnValue::DLT_RETURN_OK) {
        logINT()->LogError() << "logging: [" + id + "] unable to register application to DLT back-end. Error-code: " << ret;
    }
    


    if (logMode & LogMode::kConsole) {
        ret = dlt_enable_local_print();
        if (ret < DltReturnValue::DLT_RETURN_OK) {
            logINT()->LogWarn() << "Unable to setup console output in DLT back-end. Error-code:" << ret;
        }
        else {
            logINT()->LogInfo() << "Enabled console output.";
        }
    }

    // set application wide default log level for all already registered contexts
    ret = dlt_set_application_ll_ts_limit(static_cast<DltLogLevelType>(logLevel), DLT_TRACE_STATUS_OFF);
    
    if (ret < DltReturnValue::DLT_RETURN_OK) {
        logINT()->LogError() << "Unable to finalize init of DLT back-end. DLT daemon is probably not running.";
    }
    else {
        logINT()->LogInfo() << "Init DLT back-end done, all known contexts set to default log level: " + std::to_string(static_cast<std::underlying_type<LogLevel>::type>(logLevel));
    }

    g_timesync = new TimeSync(timesync);
}

LogManager::~LogManager()
{
    clean();
}

void LogManager::clean()
{
    std::lock_guard<std::mutex> guard(g_mutex_logContexts);

    for (const std::pair<const std::string, std::unique_ptr<Logger>>& context : g_logContexts) {
        context.second->unregisterBackends();
    }
    g_logContexts.clear();

    if (logINT())
    {
        logINT()->unregisterBackends();
    }
    (void) dlt_unregister_app();
    delete g_timesync;
    g_timesync = nullptr;
    g_LoggingInit = false;
    ///delete logINT();

}

Logger& LogManager::createLogContext(const std::string& ctxId,
                                        const std::string& ctxDescription,
                                        LogLevel ctxDefLogLevel) noexcept
{
    std::string id{internal::dltId(ctxId)};
    if (id == internCtxId || id.size() == 0) {
        logINT()->LogWarn() << "Tried to register an internally reserved context ID [" + id +
                              "] or ID is empty. Requested ID is changed to [XXXX]. Correct this in your code!";

        id = "XXXX";
    }

    try {
        std::lock_guard<std::mutex> guard(g_mutex_logContexts);

        std::unordered_map<std::string, std::unique_ptr<Logger>>::const_iterator ctxIter = g_logContexts.find(id);

        if (ctxIter == g_logContexts.cend()) {
            g_logContexts.emplace(id, std::make_unique<Logger>(id, ctxDescription, ctxDefLogLevel));
            // dlt_register_log_level_changed_callback((DltContext*)g_logContexts[id].get()->getContext(), dltlogLevelchangedcallback);
            return *g_logContexts[id].get();
        }
        else {
            logINT()->LogDebug() << "Requested an already registered context ID - returning: " << id;
            return *ctxIter->second.get();
        }
    }
    catch (const std::system_error& e) {
        logINT()->LogFatal() << "Caught system_error exception: " << e.what() << ", " << e.code().value();

        // We don't want the application to break, so returning the internal context as emergency fallback
        logINT()->LogFatal() << "Something went wrong in trying to create a logger context for the ID " << id
                            << ". As fallback the logging API internal context will be used.";
        return *logINT();
    }
    catch (const std::bad_alloc& e) {
        logINT()->LogError() << "Caught bad_alloc exception: " << e.what();

        // We don't want the application to break, so returning the internal context as emergency fallback
        logINT()->LogFatal() << "Something went wrong in trying to create a logger context for the ID " << id
                            << ". As fallback the logging API internal context will be used.";
        return *logINT();
    }
    catch (...) {
        logINT()->LogFatal() << "Caught unexpected exception.";

        // We don't want the application to break, so returning the internal context as emergency fallback
        logINT()->LogFatal() << "Something went wrong in trying to create a logger context for the ID " << id
                            << ". As fallback the logging API internal context will be used.";
        return *logINT();
    }
}

LogManager::ClientState LogManager::RemoteClientState() const noexcept
{
    return static_cast<ClientState>(dlt_get_log_state());
}

} // namespace log
} // namespace ara
