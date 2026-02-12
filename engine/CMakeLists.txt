add_library(engine)

target_sources(engine
  PRIVATE
    src/core/logger.cpp
    src/input/input_state.cpp
    src/time/frame_timer.cpp
    src/engine.cpp
)

target_include_directories(engine
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(engine PUBLIC cxx_std_20)

target_compile_options(engine
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(engine PRIVATE fmt::fmt)
target_compile_definitions(engine PRIVATE ENGINE_HAS_FMT=1)
