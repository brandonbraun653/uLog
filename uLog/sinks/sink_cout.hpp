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
  {public:
    CoutSink();
    ~CoutSink();

    Result open() final override;

    Result close() final override;

    Result flush() final override;

    IOType getIOType() final override;

    Result log( const Level level, const void *const message, const size_t length ) final override;
  };
}    // namespace uLog

#endif    // !MICRO_LOGGER_SINK_COUT_HPP
