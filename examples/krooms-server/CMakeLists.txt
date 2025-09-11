cmake_minimum_required(VERSION 3.0...10.0)
# cmake_policy(SET CMP0042 NEW)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(krooms-server)

# find_package(NodeJS REQUIRED)



# Include Node-API wrappers
# execute_process(
#     COMMAND node -p "require('node-addon-api').include"
#     OUTPUT_VARIABLE NODE_ADDON_API_DIR
#     OUTPUT_STRIP_TRAILING_WHITESPACE
# )
# 方式1：通过环境变量指定 Node 路径（推荐，避免多版本冲突）
if(NOT DEFINED NODE_EXECUTABLE)
    set(NODE_EXECUTABLE "node") # 若 node 在环境变量中，直接用 "node"
    # 若不在环境变量，手动指定路径，例如：
    # set(NODE_EXECUTABLE "/usr/local/bin/node")
endif()
# 调用 Node 执行脚本，获取 Node-API 头文件目录和库路径

# 先获取项目根目录（含 node_modules 的目录）
execute_process(
    COMMAND npm prefix
    OUTPUT_VARIABLE PROJECT_ROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 拼接 node-addon-api 路径（假设安装在项目的 node_modules 中）
set(NODE_ADDON_API_DIR "${PROJECT_ROOT}/node_modules/node-addon-api")

# 新增：手动指定 node_api.h 所在的路径（关键）
set(NODE_INCLUDE_DIR "/usr/local/include/node")

# 验证路径是否存在（可选，增强健壮性）
if(NOT EXISTS "${NODE_INCLUDE_DIR}/node_api.h")
    message(FATAL_ERROR "错误：未找到 node_api.h，请检查路径是否正确：${NODE_INCLUDE_DIR}")
endif()

execute_process(
    COMMAND ${NODE_PATH} -p "process.platform"
    OUTPUT_VARIABLE NODE_PLATFORM
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 2. 配置子项目源文件（原有代码 + Node-API 包装代码）
set(SOURCES
    src/krooms_server_wrap.cc # Node-API 包装代码
    src/krooms_server.cc # 核心 C++ 逻辑
)
# 3. 生成 Node 扩展（本质是共享库，强制 .node 扩展名）
# --------------------------
add_library(krooms-server-lib SHARED ${SOURCES})
set_target_properties(krooms-server-lib PROPERTIES
    PREFIX "" # 移除 lib 前缀
    SUFFIX ".node" # 强制 .node 扩展名（Node 要求）
    CXX_STANDARD 17 # 匹配 Node-API 要求的 C++ 标准
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib # 设置输出目录到 build/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib # 设置运行时输出目录
)
# 4. 获取 Node.js 的库路径和链接标志
execute_process(
    COMMAND ${NODE_EXECUTABLE} -p "process.execPath"
    OUTPUT_VARIABLE NODE_EXECUTABLE_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 获取 Node.js 版本信息
execute_process(
    COMMAND ${NODE_EXECUTABLE} -p "process.versions.node"
    OUTPUT_VARIABLE NODE_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 设置 Node.js 相关的编译定义
add_definitions(-DNAPI_VERSION=8)
add_definitions(-DNODE_GYP_MODULE_NAME=krooms-server-lib)
add_definitions(-DUSING_UV_SHARED=1)
add_definitions(-DUSING_V8_SHARED=1)
add_definitions(-DV8_DEPRECATION_WARNINGS=1)
add_definitions(-DV8_DEPRECATION_WARNINGS)
add_definitions(-DV8_IMMINENT_DEPRECATION_WARNINGS)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

# 链接 Node-API 和子项目依赖
target_include_directories(krooms-server-lib
    PRIVATE ${NODE_ADDON_API_DIR} # 用于 napi.h
    PRIVATE ${NODE_INCLUDE_DIR} # 用于 node_api.h
    PRIVATE src # 子项目原有头文件目录
    PRIVATE ..
    PRIVATE ${SDL2_INCLUDE_DIRS}
)

# 设置 macOS 特定的链接选项
if(APPLE)
    set_target_properties(krooms-server-lib PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

message(STATUS "NODE_ADDON_API_DIR: ${NODE_ADDON_API_DIR}")
message(STATUS "NODE_INCLUDE_DIR: ${NODE_INCLUDE_DIR}")
target_link_libraries(krooms-server-lib PRIVATE
    ggwave
    ggwave-common
    ggwave-common-sdl2
    ${CMAKE_THREAD_LIBS_INIT}
)


add_executable(krooms-server
    src/krooms_server.cc
    src/main.cpp
)

target_include_directories(krooms-server PRIVATE
    PRIVATE src # 头文件目录
    PRIVATE ..
    PRIVATE ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(krooms-server PRIVATE
    ggwave
    ggwave-common
    ggwave-common-sdl2
    ${CMAKE_THREAD_LIBS_INIT}
)

