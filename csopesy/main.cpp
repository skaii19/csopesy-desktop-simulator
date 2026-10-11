#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <tuple>
#include <cstdlib>
#include <ctime>
#include <sstream>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Select GL version + let the backend select a GLSL version
    const char* glsl_version = nullptr;
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + generally GLSL 150
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + generally GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "Desktop", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;

    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_1 = false;
    bool show_2 = false;
    bool show_task_manager = false;
    float taskbar_height = 50.0f;

    // dummy values for task manager
    std::vector<std::tuple<int, std::string, float, int>> processes = {
        { 1, "System", 2.5f, 64 },
        { 234, "explorer.exe", 4.8f, 120 },
        { 1024, "imgui_demo.exe", 12.3f, 256 },
        { 2048, "renderer", 6.1f, 512 },
        { 4096, "worker", 0.5f, 32 }
    };
    std::srand((unsigned)std::time(nullptr));

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        // Taskbar
        {
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + main_viewport->WorkSize.y - taskbar_height), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(main_viewport->WorkSize.x, taskbar_height), ImGuiCond_Always);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::Begin("##Taskbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);

            if (ImGui::Button("App A", ImVec2(90, 34))) { show_1 = !show_1; }
            ImGui::SameLine();
            if (ImGui::Button("App B", ImVec2(90, 34))) { show_2 = !show_2; }
            ImGui::SameLine();
            if (ImGui::Button("Task Manager", ImVec2(90, 34))) { show_task_manager = !show_task_manager; }

            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 90.0f);
            if (ImGui::Button("Power", ImVec2(80, 34)))
                glfwSetWindowShouldClose(window, GLFW_TRUE);

            ImGui::End();
            ImGui::PopStyleVar();
        }

        // App A window
        if (show_1)
        {
            ImGui::SetNextWindowSize(ImVec2(520, 360), ImGuiCond_Appearing);
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + main_viewport->WorkSize.x * 0.5f - 260, main_viewport->WorkPos.y + 60), ImGuiCond_Appearing);
            if (ImGui::Begin("App A - Dashboard", &show_1))
            {
                ImGui::Text("Hello There!");
            }
            ImGui::End();
        }

        // App B window
        if (show_2)
        {
            ImGui::SetNextWindowSize(ImVec2(420, 260), ImGuiCond_Appearing);
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 80, main_viewport->WorkPos.y + 80), ImGuiCond_Appearing);
            if (ImGui::Begin("App B - Info", &show_2))
            {
                ImGui::Text("Hi!!");
            }
            ImGui::End();
        }

        // Task Manager
        if (show_task_manager)
        {
            ImGui::SetNextWindowSize(ImVec2(640, 360), ImGuiCond_Appearing);
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + main_viewport->WorkSize.x * 0.5f - 320, main_viewport->WorkPos.y + 40), ImGuiCond_Appearing);
            if (ImGui::Begin("Task Manager", &show_task_manager))
            {
                ImGui::Text("Mock Task Manager");
                ImGui::Separator();

                for (auto& p : processes)
                {
                    float cpu = std::get<2>(p) + ((std::rand() % 21) - 10) * 0.1f;
                    if (cpu < 0.1f) cpu = 0.1f;
                    if (cpu > 99.9f) cpu = 99.9f;
                    int mem = std::get<3>(p) + ((std::rand() % 11) - 5);
                    if (mem < 16) mem = 16;
                    std::get<2>(p) = cpu;
                    std::get<3>(p) = mem;
                }

                if (ImGui::BeginTable("##proc_table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInner))
                {
                    ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 60);
                    ImGui::TableSetupColumn("Process", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 80);
                    ImGui::TableSetupColumn("Memory (MB)", ImGuiTableColumnFlags_WidthFixed, 120);
                    ImGui::TableHeadersRow();

                    for (auto& p : processes)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%d", std::get<0>(p));
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(std::get<1>(p).c_str());
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%.1f", std::get<2>(p));
                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("%d", std::get<3>(p));
                    }
                    ImGui::EndTable();
                }
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
