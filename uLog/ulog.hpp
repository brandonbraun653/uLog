/********************************************************************************
 *  File Name:
 *    ulog.hpp
 *
 *  Description:
 *    uLog attempts to be a fairly simple logger for embedded systems
 *
 *  2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

#pragma once
#ifndef MICRO_LOGGER_HPP
#define MICRO_LOGGER_HPP

/* C++ Includes */
#include <cstdlib>

/* uLog Includes */
#include <uLog/types.hpp>

namespace uLog
{  
  /**
   *  Initializes the backend driver
   *  
   *  @return void
   */
  void initialize();

  /**
   *  Sets a minimum log level that is needed to emit messages 
   *  to registered sinks
   *  
   *  @param[in]  level      The global log level to be set
   *  @return SinkHandleType
   */
  ResultType setGlobalLogLevel( const LogLevelType level );

  /**
   *  Registers a sink with the back end driver 
   *  
   *  @param[in]  sink      The sink to be registered
   *  @return SinkHandleType
   */
  SinkHandleType registerSink( SinkType &sink );

  /**
   *  Removes the associated sink. 
   *  
   *  @note If nullptr is passed in, all sinks are removed.
   *  
   *  @param[in]  sink      The sink that should be removed
   *  @return ResultType
   */
  ResultType removeSink( SinkHandleType sink );

  /**
   *  Enables the associated sink. 
   *  
   *  @note If nullptr is passed in, all sinks are enabled.
   *  
   *  @param[in]  sink      The sink that should be enabled
   *  @return ResultType
   */
  ResultType enableSink( SinkHandleType sink );

  /**
   *  Disables the associated sink from being able to log
   *  
   *  @note If nullptr is passed in, all sinks are disabled.
   *  
   *  @param[in]  sink      The sink that should be disabled
   *  @return ResultType
   */
  ResultType disableSink( SinkHandleType sink );

  /**
   *  Flushes the associated sink. 
   *  
   *  @note If nullptr is passed in, all sinks are flushed.
   *  
   *  @param[in]  sink      The sink that should be flushed
   *  @return ResultType
   */
  ResultType flushSink( SinkHandleType sink );

  /**
   *  Logs a trace level message to all registered sinks 
   *  
   *  @param[in]  message   The message to be logged
   *  @param[in]  length    How long the message is in bytes
   *  @return ResultType
   */
  ResultType trace( const void *const message, const size_t length );
  
  /**
   *  Logs a debug level message to all registered sinks 
   *  
   *  @param[in]  message   The message to be logged
   *  @param[in]  length    How long the message is in bytes
   *  @return ResultType
   */
  ResultType debug( const void *const message, const size_t length );
  
  /**
   *  Logs a info level message to all registered sinks 
   *  
   *  @param[in]  message   The message to be logged
   *  @param[in]  length    How long the message is in bytes
   *  @return ResultType
   */
  ResultType info( const void *const message, const size_t length );
  
  /**
   *  Logs a warn level message to all registered sinks 
   *  
   *  @param[in]  message   The message to be logged
   *  @param[in]  length    How long the message is in bytes
   *  @return ResultType
   */
  ResultType warn( const void *const message, const size_t length );
  
  /**
   *  Logs a error level message to all registered sinks 
   *  
   *  @param[in]  message   The message to be logged
   *  @param[in]  length    How long the message is in bytes
   *  @return ResultType
   */
  ResultType error( const void *const message, const size_t length );
  
  /**
   *  Logs a fatal level message to all registered sinks 
   *  
   *  @param[in]  message   The message to be logged
   *  @param[in]  length    How long the message is in bytes
   *  @return ResultType
   */
  ResultType fatal( const void *const message, const size_t length );
}

#endif  /* MICRO_LOGGER_HPP */
