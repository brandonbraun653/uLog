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
#include <mutex>
#include <string>

/* Chimera Includes */
#include <Chimera/threading.hpp>
#include <Chimera/types/threading_types.hpp>

/* uLog Includes */
#include <uLog/config.hpp>
#include <uLog/sinks/sink_intf.hpp>
#include <uLog/types.hpp>
#include <uLog/ulog.hpp>


namespace uLog
{
  static bool uLogInitialized      = false;
  static Level globalLogLevel      = Level::LVL_MIN;
  static SinkHandle globalRootSink = nullptr;
  static std::array<SinkHandle, ULOG_MAX_REGISTERABLE_SINKS> sinkRegistry;

  static size_t defaultLockTimeout = 100;
  static Chimera::Threading::RecursiveTimedMutex threadLock = nullptr;

  /**
   *  Looks up the registry index associated with a particular sink handle
   *
   *  @param[in]  sinkHandle  The handle to search for
   *  @return size_t
   */
  static size_t getSinkOffsetIndex( const SinkHandle &sinkHandle );

  void initialize()
  {
    using namespace Chimera::Threading;

    if ( threadLock == nullptr )
    {
      threadLock = createSyncObject<RecursiveTimedMutex>();
    }

    if ( !uLogInitialized && lock( threadLock, defaultLockTimeout ) )
    {
      sinkRegistry.fill( nullptr );
      uLogInitialized = true;
      unlock( threadLock );
    }
  }

  Result setGlobalLogLevel( const Level level )
  {
    if ( Chimera::Threading::lock( threadLock, defaultLockTimeout ) )
    {
      globalLogLevel = level;
      Chimera::Threading::unlock( threadLock );

      return Result::RESULT_SUCCESS;
    }

    return Result::RESULT_FAIL;
  }

  Result registerSink( SinkHandle &sink, const uLog::Config options )
  {
    constexpr size_t invalidIndex = std::numeric_limits<size_t>::max();

    size_t nullIndex      = invalidIndex;           /* First index that doesn't have a sink registered */
    bool sinkIsRegistered = false;                  /* Indicates if the sink we are registering already exists */
    bool registryIsFull   = true;                   /* Is the registry full of sinks? */
    auto result           = Result::RESULT_SUCCESS; /* Function return code */

    if ( Chimera::Threading::lock( threadLock, defaultLockTimeout ) )
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
          registryIsFull = false;
        }

        /* Does the sink already exist in the registry? */
        if ( sinkRegistry[ i ] == sink )
        {
          sinkIsRegistered = true;
          registryIsFull = false;
          result = Result::RESULT_SUCCESS;
          break;
        }
      }

      /*------------------------------------------------
      Perform the registration
      ------------------------------------------------*/
      if ( !sinkIsRegistered )
      {
        if ( registryIsFull )
        {
          result = Result::RESULT_FULL;
        }
        else if ( sink->open() != Result::RESULT_SUCCESS )
        {
          result = Result::RESULT_FAIL;
        }
        else
        {
          sinkRegistry[ nullIndex ] = sink;
        }
      }

      Chimera::Threading::unlock( threadLock );
    }

    return result;
  }

  Result removeSink( SinkHandle &sink )
  {
    Result result = Result::RESULT_LOCKED;

    if ( Chimera::Threading::lock( threadLock, defaultLockTimeout ) )
    {
      auto index = getSinkOffsetIndex( sink );
      if ( index < sinkRegistry.size() )
      {
        result = sinkRegistry[ index ]->close();

        sinkRegistry[ index ].reset();
        sinkRegistry[ index ] = nullptr;
        result = Result::RESULT_SUCCESS;
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

        result = Result::RESULT_SUCCESS;
      }

      Chimera::Threading::unlock( threadLock );
    }

    return result;
  }

  Result setRootSink( SinkHandle &sink )
  {
    Result result = Result::RESULT_LOCKED;

    if ( Chimera::Threading::lock( threadLock, defaultLockTimeout ) )
    {
      globalRootSink = sink;
      Chimera::Threading::unlock( threadLock );
      result = Result::RESULT_SUCCESS;
    }

    return result;
  }

  SinkHandle getRootSink()
  {
    return globalRootSink;
  }

  size_t getSinkOffsetIndex( const SinkHandle &sinkHandle )
  {
    /*------------------------------------------------
    Figure out the real addresses and boundary limit them

    Note: I'm going to run into trouble if packing is weird...
    ------------------------------------------------*/
    std::uintptr_t offsetAddress = reinterpret_cast<std::uintptr_t>( &sinkHandle );
    std::uintptr_t beginAddress  = reinterpret_cast<std::uintptr_t>( &sinkRegistry[ 0 ] );
    std::uintptr_t secondAddress = reinterpret_cast<std::uintptr_t>( &sinkRegistry[ 1 ] );
    size_t elementSize = secondAddress - beginAddress;

    if ( ( sinkHandle == nullptr ) || ( beginAddress > offsetAddress ) || !elementSize )
    {
      return std::numeric_limits<size_t>::max();
    }

    /*------------------------------------------------
    Calculate the index
    ------------------------------------------------*/
    size_t index = ( offsetAddress - beginAddress ) / elementSize;

    if ( index > sinkRegistry.size() )
    {
      index = std::numeric_limits<size_t>::max();
    }

    return index;
  }

  Result log( const Level level, const void *const message, const size_t length )
  {
    /*------------------------------------------------
    Input boundary checking
    ------------------------------------------------*/
    if ( !Chimera::Threading::lock( threadLock, defaultLockTimeout ) )
    {
      return Result::RESULT_LOCKED;
    }
    else if ( ( level < globalLogLevel ) || !message || !length )
    {
      Chimera::Threading::unlock( threadLock );
      return Result::RESULT_FAIL;
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

    Chimera::Threading::unlock( threadLock );
    return Result::RESULT_SUCCESS;
  }

}    // namespace uLog