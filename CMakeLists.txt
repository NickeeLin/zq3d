# CMakeList.txt: zq3d 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.10)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("zq3d")

find_package(fmt CONFIG REQUIRED)
# find_package(wxWidgets REQUIRED COMPONENTS html adv gl core base webview aui net media) # by components 
find_package(wxWidgets CONFIG REQUIRED) # by config. we used vcpkg.json
find_package(OpenGL REQUIRED)
# d3dx12 glad glm
find_path(D3DX12_INCLUDE_DIRS "d3dx12.h")
#find_package(GLEW REQUIRED)
find_package(glad REQUIRED)
find_package(glm CONFIG REQUIRED)

# Using manifest configuration to solve high resolution screen (DPI) adaptation in windows systems.
if (wxWidgets_FOUND)
# Manifest is provided through zq3d.rc, don't generate your own.
 set(RC_FILE src/zq3d.rc)
 set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
else ()
# use manifest file directly.
 set(MANIFEST_FILE src/platform/msw/zq3d.manifest)
endif()

if(DEFINED ENV{VK_SDK_PATH})
    include_directories($ENV{VK_SDK_PATH}/include)
endif()



# 将源代码添加到此项目的可执行文件。
add_executable(zq3d WIN32 "src/zq3d.cpp" "src/zq3d.h"  ${RC_FILE} ${MANIFEST_FILE})

target_include_directories(zq3d PRIVATE src src/header)
target_include_directories(zq3d PRIVATE ${D3DX12_INCLUDE_DIRS})
target_link_libraries(zq3d PRIVATE fmt::fmt wx::core wx::base wx::gl wx::webview OpenGL::GL glad::glad glm::glm) # OpenGL::GL

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET zq3d PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
