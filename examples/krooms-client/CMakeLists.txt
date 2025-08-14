cmake_minimum_required(VERSION 3.0...10.0)

project(krooms-client)


add_executable(krooms-client
        main.cpp
)

target_include_directories(krooms-client PRIVATE
    ..
    ${SDL2_INCLUDE_DIRS}
    )

target_link_libraries(krooms-client PRIVATE
    ggwave
    ggwave-common
    ggwave-common-sdl2
    ${CMAKE_THREAD_LIBS_INIT}
    )
