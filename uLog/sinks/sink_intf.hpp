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

    virtual Result open() = 0;

    virtual Result close() = 0;

    virtual Result flush() = 0;

    virtual Result enable() = 0;

    virtual Result disable() = 0;
    
    /**
     *  Sets the minimum log level threshold. This level, plus any higher priority 
     *  levels, will be logged with the sink. 
     *
     *  @code
     *    // This will log every message that arrives
     *    setLogLevel( LogLevelType::TRACE );
     *
     *    // This will log only fatal messages (highest priority)
     *    setLogLevel( LogLevelType::FATAL );
     *
     *    // This will log INFO, WARN, ERROR, & FATAL messages
     *    setLogLevel( LogLevelType::INFO );
     *  @endcode
     *
     *  @param[in]  level   The minimum log level for this sink
     *  @return ResultType
     */
    virtual Result setLogLevel( const Level level ) = 0;

    virtual Level getLogLevel() = 0;

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
    virtual Result log( const Level level, const void *const message, const size_t length ) = 0;
  };

}

#endif  /* MICRO_LOGGER_SINK_INTERFACE_HPP */
