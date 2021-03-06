include("${COMMON_TOOL_ROOT}/cmake/utility/embedded.cmake")

# ====================================================
# Common
# ====================================================
set(LINK_LIBS
  chimera_intf_inc       # Chimera public headers
  aurora_intf_inc        # Aurora public headers
  ulog_inc
)

# ====================================================
# Public Include Target
# ====================================================
set(INC ulog_inc)
add_library(${INC} INTERFACE)
target_include_directories(${INC} INTERFACE ".")
export(TARGETS ${INC} FILE "${PROJECT_BINARY_DIR}/uLog/${INC}.cmake")

# ====================================================
# Driver Library
# ====================================================
function(build_library variant)
  set(LIB ulog_core${variant})
  add_library(${LIB} STATIC
    uLog/ulog.cpp
    uLog/sinks/sink_cout.cpp
    uLog/sinks/sink_intf.cpp
  )
  target_link_libraries(${LIB} PRIVATE ${LINK_LIBS} prj_build_target${variant} prj_device_target)
  export(TARGETS ${LIB} FILE "${PROJECT_BINARY_DIR}/uLog/${LIB}.cmake")
endfunction()

add_target_variants(build_library)
