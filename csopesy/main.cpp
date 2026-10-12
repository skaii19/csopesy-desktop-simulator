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
#include <string.h>

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

    const char* glsl_version = nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // removes the top bar

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(monitor);
    GLFWwindow* window = glfwCreateWindow((int)mode->width, (int)mode->height, "Desktop", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;

    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // state
    bool show_1 = false;
    bool show_2 = false;
    bool show_message = false;
    bool show_task_manager = false;
    static ImVec4 colors = ImVec4(1.0, 0, 0, 1.0);
    float taskbar_height = 50.0f;
    static char text[64] = "";
    static char display[64] = "";

    // dummy values for task manager
    std::vector<std::tuple<int, std::string, float, int>> processes = {
        { 1, "System", 2.5f, 64 },
        { 234, "application.exe", 4.8f, 120 },
        { 1024, "demo.exe", 12.3f, 256 },
        { 2048, "game.exe", 6.1f, 512 },
        { 4096, "background.exe", 0.5f, 32 }
    };

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        ImDrawList* bg_draw_list = ImGui::GetBackgroundDrawList();

        // Define gradient colors (IM_COL32(R, G, B, A))
        ImU32 top_color = IM_COL32(30, 30, 60, 255); // indigo
        ImU32 bottom_color = IM_COL32(80, 100, 150, 255); // lighter blue


        // Draws rectangle covering the entire viewport with vertical gradient
        bg_draw_list->AddRectFilledMultiColor(
            main_viewport->WorkPos,
            ImVec2(main_viewport->WorkPos.x + main_viewport->WorkSize.x,
                main_viewport->WorkPos.y + main_viewport->WorkSize.y),
            top_color,     // Top left
            top_color,     // Top right
            bottom_color,  // Bottom right
            bottom_color   // Bottom left
        );
        

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
            if (ImGui::Button("Task Manager", ImVec2(200, 34))) { show_task_manager = !show_task_manager; }

            // Get time using <ctime> 
            time_t rawtime;
            struct tm timeinfo;
            time(&rawtime);

#if defined(_WIN32)
            localtime_s(&timeinfo, &rawtime);
#else
            localtime_r(&rawtime, &timeinfo);
#endif

            char time_buffer[32];
            std::strftime(time_buffer, sizeof(time_buffer), "%I:%M:%S %p", &timeinfo);

            float power_width = 80.0f;
            float right_padding = 10.0f;

            // show time on the right 
            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - power_width - right_padding);
            if (ImGui::Button("Power", ImVec2(power_width, 34)))
                glfwSetWindowShouldClose(window, GLFW_TRUE);

            float time_width = ImGui::CalcTextSize(time_buffer).x;
            float spacing = 15.0f;

            ImGui::SameLine(
                ImGui::GetWindowContentRegionMax().x
                - power_width
                - right_padding
                - time_width
                - spacing
            );

            ImGui::SetCursorPosY((ImGui::GetWindowHeight() - ImGui::GetFrameHeight()) * 0.5f);
            ImGui::Text("%s", time_buffer);
            
            ImGui::End();
            ImGui::PopStyleVar();
        }

        // App A window
        if (show_1)
        {
            ImGui::SetNextWindowSize(ImVec2(520, 360), ImGuiCond_Appearing);
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + main_viewport->WorkSize.x * 0.5f - 260, main_viewport->WorkPos.y + 60), ImGuiCond_Appearing);
            if (ImGui::Begin("App A", &show_1))
            {
                ImGui::Text("Pick a Color!");
                ImGui::ColorEdit3("##colorpicker", (float*)&colors); //placeholder
            }
            ImGui::End();
        }

        // App B window
        if (show_2)
        {
            ImGui::SetNextWindowSize(ImVec2(420, 260), ImGuiCond_Appearing);
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 80, main_viewport->WorkPos.y + 80), ImGuiCond_Appearing);
            if (ImGui::Begin("App B", &show_2))
            {
                ImGui::Text("Write a message!");
                ImGui::InputText("##textinput", text, sizeof(text));

                ImGui::SameLine();
                if (ImGui::Button("Send")) {
                    show_message = true;
                    std::memcpy(display, text, sizeof(text));
                }
            }

            if (show_message) {
                ImGui::Text(display);
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

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
