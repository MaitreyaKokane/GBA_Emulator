#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include <cmath>
#include <cstdio>
#include <limits>

struct DisplayModes
{
    SDL_DisplayMode** modes = nullptr; // SDL-owned mode objects; this array is ours to free.
    int count = 0;
    const SDL_DisplayMode* preferredExclusive = nullptr;
    SDL_DisplayID displayID = 0;
};

static void ReleaseDisplayModes(DisplayModes& cache)
{
    SDL_free(cache.modes);
    cache.modes = nullptr;
    cache.count = 0;
    cache.preferredExclusive = nullptr;
    cache.displayID = 0;
}

// Cache display modes instead of enumerating them every time F11 is pressed.
static void RefreshDisplayModes(SDL_Window* window, DisplayModes& cache)
{
    ReleaseDisplayModes(cache);

    const SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
    if (displayID == 0)
    {
        std::printf("Could not determine window display: %s\n", SDL_GetError());
        return;
    }

    int count = 0;
    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displayID, &count);
    if (modes == nullptr || count <= 0)
    {
        std::printf("Could not enumerate fullscreen modes: %s\n", SDL_GetError());
        SDL_free(modes);
        return;
    }

    cache.modes = modes;
    cache.count = count;
    cache.displayID = displayID;
    cache.preferredExclusive = modes[0]; // SDL sorts modes by preference.

    // Prefer the desktop resolution and the closest refresh rate.
    const SDL_DisplayMode* desktop = SDL_GetDesktopDisplayMode(displayID);
    if (desktop != nullptr)
    {
        float bestRefreshDifference = std::numeric_limits<float>::infinity();

        for (int i = 0; i < count; ++i)
        {
            const SDL_DisplayMode* candidate = modes[i];
            if (candidate == nullptr ||
                candidate->w != desktop->w ||
                candidate->h != desktop->h)
            {
                continue;
            }

            const float difference =
                std::fabs(candidate->refresh_rate - desktop->refresh_rate);

            if (difference < bestRefreshDifference)
            {
                bestRefreshDifference = difference;
                cache.preferredExclusive = candidate;
            }
        }
    }
}

static bool SetWindowMode(
    SDL_Window* window,
    bool fullscreen,
    bool borderless,
    const SDL_DisplayMode* exclusiveMode,
    bool& isFullscreen,
    bool& isBorderless)
{
    if (window == nullptr)
        return false;

    if (!fullscreen)
    {
        if (!SDL_SetWindowFullscreen(window, false))
        {
            std::printf("Could not leave fullscreen: %s\n", SDL_GetError());
            return false;
        }

        SDL_SetWindowBordered(window, true);
    }
    else
    {
        // SDL3 uses nullptr for borderless desktop fullscreen.
        // A non-null mode requests exclusive fullscreen.
        const SDL_DisplayMode* mode = borderless ? nullptr : exclusiveMode;
        if (!borderless && mode == nullptr)
        {
            std::printf("Exclusive fullscreen is unavailable; using borderless instead.\n");
            mode = nullptr;
            borderless = true;
        }

        if (!SDL_SetWindowFullscreenMode(window, mode))
        {
            std::printf("Could not set fullscreen mode: %s\n", SDL_GetError());
            return false;
        }

        // Changing the mode while already fullscreen is handled by
        // SDL_SetWindowFullscreenMode; don't request fullscreen again.
        if (!isFullscreen && !SDL_SetWindowFullscreen(window, true))
        {
            std::printf("Could not enter fullscreen: %s\n", SDL_GetError());
            return false;
        }
    }

    isFullscreen = fullscreen;
    isBorderless = fullscreen && borderless;
    return true;
}

int run(int, char**)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    const char* glslVersion = nullptr;

#if defined(IMGUI_IMPL_OPENGL_ES2)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);   // No 3D depth buffer needed for this UI-only stage.
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0); // ImGui doesn't need a stencil buffer here.

    float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    if (scale <= 0.0f)
        scale = 1.0f;

    SDL_Window* window = SDL_CreateWindow(
        "Indigo Emulator",
        static_cast<int>(1280 * scale),
        static_cast<int>(720 * scale),
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (window == nullptr)
    {
        std::printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    const SDL_WindowID windowID = SDL_GetWindowID(window);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr)
    {
        std::printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!SDL_GL_MakeCurrent(window, glContext))
    {
        std::printf("SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // VSync prevents the empty UI loop from rendering as fast as possible.
    if (!SDL_GL_SetSwapInterval(1))
        std::printf("VSync unavailable: %s\n", SDL_GetError());

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;

    if (!ImGui_ImplSDL3_InitForOpenGL(window, glContext))
    {
        std::printf("ImGui SDL3 initialization failed.\n");
        ImGui::DestroyContext();
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!ImGui_ImplOpenGL3_Init(glslVersion))
    {
        std::printf("ImGui OpenGL3 initialization failed.\n");
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    DisplayModes displayModes;
    RefreshDisplayModes(window, displayModes);

    bool isFullscreen = false;
    bool isBorderless = false;
    bool done = false;

    int drawableWidth = 0;
    int drawableHeight = 0;
    SDL_GetWindowSizeInPixels(window, &drawableWidth, &drawableHeight);
    glViewport(0, 0, drawableWidth, drawableHeight);
    glClearColor(0.10f, 0.10f, 0.10f, 1.0f); // GL state: set once, not every frame.

    SDL_ShowWindow(window);

    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT)
            {
                done = true;
            }
            else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == windowID)
            {
                done = true;
            }
            else if (event.type == SDL_EVENT_WINDOW_DISPLAY_CHANGED &&
                event.window.windowID == windowID)
            {
                // Refresh the cache only if the window moves to another display.
                RefreshDisplayModes(window, displayModes);
            }
            else if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED &&
                event.window.windowID == windowID)
            {
                drawableWidth = event.window.data1;
                drawableHeight = event.window.data2;
                glViewport(0, 0, drawableWidth, drawableHeight);
            }
            else if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.windowID == windowID &&
                !event.key.repeat)
            {
                if (event.key.key == SDLK_ESCAPE && isFullscreen)
                {
                    SetWindowMode(window, false, false,
                        displayModes.preferredExclusive,
                        isFullscreen, isBorderless);
                }
                else if (event.key.key == SDLK_F11)
                {
                    if (!isFullscreen)
                    {
                        // Windowed -> Exclusive fullscreen, or borderless if unavailable.
                        const bool canUseExclusive =
                            displayModes.preferredExclusive != nullptr;
                        SetWindowMode(window, true, !canUseExclusive,
                            displayModes.preferredExclusive,
                            isFullscreen, isBorderless);
                    }
                    else if (!isBorderless)
                    {
                        // Exclusive -> Borderless fullscreen.
                        SetWindowMode(window, true, true,
                            displayModes.preferredExclusive,
                            isFullscreen, isBorderless);
                    }
                    else
                    {
                        // Borderless -> Windowed.
                        SetWindowMode(window, false, false,
                            displayModes.preferredExclusive,
                            isFullscreen, isBorderless);
                    }
                }
            }
        }

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                    // TODO: Load a ROM using a file dialog.
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    // TODO: Save emulator state.
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit"))
                    done = true;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Display"))
            {
                if (ImGui::BeginMenu("Mode (F11 to cycle)"))
                {
                    if (ImGui::MenuItem("Windowed", nullptr, !isFullscreen))
                    {
                        SetWindowMode(window, false, false,
                            displayModes.preferredExclusive,
                            isFullscreen, isBorderless);
                    }

                    if (ImGui::MenuItem("Exclusive Fullscreen", nullptr,
                        isFullscreen && !isBorderless,
                        displayModes.preferredExclusive != nullptr))
                    {
                        SetWindowMode(window, true, false,
                            displayModes.preferredExclusive,
                            isFullscreen, isBorderless);
                    }

                    if (ImGui::MenuItem("Borderless Fullscreen", nullptr,
                        isFullscreen && isBorderless))
                    {
                        SetWindowMode(window, true, true,
                            displayModes.preferredExclusive,
                            isFullscreen, isBorderless);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // TODO: Run the emulator core at its own timing and draw the GBA framebuffer here.
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ReleaseDisplayModes(displayModes);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int main(int argc, char** argv)
{
    return run(argc, argv);
}
