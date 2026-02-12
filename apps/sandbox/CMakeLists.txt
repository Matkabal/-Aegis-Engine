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
