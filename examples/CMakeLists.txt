# dependencies
cmake_minimum_required(VERSION 3.0...10.0)

find_package(Threads REQUIRED)

if (GGWAVE_SUPPORT_SDL2)
    # SDL2

    if (EMSCRIPTEN)
        set(CMAKE_CXX_FLAGS " \
        -s USE_SDL=2 \
        -s DISABLE_EXCEPTION_CATCHING=0 \
        ")

        set(CMAKE_CXX_LINK_FLAGS " \
        --bind \
        -s TOTAL_MEMORY=67108864 \
        -s ASSERTIONS=1 \
        -s 'EXPORTED_RUNTIME_METHODS=[\"writeArrayToMemory\"]' \
        ")

        unset(SDL2_INCLUDE_DIRS)
        unset(SDL2_LIBRARIES)
    endif()

    if (NOT EMSCRIPTEN)
        # 判断是否为 Windows 平台
        if(WIN32)
            # 设置查找 SDL2 静态库
            set(SDL2_STATIC ON CACHE BOOL "Force static SDL2 libraries" FORCE)
            set(SDL2_SHARED OFF CACHE BOOL "Disable shared SDL2 libraries" FORCE)
        endif()

        find_package(SDL2)

        if (NOT USE_FINDSDL2 AND NOT SDL2_FOUND)
            message(WARNING "Unable to find SDL2 library. It is either not installed or CMake cannot find it."
                " In the latter case, setting the USE_FINDSDL2 variable might help:\n"
                "   $ cmake -D USE_FINDSDL2 .."
                )

            message(FATAL_ERROR "Aborting")
        endif()

        string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

        message(STATUS "SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")
        message(STATUS "SDL2_LIBRARIES = ${SDL2_LIBRARIES}")
    endif()
endif()

# helper libraries

add_library(ggwave-common
    ggwave-common.cpp
    )

target_link_libraries(ggwave-common PRIVATE
    ${CMAKE_DL_LIBS}
    )

if (MINGW)
    target_link_libraries(ggwave-common PUBLIC
        stdc++
    )
endif()

if (GGWAVE_SUPPORT_SDL2)
    # ggwave-common-sdl2

    add_library(ggwave-common-sdl2
        ggwave-common-sdl2.cpp
        )

    target_include_directories(ggwave-common-sdl2 PUBLIC
        ${SDL2_INCLUDE_DIRS}
        )

    target_link_libraries(ggwave-common-sdl2 PRIVATE
        ggwave
        ${SDL2_LIBRARIES}
        )
endif()

# examples

if (GGWAVE_SUPPORT_SDL2)
    if (EMSCRIPTEN)
        # emscripten sdl2 examples
    else()
        # non-emscripten sdl2 examples

        add_subdirectory(krooms-send)
        add_subdirectory(krooms-receive)
    endif()
endif()

install(TARGETS ggwave-common
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    )

if (GGWAVE_SUPPORT_SDL2)
    install(TARGETS ggwave-common-sdl2
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        )
endif()
