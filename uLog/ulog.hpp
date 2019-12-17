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
#include <array>
#include <cstdlib>
#include <cstring>
#include <string>

/* uLog Includes */
#include <uLog/config.hpp>
#include <uLog/types.hpp>

/*------------------------------------------------
TODO: Unpolute the namespace
------------------------------------------------*/

#if defined( WIN32 ) || defined( WIN64 )
#include <mutex>
#define FLAG_LOCK( x ) ( x.try_lock() )
#define FLAG_RELEASE( x ) ( x.unlock() )

extern std::recursive_mutex threadLock;

#elif defined( USING_FREERTOS )
#include "FreeRTOS.h"
#include "semphr.h"

#define FLAG_LOCK( x ) ( ( xSemaphoreTakeRecursive( x, portMAX_DELAY ) == pdPASS ) )
#define FLAG_RELEASE( x ) ( ( xSemaphoreGiveRecursive( x ) == pdPASS ) )

extern SemaphoreHandle_t threadLock;

#endif

namespace uLog
{
  extern std::array<char, ULOG_MAX_SNPRINTF_BUFFER_LENGTH> printfBuffer;

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
  Result setGlobalLogLevel( const Level level );

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
  Result removeSink( SinkHandleType sink );

  /**
   *  Enables the associated sink. 
   *  
   *  @note If nullptr is passed in, all sinks are enabled.
   *  
   *  @param[in]  sink      The sink that should be enabled
   *  @return ResultType
   */
  Result enableSink( SinkHandleType sink );

  /**
   *  Disables the associated sink from being able to log
   *  
   *  @note If nullptr is passed in, all sinks are disabled.
   *  
   *  @param[in]  sink      The sink that should be disabled
   *  @return ResultType
   */
  Result disableSink( SinkHandleType sink );

  /**
   *  Flushes the associated sink. 
   *  
   *  @note If nullptr is passed in, all sinks are flushed.
   *  
   *  @param[in]  sink      The sink that should be flushed
   *  @return ResultType
   */
  Result flushSink( SinkHandleType sink );

  Result log( const Level lvl, const void *const msg, const size_t length );

  template<typename... Args>
  Result flog( const Level lvl, const char *str, Args const &... args )
  {
    auto result = Result::RESULT_SUCCESS;

    if ( FLAG_LOCK( threadLock ) )
    {
      snprintf( printfBuffer.data(), printfBuffer.size(), str, args... );
      result = log( lvl, printfBuffer.data(), strlen( printfBuffer.data() ) );
      FLAG_RELEASE( threadLock );
    }

    return result;
  }
}

#endif  /* MICRO_LOGGER_HPP */
