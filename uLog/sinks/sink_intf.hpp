/********************************************************************************
 *  File Name:
 *    sink_intf.hpp
 *
 *  Description:
 *    Defines the interface that sinks must implement at a bare minimum
 *
 *  2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

#pragma once
#ifndef MICRO_LOGGER_SINK_INTERFACE_HPP
#define MICRO_LOGGER_SINK_INTERFACE_HPP

/* C++ Includes */
#include <cstdint>

/* uLog Includes */
#include <uLog/types.hpp>

namespace uLog
{
  class SinkInterface
  {
  public:
    virtual ~SinkInterface() = default;

    virtual ResultType open() = 0;

    virtual ResultType close() = 0;

    virtual ResultType flush() = 0;

    virtual ResultType enable() = 0;

    virtual ResultType disable() = 0;
    
    virtual ResultType setLogLevel( const LogLevelType level ) = 0;

    virtual LogLevelType getLogLevel() = 0;

    /**
     *  Provides the core functionality of the sink by logging messages.
     *  
     *  @note   Assume the memory can be modified/destroyed after return
     *  
     *  @param[in]  level     The log level the message was sent at
     *  @param[in]  message   The message to be logged. Can be any kind of data
     *  @param[in]  length    How large the message is in bytes
     *  @return ResultType    Whether or not the logging action succeeded
     */
    virtual ResultType log( const LogLevelType level, const void *const message, const size_t length ) = 0;
  };

}

#endif  /* MICRO_LOGGER_SINK_INTERFACE_HPP */
