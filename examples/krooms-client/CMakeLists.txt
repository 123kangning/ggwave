cmake_minimum_required(VERSION 3.0...10.0)
# cmake_policy(SET CMP0042 NEW)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# find_package(node-addon-api REQUIRED)
project(krooms-client)

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
    # src/krooms_client_wrap.cc # Node-API 包装代码
    src/krooms_client.cc # Node-API 扩展入口（必须）
    src/main.cpp # 子项目原有 C++ 逻辑（复用）
)
# 3. 生成 Node 扩展（本质是共享库，强制 .node 扩展名）
# --------------------------
add_library(krooms-client-lib SHARED ${SOURCES})
set_target_properties(krooms-client-lib PROPERTIES
    PREFIX "" # 移除 lib 前缀
    SUFFIX ".node" # 强制 .node 扩展名（Node 要求）
    CXX_STANDARD 17 # 匹配 Node-API 要求的 C++ 标准
)
# 4. 链接 Node-API 和子项目依赖

target_include_directories(krooms-client-lib
    PRIVATE ${NODE_ADDON_API_DIR} # 用于 napi.h
    PRIVATE ${NODE_INCLUDE_DIR} # 用于 node_api.h
    PRIVATE src/krooms_client.h # 子项目原有头文件目录
    ..
    ${SDL2_INCLUDE_DIRS}
)
target_link_libraries(krooms-client-lib PRIVATE
    ggwave
    ggwave-common
    ggwave-common-sdl2
    # node-addon-api
    ${CMAKE_THREAD_LIBS_INIT}
    # ${CMAKE_JS_LIB}
    # ${NODEJS_LIBRARIES}
)


add_executable(krooms-client
   src/krooms_client.cc
    src/main.cpp
)

target_include_directories(krooms-client PRIVATE
PRIVATE ${NODE_ADDON_API_DIR} # 用于 napi.h
    PRIVATE ${NODE_INCLUDE_DIR} # 用于 node_api.h
    PRIVATE src/krooms_client.h # 子项目原有头文件目录
    ..
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(krooms-client PRIVATE
    ggwave
    ggwave-common
    ggwave-common-sdl2
    # node-addon-api
    ${CMAKE_THREAD_LIBS_INIT}
    # ${CMAKE_JS_LIB}
    # ${NODEJS_LIBRARIES}
)