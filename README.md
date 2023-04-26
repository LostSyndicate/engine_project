A basic game engine, with the intent for me to learn how renderers and other logic should be implemented.

# Libaries Used
Compilation Required:
- SDL2 (lib/sdl2)
- freetype (lib/ft2)
- assimp (lib/assimp)

No compilation required:
- [glad](https://glad.dav1d.de/) (both .h and .c files must be put in lib/glad, along with khrplatform.h so they should all be in the same folder)
- glm (lib/glm)
- stb (uses image,image_write,rect_pack then put in lib/stb)

When compiled, libraries and DLLs should be put under lib/x64 or lib/x86 and when compiling the program, be put in the same directory as the executable.

***OpenGL 3.3 Core is used for rendering, version can be higher.***

# Compilation
- **Make sure you compile everything with same compiler!**

Run CMakeLists.txt then open the project and compile.
On linux use 'cmake -B"build" -G"MinGW Makefiles" CMakeLists.txt" then cd into build and run make.
If you are compiling from MinGW on windows you may need to open "Developer Command Prompt for VS" and run nmake from there.

Once compiled don't forget to copy the DLLs to the directory the executable is located.

# Starting the application

To start the application, create a class that uses ```Application``` and run it.

```c++
#include "Core/Application.h"

class MyApp : public Application {
public:
  MyApp(const char* title, int width, int height) : Application(title,width,height()
  {}
  
  void OnInit() override { /* called when app wants to load. */ }
  void OnDestroy() override { /* called when app is about to be destroyed. */}
  void OnFrame(float delta) override { /* called every frame */ }
  void OnEvent(SDL_Event event) override { /* called when an SDL event is processed. */ }
  // etc.. look at Application for more methods to override.
};

// in main.cpp:

int main() {
  // create app with window titled 'hi'
  MyApp app("hi",1280,720);
  
  // call init logic
  app->OnInit();
  
  // run app and enter main loop.
  app->Run();
  
  // call destroy logic.
  app->OnDestroy();
  return 0;
}

```

# Rendering

See ```Graphics/Model.h``` or ```Graphics/Mesh.h```.

- In the future, you will be able to use ```Graphics/GraphicsRenderer.h```. This system would allow you to add some component to render, and handle all rendering aspects automatically.

To render text and most 2D shapes use ```UI/UICanvas.h```

# TODO

- [x] Primitive rendering (mesh, canvas)
- [x] Text rendering (canvas)
- [x] Basic UI
- [x] Node system
- [ ] Graphics Renderer
- [ ] Graphics Command List
- [ ] Forward Tiled Renderer
- [ ] Shadow Renderer (however there is a shadow mapper in _Test.cpp)
- [ ] Screen-Space Ambient Occulsion (SSAO)
- [ ] Deferred Renderer (may not due because of transparency)
