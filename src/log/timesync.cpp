/*! 
 *   @file timesync.cpp
 *  @brief Source file for implementing class TimeSync.
 *  
 *  Copyright (c) by Hinge-Tech R&D Center.\n
 *  Description: implement the base of class TimeSync for user\n
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

#include "timesync.h"
#include "dlt/dlt_user.h"

namespace ara 
{
namespace log 
{

TimeSync::TimeSync(std::string timeResource)
{
    m_timeRes = kPureLocalTimeBase;
    if ("PureLocalTimeBase" == timeResource)
    {
        m_timeRes = kPureLocalTimeBase;
    }

    // set the back-end
    dlt_with_timestamp((int8_t)m_timeRes);
}

}
}