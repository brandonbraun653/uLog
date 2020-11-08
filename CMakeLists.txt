# ====================================================
# Common
# ====================================================
set(LINK_LIBS
  chimera_inc       # Chimera public headers
  aurora_inc        # Aurora public headers
  aurora_build_options
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
set(LIB ulog_core)
add_library(${LIB} STATIC
  uLog/ulog.cpp
  uLog/sinks/sink_cout.cpp
)
target_link_libraries(${LIB} PRIVATE ${LINK_LIBS} ${INC})
export(TARGETS ${LIB} FILE "${PROJECT_BINARY_DIR}/uLog/${LIB}.cmake")