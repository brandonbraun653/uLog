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

namespace uLog
{
  enum class ResultType : size_t 
  {
    RESULT_SUCCESS,
    RESULT_FAIL,
    RESULT_LOCKED,
    RESULT_FULL    
  };

  enum class LogLevelType : size_t
  {
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,

    LOG_LEVEL_MIN = LOG_LEVEL_TRACE,
    LOG_LEVEL_MAX = LOG_LEVEL_FATAL
  };

  class SinkInterface;

  using SinkHandleType = void *;
  using SinkType = std::shared_ptr<SinkInterface>;
}

#endif  /* MICRO_LOGGER_TYPES_HPP */