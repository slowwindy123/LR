set( LT_MAJOR_VERSION 1)
set( LT_MINOR_VERSION 0)
set( LT_PATCH_LEVEL 0)
set( LT_VERSION ${LT_MAJOR_VERSION}.${LT_MINOR_VERSION}.${LT_PATCH_LEVEL})

add_subdirectory(tmp)

set(LOG_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logging.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logstream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timesync.cpp
    )

#message("${LOG_SRCS}")

INCLUDE_DIRECTORIES(${LOG_AND_TRACE_HOME}/include)
link_directories(${LOG_AND_TRACE_HOME}/lib)

add_library(log
    SHARED
    ${LOG_SRCS}
    )

#set_target_properties(log PROPERTIES VERSION ${LT_VERSION} SOVERSION ${LT_MAJOR_VERSION})

install(TARGETS log LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
