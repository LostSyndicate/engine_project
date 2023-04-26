cmake_minimum_required (VERSION 3.8)

project(SM3)

set(CMAKE_CXX_FLAGS "-fcompare-debug-second")

include_directories(SM3 ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/ft2 "${CMAKE_SOURCE_DIR}/include")

add_executable (SM3 ${CMAKE_SOURCE_DIR}/lib/glad/glad.c
        "source/_Test.cpp"
        "source/Core/Reference.cpp"
        "source/Core/Application.cpp"
        "source/Graphics/Texture.cpp"
        "source/Graphics/Mesh.cpp"
        "source/Graphics/Shader.cpp"
        "source/Graphics/Uniform.cpp"
        "source/Graphics/Camera.cpp"
        "source/Graphics/Model.cpp"
        "source/Graphics/Framebuffer.cpp"
        "source/Graphics/Skybox.cpp"
        "source/Input/Input.cpp"
        "source/Graphics/Font.cpp"
        "source/Core/Util.cpp"

        "source/Graphics/Context.cpp"
        "source/Core/Scene.cpp"
        "source/Graphics/Light.cpp"
        "source/Components/Transform.cpp"
        "source/Graphics/Renderable.cpp"
        "source/Graphics/Material.cpp"
        
        "source/UI/UIElement.cpp"
        "source/UI/UIWindow.cpp"
        "source/UI/UILabel.cpp"
        "source/UI/UIDropdown.cpp"
        "source/UI/UIButton.cpp"
        "source/UI/UICanvas.cpp"
        "source/UI/UIField.cpp"
 "source/Xml/XmlParser.cpp" "source/Core/NodeManager.cpp" "source/Core/Profiler.cpp")

target_compile_features(SM3 PRIVATE cxx_std_17)
target_link_options(SM3 PRIVATE -static-libgcc -static-libstdc++)
target_link_directories(SM3 PUBLIC ${CMAKE_SOURCE_DIR}/lib/x64)
target_link_libraries(SM3 SDL2 assimp freetype)
