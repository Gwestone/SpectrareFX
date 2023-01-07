cmake_minimum_required(VERSION 3.1)

function(add_shader TARGET SHADER)
    find_program(GLSLC glslc)

    set(current-shader-path ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER})
    set(current-output-path ${CMAKE_BINARY_DIR}/shaders/${SHADER}.spv)

    # Add a custom command to compile GLSL to SPIR-V.
    get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
    file(MAKE_DIRECTORY ${current-output-dir})

    add_custom_command(
            OUTPUT ${current-output-path}
            COMMAND ${GLSLC} -o ${current-output-path} ${current-shader-path}
            DEPENDS ${current-shader-path}
            IMPLICIT_DEPENDS CXX ${current-shader-path}
            VERBATIM)

    # Make sure our build depends on this output.
    set_source_files_properties(${current-output-path} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${current-output-path})
endfunction(add_shader)

set(ImGuiImportFiles
        libs/imgui/imconfig.h
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_internal.h
        libs/imgui/imgui.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/imgui_demo.cpp
        libs/imgui/backends/imgui_impl_glfw.h libs/imgui/backends/imgui_impl_glfw.cpp
        libs/imgui/backends/imgui_impl_vulkan.h libs/imgui/backends/imgui_impl_vulkan.cpp)

include_directories("./libs/tinyobjloader")
include_directories("./libs/imgui")

#set(CMAKE_CXX_FLAGS_RELEASE "-O2")
project(SpectrareFX)

set(CMAKE_CXX_STANDARD 17)

add_executable(SpectrareFX
        src/main.cpp
        src/Graphics/Window.cpp src/Graphics/Window.h
        src/Graphics/Vh.cpp src/Graphics/Vh.h
        src/Logger/Logger.cpp src/Logger/Logger.h
        src/Graphics/QueueFamilyIndices.h src/Graphics/SwapChainSupportDetails.h
        src/FileHelper.cpp src/FileHelper.h
        src/Graphics/SyncObjects.h
        src/Graphics/Wrapper.h
        src/Graphics/DebugLayer.cpp src/Graphics/DebugLayers.h
        src/Graphics/Pipeline.cpp src/Graphics/Pipeline.h
        src/Graphics/Device.cpp src/Graphics/Device.h
        src/Graphics/SwapChain.cpp src/Graphics/SwapChain.h
        src/Graphics/App.cpp src/Graphics/App.h
        src/Graphics/Model.cpp src/Graphics/Model.h
        src/Graphics/Object.cpp src/Graphics/Object.h
        src/Graphics/Camera.cpp src/Graphics/Camera.h
        src/Graphics/Render.h src/Graphics/Render.cpp
        src/Graphics/BasicRenderSystem.h src/Graphics/BasicRenderSystem.cpp
        src/Graphics/KeyboardMovementController.h src/Graphics/KeyboardMovementController.cpp
        src/Graphics/utils.h
        src/Graphics/Buffer.h src/Graphics/Buffer.cpp
        src/Graphics/FrameInfo.h
        src/Graphics/Descriptors.cpp
        src/Graphics/imguiImports.h
        ${ImGuiImportFiles})

target_link_libraries(SpectrareFX glfw vulkan dl pthread X11 Xxf86vm Xrandr Xi)
add_shader(SpectrareFX shader.frag)
add_shader(SpectrareFX shader.vert)

file(COPY "models" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "./libs/imgui/misc/fonts" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")