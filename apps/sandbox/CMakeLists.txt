add_executable(sandbox
  main.cpp
)

find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
  find_package(SDL2 REQUIRED)
endif()

target_link_libraries(sandbox PRIVATE engine SDL2::SDL2)
if(TARGET SDL2::SDL2main)
  target_link_libraries(sandbox PRIVATE SDL2::SDL2main)
endif()

if(ENGINE_ENABLE_BGFX)
  find_package(bgfx CONFIG QUIET)

  set(ENGINE_BGFX_TARGET "")
  if(TARGET bgfx::bgfx)
    set(ENGINE_BGFX_TARGET bgfx::bgfx)
  elseif(TARGET bgfx)
    set(ENGINE_BGFX_TARGET bgfx)
  endif()

  if(ENGINE_BGFX_TARGET)
    target_link_libraries(sandbox PRIVATE ${ENGINE_BGFX_TARGET})
    target_compile_definitions(sandbox PRIVATE ENGINE_HAS_BGFX=1)
    message(STATUS "bgfx enabled for sandbox")
  else()
    message(WARNING "ENGINE_ENABLE_BGFX=ON but bgfx was not found. Sandbox will run without bgfx.")
  endif()
endif()
