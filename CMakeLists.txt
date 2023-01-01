cmake_minimum_required(VERSION 3.24)

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

set(CMAKE_CXX_FLAGS_RELEASE "-O2")
project(SpectrareFX)

set(CMAKE_CXX_STANDARD 17)

add_executable(SpectrareFX src/main.cpp src/Graphics/Graphics.cpp src/Graphics/Graphics.h src/Graphics/Window.cpp src/Graphics/Window.h src/Graphics/Vh.cpp src/Graphics/Vh.h src/Logger/Logger.cpp src/Logger/Logger.h src/Graphics/QueueFamilyIndices.h src/Graphics/SwapChainSupportDetails.h src/FileHelper.cpp src/FileHelper.h src/Graphics/SyncObjects.h src/Graphics/Wrapper.h src/Graphics/DebugLayer.cpp src/Graphics/DebugLayers.h src/Graphics/Renderer.cpp src/Graphics/Renderer.h src/Graphics/Pipeline.cpp src/Graphics/Pipeline.h src/Graphics/Device.cpp src/Graphics/Device.h src/Graphics/SwapChain.cpp src/Graphics/SwapChain.h src/Graphics/App.cpp src/Graphics/App.h src/Graphics/Model.cpp src/Graphics/Model.h src/Graphics/Object.cpp src/Graphics/Object.h)

target_link_libraries(SpectrareFX glfw vulkan dl pthread X11 Xxf86vm Xrandr Xi)
add_shader(SpectrareFX shader.frag)
add_shader(SpectrareFX shader.vert)