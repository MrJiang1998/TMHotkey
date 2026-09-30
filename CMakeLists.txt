# TMHotkey - 自定义按键触发 TM（隐藏/显示全部界面）
# 依赖 CommonLibSSE-NG (vcpkg) + spdlog

cmake_minimum_required(VERSION 3.21)

project(TMHotkey VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 静态链接 CRT
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_FLAGS_RELEASE "/O2 /std:c++20 /MP")

if(NOT WIN32)
  message(FATAL_ERROR "TMHotkey 必须在 Windows 环境构建")
endif()

find_package(spdlog CONFIG REQUIRED)

# 兼容不同来源的 CommonLibSSE 包名
find_package(CommonLibSSE CONFIG QUIET)
if(NOT TARGET CommonLibSSE::CommonLibSSE)
  find_package(CommonLibSSE-NG CONFIG QUIET)
endif()

if(TARGET CommonLibSSE::CommonLibSSE)
  set(COMMONLIB_TARGET CommonLibSSE::CommonLibSSE)
elseif(TARGET CommonLibSSE-NG::CommonLibSSE-NG)
  set(COMMONLIB_TARGET CommonLibSSE-NG::CommonLibSSE-NG)
else()
  message(FATAL_ERROR "未找到 CommonLibSSE target")
endif()

add_library(TMHotkey SHARED main.cpp)
target_link_libraries(TMHotkey PRIVATE ${COMMONLIB_TARGET} spdlog::spdlog)
target_compile_definitions(TMHotkey PRIVATE NOMINMAX)

set_target_properties(TMHotkey PROPERTIES
  PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 复制 DLL 到安装目录
add_custom_command(TARGET TMHotkey POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/dist/SKSE/Plugins"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:TMHotkey>"
          "${CMAKE_SOURCE_DIR}/dist/SKSE/Plugins/TMHotkey.dll"
  COMMENT "复制 TMHotkey.dll 到 dist/SKSE/Plugins/"
)
