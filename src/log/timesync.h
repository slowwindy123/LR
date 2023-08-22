/*! 
 *   @file timesync.h
 *  @brief Head file for declaring class TimeSync.
 *  
 *  Copyright (c) by Hinge-Tech R&D Center.\n
 *  Description: declare the base of class TimeSync for user\n
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
#ifndef ARA_LOG_TIMESYNC_H__
#define ARA_LOG_TIMESYNC_H__

#include <string>

namespace ara 
{
namespace log 
{

/*!
 *  @brief  The class TimeSync represents time syncanization. 
 */

class TimeSync
{
public:
    /*!
     *  @brief  TimeSync constructor.
     * 
     *  @details convert cfg string value to enum value 
     */
    TimeSync(std::string timeResource);

private:
    /*!  @brief  Time Base resource from   */
    enum TimeBaseResource
    {
        kNoneTimeBase = 0,  /*!< no time syncanization  */
        kPureLocalTimeBase  /*!< pure local time syncanization  */
    };
    TimeBaseResource m_timeRes; /*!< member of time syncanization  */
};

}
}
#endif // ARA_LOG_TIMESYNC_H__
