/********************************************************************************
 *  File Name:
 *    sink_cout.cpp
 *
 *  Description:
 *    Implementation of the Visual GDB ARM Semihosting sink. This will print out
 *    data onto the debug console inside of embedded visual studio projects with
 *    Visual GDB.
 *
 *  Links:
 *    https://visualgdb.com/tutorials/arm/semihosting/
 *
 *  2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

/* uLog Includes */
#include <uLog/sinks/sink_vgdb_semihosting.hpp>

#if defined( MICRO_LOGGER_HAS_VGDB_SEMIHOSTING ) && ( MICRO_LOGGER_HAS_VGDB_SEMIHOSTING == 1 )

extern int WriteToFastSemihostingChannel( unsigned char channel, const void *pBuffer, int size, int writeAll );

static constexpr unsigned char DefaultChannel = 1;    // Found by enabling printf redirects and then manually inspecting
static constexpr int DefaultWriteMode         = 1;    // This is equivalent to FAST_SEMIHOSTING_BLOCKING_MODE

namespace uLog
{
  VGDBSemihostingSink::VGDBSemihostingSink()
  {
    enabled  = false;
    logLevel = Level::LVL_MIN;
  }

  VGDBSemihostingSink::~VGDBSemihostingSink()
  {
  }

  Result VGDBSemihostingSink::open()
  {
    return Result::RESULT_SUCCESS;
  }

  Result VGDBSemihostingSink::close()
  {
    return Result::RESULT_SUCCESS;
  }

  Result VGDBSemihostingSink::flush()
  {
    return Result::RESULT_SUCCESS;
  }

  Result VGDBSemihostingSink::enable()
  {
    enabled = true;
    return Result::RESULT_SUCCESS;
  }

  Result VGDBSemihostingSink::disable()
  {
    enabled = false;
    return Result::RESULT_SUCCESS;
  }

  Result VGDBSemihostingSink::setLogLevel( const Level level )
  {
    logLevel = level;
    return Result::RESULT_SUCCESS;
  }

  Level VGDBSemihostingSink::getLogLevel()
  {
    return logLevel;
  }

  Result VGDBSemihostingSink::log( const Level level, const void *const message, const size_t length )
  {
    /*------------------------------------------------
    Check to see if we should even write
    ------------------------------------------------*/
    if ( !enabled || ( level < logLevel ) || !message || !length )
    {
      return Result::RESULT_FAIL;
    }

    /*------------------------------------------------
    Write the data!
    ------------------------------------------------*/
    auto result      = Result::RESULT_SUCCESS;
    int bytesWritten = WriteToFastSemihostingChannel( DefaultChannel, message, length, DefaultWriteMode );

    if ( static_cast<size_t>( bytesWritten ) != length )
    {
      result = Result::RESULT_FAIL;
    }

    return result;
  }
}    // namespace uLog

#endif /* MICRO_LOGGER_HAS_VGDB_SEMIHOSTING */