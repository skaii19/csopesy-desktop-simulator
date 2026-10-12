# Created by:
- Chua, Myka Nadine R.
- Lim, Julienne Skye L.
- Ong, Eiress Bassey H.
- Xu, Kai Wen H.

# How to Run:
## 1. DOWNLOAD THE REQUIRED LIBRARIES

Dear ImGui
1. Download the latest source ZIP from the Dear ImGui GitHub repository.
2. Extract the ZIP file.

GLFW
1. Open the GLFW download page.
2. Download the 64-bit Windows precompiled binaries, not the source package.
3. Extract the files to a permanent location
   C:\Libraries\glfw

## 2. CREATE AND CONFIGURE THE PROJECT
1. Open Visual Studio 2022 and create a new Empty Project (C++).
2. In the standard toolbar, set Solution Platform to x64 instead of x86.
3. Open the project directory in File Explorer (the folder containing the
   .vcxproj file).
4. Create a folder named "imgui".

## 3. ORGANIZE THE IMGUI FILES
Copy these core files from the extracted Dear ImGui folder into the
project's imgui folder:

- imgui.h
- imgui.cpp
- imgui_draw.cpp
- imgui_tables.cpp
- imgui_widgets.cpp
- imconfig.h
- imgui_internal.h
- imstb_rectpack.h
- imstb_textedit.h
- imstb_truetype.h

From Dear ImGui's backends folder, copy these files into the same imgui
folder:

- imgui_impl_glfw.h
- imgui_impl_glfw.cpp
- imgui_impl_opengl3.h
- imgui_impl_opengl3.cpp
- imgui_impl_opengl_loader.h
- imgui_demo.cpp

## 4. ADD THE FILES TO SOLUTION EXPLORER
1. Open Visual Studio Solution Explorer.
2. Add the project's source files from the csopesy folder.
3. You may also copy the imgui folder from here.

## 5. CONFIGURE INCLUDE AND LIBRARY PATHS
1. Right-click the project in Solution Explorer and select Properties.
2. Set Configuration to "All Configurations" and Platform to "x64".

C/C++ > General > Additional Include Directories:
- $(ProjectDir)imgui
- C:\Libraries\glfw\include

Linker > General > Additional Library Directories:
- C:\Libraries\glfw\lib-vc2022

Linker > Input > Additional Dependencies:
- opengl32.lib
- C:\Libraries\glfw\lib-vc2022\glfw3.lib

Click Apply, then OK.

## 6. BUILD AND RUN

1. Press Ctrl+Shift+B to build the solution.
