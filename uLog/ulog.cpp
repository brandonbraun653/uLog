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
#include <atomic>
#include <cstdint>
#include <limits>

/* uLog Includes */
#include <uLog/config.hpp>
#include <uLog/sinks/sink_intf.hpp>
#include <uLog/types.hpp>
#include <uLog/ulog.hpp>

#define FLAG_LOCK( x ) ( !x.test_and_set( std::memory_order_acquire ) )
#define FLAG_RELEASE( x ) ( x.clear( std::memory_order_release ) )

namespace uLog
{
  static bool uLogInitialized = false;
  static std::atomic_flag threadLock = ATOMIC_FLAG_INIT;
  static LogLevelType globalLogLevel = LogLevelType::LOG_LEVEL_MIN;
  static std::array<SinkType, ULOG_MAX_REGISTERABLE_SINKS> sinkRegistry;

  /**
   *  Common entry point into the logging system for all log levels.
   *
   *  @param[in]  level     The level to be logged at
   *  @param[in]  message   The message to be logged
   *  @param[in]  length    How many bytes are contained in the message
   *  @return ResultType
   */
  static ResultType commonLogger( const LogLevelType level, const void *const message, const size_t length );

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

    if ( FLAG_LOCK( threadLock ) )
    {
      /*------------------------------------------------
      Find the first empty slot in the registry and assign the sink
      ------------------------------------------------*/
      for ( size_t i = 0; i < sinkRegistry.size(); i++ )
      {
        if ( ( sinkRegistry[ i ] == nullptr ) && ( sink->open() == ResultType::RESULT_SUCCESS ) )
        {
          sinkRegistry[ i ] = sink;
          temp              = &sinkRegistry[ i ];
          break;
        }
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

  ResultType trace( const void *const message, const size_t length )
  {
    return commonLogger( LogLevelType::LOG_LEVEL_TRACE, message, length );
  }

  ResultType debug( const void *const message, const size_t length )
  {
    return commonLogger( LogLevelType::LOG_LEVEL_DEBUG, message, length );
  }

  ResultType info( const void *const message, const size_t length )
  {
    return commonLogger( LogLevelType::LOG_LEVEL_INFO, message, length );
  }

  ResultType warn( const void *const message, const size_t length )
  {
    return commonLogger( LogLevelType::LOG_LEVEL_WARN, message, length );
  }

  ResultType error( const void *const message, const size_t length )
  {
    return commonLogger( LogLevelType::LOG_LEVEL_ERROR, message, length );
  }

  ResultType fatal( const void *const message, const size_t length )
  {
    return commonLogger( LogLevelType::LOG_LEVEL_FATAL, message, length );
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

  ResultType commonLogger( const LogLevelType level, const void *const message, const size_t length )
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