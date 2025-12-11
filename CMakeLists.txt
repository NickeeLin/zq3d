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

set(ZQ3D_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
file(TO_NATIVE_PATH "${ZQ3D_RESOURCES_DIR}" ZQ3D_RESOURCES_DIR_WIN)
 
# dependencies
find_package(fmt CONFIG REQUIRED)
# find_package(wxWidgets REQUIRED COMPONENTS html adv gl core base webview aui net media) # by components 
find_package(wxWidgets CONFIG REQUIRED) # by config. we used vcpkg.json
find_package(OpenGL REQUIRED)
# d3dx12 glad glm
find_path(D3DX12_INCLUDE_DIRS "d3dx12.h")
#find_package(GLEW REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

if(DEFINED ENV{VK_SDK_PATH})
    include_directories($ENV{VK_SDK_PATH}/include)
endif()

# Using manifest configuration to solve high resolution screen (DPI) adaptation in windows systems.
if (wxWidgets_FOUND)
# Manifest is provided through zq3d.rc, don't generate your own.
 set(RC_FILE src/zq3d.rc)
 set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
else ()
# use manifest file directly.
 set(MANIFEST_FILE src/platform/msw/zq3d.manifest)
endif()

# add souces file
set(SOURCES 
    # main
    "src/zq3d.cpp"
    # ui
     "src/ui/ZQFrame.cpp" 
     "src/ui/ZQGLCanvas.cpp" 
     "src/ui/ZQGLContext.cpp"
    # render
    "src/render/RenderEngine.cpp" 
    "src/render/ShaderManager.cpp"
    "src/render/ShaderProgram.cpp"
    # scene
    "src/scene/Buffer.cpp"
    "src/scene/Camera.cpp"
    "src/scene/Component.cpp" 
    "src/scene/Light.cpp" 
    "src/scene/Material.cpp" 
    "src/scene/Mesh.cpp"
    "src/scene/Model.cpp" 
    "src/scene/Texture.cpp"
    "src/scene/LightSource.cpp" 
    "src/scene/SceneManager.cpp" 
    # utils
    "src/utils/TestUtils.cpp"
    "src/utils/Utils.cpp" 
 
)

# add lib
set(PKGLIBS fmt::fmt wx::core wx::base wx::gl wx::webview OpenGL::GL glad::glad glm::glm assimp::assimp)
# 将源代码添加到此项目的可执行文件。
add_executable(zq3d WIN32 ${RC_FILE} ${MANIFEST_FILE} ${SOURCES}  "src/time/TimeManager.cpp" "src/time/TimeManager.h")

target_include_directories(zq3d PRIVATE src ${D3DX12_INCLUDE_DIRS})
target_link_libraries(zq3d PRIVATE ${PKGLIBS}) # OpenGL::GL

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET zq3d PROPERTY CXX_STANDARD 20)
endif()

# install resources
if(WIN32)
    install(DIRECTORY "${ZQ3D_RESOURCES_DIR}/" DESTINATION "${CMAKE_INSTALL_PREFIX}/resources")
endif()

# Symlinking the resources directory into the build tree
if (WIN32)
    # This has to be a separate target due to the windows command line lenght limits

    if (CMAKE_CONFIGURATION_TYPES)
        foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
            file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CONF}" WIN_CONF_OUTPUT_DIR)
            file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CONF}/resources" WIN_RESOURCES_SYMLINK)
            add_custom_command(TARGET zq3d POST_BUILD
                COMMAND if exist "${WIN_CONF_OUTPUT_DIR}" "("
                        if not exist "${WIN_RESOURCES_SYMLINK}" "("
                            mklink /J "${WIN_RESOURCES_SYMLINK}" "${ZQ3D_RESOURCES_DIR_WIN}"
                        ")"
                    ")"
                COMMENT "Symlinking the resources directory into the build tree"
                VERBATIM
            )
        endforeach ()
    else ()
        file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/resources" WIN_RESOURCES_SYMLINK)
        add_custom_command(TARGET zq3d POST_BUILD
            COMMAND if not exist "${WIN_RESOURCES_SYMLINK}" "(" mklink /J "${WIN_RESOURCES_SYMLINK}" "${ZQ3D_RESOURCES_DIR_WIN}" ")"
            COMMENT "Symlinking the resources directory into the build tree"
            VERBATIM
        )
    endif ()
endif()

