/********************************************************************************
 *  File Name:
 *    sink_cout.cpp
 *
 *  Description:
 *    Implementation of the std::cout based uLog sink
 *
 *  2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

/* C++ Includes */
#include <iostream>
#include <string>

/* uLog Includes */
#include <uLog/sinks/sink_cout.hpp>

namespace uLog
{
  CoutSink::CoutSink()
  { 
    enabled = false;
    logLevel = LogLevelType::LOG_LEVEL_MIN;
  }

  CoutSink::~CoutSink()
  {
  }

  ResultType CoutSink::open()
  {
    return ResultType::RESULT_SUCCESS;
  }

  ResultType CoutSink::close()
  {
    return ResultType::RESULT_SUCCESS;
  }

  ResultType CoutSink::flush()
  {
    std::cout << " " << std::flush;
    return ResultType::RESULT_SUCCESS;
  }

  ResultType CoutSink::enable()
  {
    enabled = true;
    return ResultType::RESULT_SUCCESS;
  }

  ResultType CoutSink::disable()
  {
    enabled = false;
    return ResultType::RESULT_SUCCESS;
  }

  ResultType CoutSink::setLogLevel( const LogLevelType level )
  {
    logLevel = level;
    return ResultType::RESULT_SUCCESS;
  }

  LogLevelType CoutSink::getLogLevel()
  {
    return logLevel;
  }

  ResultType CoutSink::log( const LogLevelType level, const void *const message, const size_t length )
  {
    /*------------------------------------------------
    Check to see if we should even write
    ------------------------------------------------*/
    if (!enabled || ( level < logLevel ) || !message || !length)
    {
      return ResultType::RESULT_FAIL;
    }

    /*------------------------------------------------
    Simply assign the data as a string, then write it.
    ------------------------------------------------*/
    std::string coutBuffer;
    coutBuffer.assign( reinterpret_cast<const char *const>( message ), length );
    std::cout << coutBuffer << std::flush;

    return ResultType::RESULT_SUCCESS;
  }
}    // namespace uLog
