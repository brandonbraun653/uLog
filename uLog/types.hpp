/********************************************************************************
 *  File Name:
 *    types.hpp
 *
 *  Description:
 *    uLog types 
 *
 *  2019 | Brandon Braun | brandonbraun653@gmail.com
 ********************************************************************************/

#pragma once
#ifndef MICRO_LOGGER_TYPES_HPP
#define MICRO_LOGGER_TYPES_HPP

/* C++ Includes */
#include <cstdint>
#include <memory>

namespace uLog
{
  enum class Result : size_t 
  {
    RESULT_SUCCESS,
    RESULT_FAIL,
    RESULT_FAIL_MSG_TOO_LONG,
    RESULT_LOCKED,
    RESULT_FULL,
    RESULT_INVALID_LEVEL
  };

  /**
   *  The supported logging level types for all log sinks. An increasing numerical
   *  value corresponds with an increasing priority.
   */
  enum class Level : size_t
  {
    LVL_TRACE,
    LVL_DEBUG,
    LVL_INFO,
    LVL_WARN,
    LVL_ERROR,
    LVL_FATAL,

    LVL_MIN = LVL_TRACE,
    LVL_MAX = LVL_FATAL
  };

  class SinkInterface;

  using SinkHandleType = void *;
  using SinkType = std::shared_ptr<SinkInterface>;
}

#endif  /* MICRO_LOGGER_TYPES_HPP */