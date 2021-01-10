/********************************************************************************
 *  File Name:
 *    sink_intf.cpp
 *
 *  Description:
 *    Insert Description
 *
 *  2021 | Brandon Braun | brandonbraun653@gmail.com
 *******************************************************************************/

/* include description */
#include <uLog/sinks/sink_intf.hpp>

namespace uLog
{
  SinkInterface::SinkInterface()
  {
    mSinkEnabled  = false;
    mLoggingLevel = Level::LVL_MAX;
    mName         = "";
    mLogBuffer.fill( 0 );
  }
}  // namespace
