#######
# SPDX license identifier: MPL-2.0
#
# Copyright (C) 2011-2015, BMW AG
#
# This file is part of GENIVI Project DLT - Diagnostic Log and Trace.
#
# This Source Code Form is subject to the terms of the
# Mozilla Public License (MPL), v. 2.0.
# If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.
#
# For further information see http://www.genivi.org/.
#######

if(WITH_DLT_DISABLE_MACRO)
    set(DLT_DISABLE_MACRO 1)
endif()


set(HEADER_LIST dlt.h dlt_user_macros.h dlt_client.h dlt_protocol.h
                dlt_common.h dlt_types.h dlt_shm.h dlt_offline_trace.h dlt_offline.h
                dlt_filetransfer.h dlt_common_api.h
                dlt_version.h
                dlt_user.h)

if(WITH_DLT_DISABLE_MACRO)
    list(REMOVE_ITEM HEADER_LIST dlt_user_macros.h)
endif()

if(WITH_DLT_CXX11_EXT)
    list(APPEND HEADER_LIST dlt_cpp_extension.hpp)
endif()

install(FILES ${HEADER_LIST}
        DESTINATION include/dlt
        COMPONENT devel)

