add_library(engine)

target_sources(engine
  PRIVATE
    src/assets/asset_manager.cpp
    src/assets/gltf_loader.cpp
    src/assets/mesh_data.cpp
    src/core/logger.cpp
    src/input/input_state.cpp
    src/renderer/basic_renderer.cpp
    src/runtime/camera.cpp
    src/runtime/scene.cpp
    src/runtime/transform.cpp
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

find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
  find_package(SDL2 REQUIRED)
endif()
target_link_libraries(engine PRIVATE SDL2::SDL2)

if(ENGINE_ENABLE_BGFX)
  find_package(bgfx CONFIG QUIET)
  if(TARGET bgfx::bgfx)
    target_link_libraries(engine PRIVATE bgfx::bgfx)
    target_compile_definitions(engine PUBLIC ENGINE_HAS_BGFX=1)
  elseif(TARGET bgfx)
    target_link_libraries(engine PRIVATE bgfx)
    target_compile_definitions(engine PUBLIC ENGINE_HAS_BGFX=1)
  endif()
endif()
