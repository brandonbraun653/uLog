/********************************************************************************
 *  File Name:
 *    ulog.cpp
 *
 *  Description:
 *    uLog implementation
 *
 *  2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

/* C++ Includes */
#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <stdarg.h>

/* uLog Includes */
#include <uLog/config.hpp>
#include <uLog/sinks/sink_intf.hpp>
#include <uLog/types.hpp>
#include <uLog/ulog.hpp>

#if defined( WIN32 ) || defined( WIN64 )
std::atomic_flag threadLock = ATOMIC_FLAG_INIT;
#elif defined( USING_FREERTOS )
SemaphoreHandle_t threadLock = xSemaphoreCreateRecursiveMutex();
#endif

namespace uLog
{
  static bool uLogInitialized = false;
  static LogLevelType globalLogLevel = LogLevelType::LOG_LEVEL_MIN;
  static std::array<SinkType, ULOG_MAX_REGISTERABLE_SINKS> sinkRegistry;

  std::array<char, ULOG_MAX_SNPRINTF_BUFFER_LENGTH> printfBuffer;

  /**
   *  Looks up the registry index associated with a particular sink handle
   *
   *  @param[in]  sinkHandle  The handle to search for
   *  @return size_t
   */
  static size_t getSinkOffsetIndex( const SinkHandleType sinkHandle );

  void initialize()
  {
    if ( !uLogInitialized && FLAG_LOCK( threadLock ) )
    {
      sinkRegistry.fill( nullptr );
      uLogInitialized = true;
      FLAG_RELEASE( threadLock );
    }
  }

  ResultType setGlobalLogLevel( const LogLevelType level )
  {
    if ( FLAG_LOCK( threadLock ) )
    {
      globalLogLevel = level;
      FLAG_RELEASE( threadLock );
      
      return ResultType::RESULT_SUCCESS;
    }

    return ResultType::RESULT_FAIL;
  }

  SinkHandleType registerSink( SinkType &sink )
  {
    SinkHandleType temp = nullptr;
    constexpr size_t invalidIndex = std::numeric_limits<size_t>::max();

    size_t nullIndex      = invalidIndex; /* First index that doesn't have a sink registered */
    bool sinkIsRegistered = false;        /* Indicates if the sink we are registering already exists */


    if ( FLAG_LOCK( threadLock ) )
    {
      /*------------------------------------------------
      Check if the sink already is registered as well as
      an empty slot to insert the new sink.
      ------------------------------------------------*/
      for ( size_t i = 0; i < sinkRegistry.size(); i++ )
      {
        /* Did we find the first location that is free? */
        if ( ( nullIndex == invalidIndex ) && ( sinkRegistry[ i ] == nullptr ) )
        {
          nullIndex = i;
        }

        /* Does the sink already exist in the registry? */
        if ( sinkRegistry[ i ] == sink )
        {
          sinkIsRegistered = true;
          break;
        }
      }

      /*------------------------------------------------
      Perform the registration
      ------------------------------------------------*/
      if ( !sinkIsRegistered && ( sink->open() == ResultType::RESULT_SUCCESS ) )
      {
        /* OOB access protected by the for() loop above */
        sinkRegistry[ nullIndex ] = sink;
        temp                      = &sinkRegistry[ nullIndex ];
      }

      FLAG_RELEASE( threadLock );
    }

    return temp;
  }

  ResultType removeSink( SinkHandleType sink )
  {
    ResultType result = ResultType::RESULT_FAIL;

    if ( FLAG_LOCK( threadLock ) )
    {
      auto index = getSinkOffsetIndex( sink );
      if ( index < sinkRegistry.size() )
      {
        result = sinkRegistry[ index ]->close();

        sinkRegistry[ index ].reset();
        sinkRegistry[ index ] = nullptr;
      }
      else if ( sink == nullptr )
      {
        for ( size_t i = 0; i < sinkRegistry.size(); i++ )
        {
          if ( sinkRegistry[ i ] )
          {
            sinkRegistry[ i ]->close();
            sinkRegistry[ i ].reset();
            sinkRegistry[ i ] = nullptr;
          }
        }

        result = ResultType::RESULT_SUCCESS;
      }

      FLAG_RELEASE( threadLock );
    }
    else
    {
      result = ResultType::RESULT_LOCKED;
    }

    return result;
  }

  ResultType enableSink( SinkHandleType sink )
  {
    ResultType result = ResultType::RESULT_FAIL;

    if ( FLAG_LOCK( threadLock ) )
    {
      auto index = getSinkOffsetIndex( sink );
      if ( index < sinkRegistry.size() )
      {
        result = sinkRegistry[ index ]->enable();
      }
      else if ( sink == nullptr )
      {
        for ( size_t i = 0; i < sinkRegistry.size(); i++ )
        {
          if (sinkRegistry[ i ])
          {
            sinkRegistry[ i ]->enable();
          }
          
        }

        result = ResultType::RESULT_SUCCESS;
      }

      FLAG_RELEASE( threadLock );
    }
    else
    {
      result = ResultType::RESULT_LOCKED;
    }

    return result;
  }

  ResultType disableSink( SinkHandleType sink )
  {
    ResultType result = ResultType::RESULT_FAIL;

    if ( FLAG_LOCK( threadLock ) )
    {
      auto index = getSinkOffsetIndex( sink );
      if ( index < sinkRegistry.size() )
      {
        result = sinkRegistry[ index ]->disable();
      }
      else if ( sink == nullptr )
      {
        for ( size_t i = 0; i < sinkRegistry.size(); i++ )
        {
          if ( sinkRegistry[ i ] )
          {
          	sinkRegistry[ i ]->disable();
          }
        }

        result = ResultType::RESULT_SUCCESS;
      }

      FLAG_RELEASE( threadLock );
    }
    else
    {
      result = ResultType::RESULT_LOCKED;
    }

    return result;
  }

  ResultType flushSink( SinkHandleType sink )
  {
    ResultType result = ResultType::RESULT_FAIL;

    if ( FLAG_LOCK( threadLock ) )
    {
      auto index = getSinkOffsetIndex( sink );
      if ( index < sinkRegistry.size() )
      {
        result = sinkRegistry[ index ]->flush();
      }
      else if ( sink == nullptr )
      {
        for ( size_t i = 0; i < sinkRegistry.size(); i++ )
        {
          if (sinkRegistry[ i ])
          {
            sinkRegistry[ i ]->flush();
          }
        }

        result = ResultType::RESULT_SUCCESS;
      }

      FLAG_RELEASE( threadLock );
    }
    else
    {
      result = ResultType::RESULT_LOCKED;
    }

    return result;
  }

  size_t getSinkOffsetIndex( const SinkHandleType sinkHandle )
  {
    /*------------------------------------------------
    Figure out the real addresses and boundary limit them
    ------------------------------------------------*/
    std::uintptr_t offsetAddress = reinterpret_cast<std::uintptr_t>( sinkHandle );
    std::uintptr_t beginAddress  = reinterpret_cast<std::uintptr_t>( &sinkRegistry[ 0 ] );

    if ( ( sinkHandle == nullptr ) || ( beginAddress > offsetAddress ) )
    {
      return std::numeric_limits<size_t>::max();
    }

    /*------------------------------------------------
    Calculate the index
    ------------------------------------------------*/
    size_t index = ( offsetAddress - beginAddress ) / sizeof( SinkType );

    if ( index > sinkRegistry.size() )
    {
      index = std::numeric_limits<size_t>::max();
    }

    return index;
  }

  ResultType log( const LogLevelType level, const void *const message, const size_t length )
  {
    /*------------------------------------------------
    Input boundary checking
    ------------------------------------------------*/
    if ( !FLAG_LOCK( threadLock ) )
    {
      return ResultType::RESULT_LOCKED;
    }
    else if ( ( level < globalLogLevel ) || !message || !length )
    {
      FLAG_RELEASE( threadLock );
      return ResultType::RESULT_FAIL;
    }

    /*------------------------------------------------
    Process the message through each sink. At the moment
    we won't concern ourselves if a sink failed to log.
    ------------------------------------------------*/
    for ( size_t i = 0; i < sinkRegistry.size(); i++ )
    {
      if ( sinkRegistry[ i ] && ( sinkRegistry[ i ]->getLogLevel() >= globalLogLevel ) )
      {
        sinkRegistry[ i ]->log( level, message, length );
      }
    }

    FLAG_RELEASE( threadLock );
    return ResultType::RESULT_SUCCESS;
  }


}    // namespace uLog