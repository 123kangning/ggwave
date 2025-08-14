cmake_minimum_required(VERSION 3.0...10.0)

set(TARGET waver)

if (EMSCRIPTEN)
    add_executable(${TARGET} main.cpp common.cpp interface.cpp interface-emscripten.cpp)

    target_include_directories(${TARGET} PRIVATE
        ..
        ${SDL2_INCLUDE_DIRS}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}/
        )

    target_link_libraries(${TARGET} PRIVATE
        ggwave
        ggwave-common
        ggwave-common-sdl2
        ggsock
        imgui-sdl2
        ${CMAKE_THREAD_LIBS_INIT}
        )

    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS " \
        -s FORCE_FILESYSTEM=1 \
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/../assets/fonts@/ \
        ")

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/build_timestamp-tmpl.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}/build_timestamp.h @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index-tmpl.html ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}/index.html @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/style.css ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}/style.css COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/background-0.png ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}/background-0.png COPYONLY)
else()
    add_executable(${TARGET} main.cpp common.cpp interface.cpp interface-unix.cpp)

    if(WIN32 AND GGWAVE_SUPPORT_SDL2)
        # 获取编译器路径
        get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)

        # 需要复制的运行时库列表
        set(MINGW_RUNTIME_DLLS
                libgcc_s_seh-1.dll
                libstdc++-6.dll
                libwinpthread-1.dll
        )

        # 复制SDL2.dll
        get_target_property(SDL2_LIB_PATH SDL2::SDL2 LOCATION)
        get_filename_component(SDL2_DLL_DIR "${SDL2_LIB_PATH}" DIRECTORY)
        add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${SDL2_DLL_DIR}/SDL2.dll"
                "$<TARGET_FILE_DIR:${TARGET}>"
                COMMENT "Copying SDL2.dll"
        )

        # 复制MinGW运行时DLL
        foreach(DLL_NAME IN LISTS MINGW_RUNTIME_DLLS)
            add_custom_command(TARGET ${TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${MINGW_BIN_DIR}/${DLL_NAME}"
                    "$<TARGET_FILE_DIR:${TARGET}>"
                    COMMENT "Copying ${DLL_NAME}"
            )
        endforeach()
    endif()

    target_include_directories(${TARGET} PRIVATE
        ..
        ${SDL2_INCLUDE_DIRS}
        )

    target_link_libraries(${TARGET} PRIVATE
        ggwave
        ggwave-common
        ggwave-common-sdl2
        ggsock
        imgui-sdl2
        ${CMAKE_THREAD_LIBS_INIT}
        )

    install(FILES ${PROJECT_SOURCE_DIR}/examples/assets/fonts/DroidSans.ttf DESTINATION bin)
    install(FILES ${PROJECT_SOURCE_DIR}/examples/assets/fonts/fontawesome-webfont.ttf DESTINATION bin)
endif()

install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
