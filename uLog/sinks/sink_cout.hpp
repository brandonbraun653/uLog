/********************************************************************************
 *  File Name:
 *    sink_cout.hpp
 *
 *  Description:
 *    Implements a sink for std::cout
 *
 *  2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

#pragma once
#ifndef MICRO_LOGGER_SINK_COUT_HPP
#define MICRO_LOGGER_SINK_COUT_HPP

/* C++ Includes */
#include <cstdlib>

/* uLog Includes */
#include <uLog/sinks/sink_intf.hpp>
#include <uLog/types.hpp>

namespace uLog
{
  class CoutSink : public SinkInterface
  {
  public:
    CoutSink();
    ~CoutSink();

    ResultType open() final override;

    ResultType close() final override;

    ResultType flush() final override;

    ResultType enable() final override;

    ResultType disable() final override;

    ResultType setLogLevel( const LogLevelType level ) final override;

    LogLevelType getLogLevel() final override;

    ResultType log( const LogLevelType level, const void *const message, const size_t length ) final override;

  private:
    bool enabled;
    LogLevelType logLevel;
  };
}    // namespace uLog

#endif    // !MICRO_LOGGER_SINK_COUT_HPP
