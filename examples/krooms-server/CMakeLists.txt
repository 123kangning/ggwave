cmake_minimum_required(VERSION 3.0...10.0)

project(krooms-server)


add_executable(krooms-server
        main.cpp
)

target_include_directories(krooms-server PRIVATE
    ..
    ${SDL2_INCLUDE_DIRS}
    )

target_link_libraries(krooms-server PRIVATE
    ggwave
    ggwave-common
    ggwave-common-sdl2
    ${CMAKE_THREAD_LIBS_INIT}
    )
