#include "window.hpp"
#include <SDL3/SDL.h>

#include <glad/gl.h>

WindowHandle Window::Create(const std::string& title, int width, int height) 
{
    SDL_Init(SDL_INIT_VIDEO);

    WindowHandle handle;
    handle = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL);
    return handle;
}

void Window::Destroy(WindowHandle window) 
{
    if (window) 
    {
        SDL_DestroyWindow((SDL_Window*) window);
    }

    SDL_Quit();
}

GraphicsContextHandle Window::CreateGraphicsContext(WindowHandle window) 
{
    return SDL_GL_CreateContext((SDL_Window*) window);
}

void Window::MakeContextCurrent(WindowHandle window, GraphicsContextHandle context) 
{
    SDL_GL_MakeCurrent((SDL_Window*) window, static_cast<SDL_GLContext>(context));
}

void Window::SwapBuffers(WindowHandle window) 
{
    SDL_GL_SwapWindow((SDL_Window*) window);
}

void Window::LockMouse(WindowHandle window, bool lock, int width, int height) 
{
    SDL_WarpMouseInWindow((SDL_Window*) window, float(width / 2), float(height / 2));
    SDL_SetWindowRelativeMouseMode((SDL_Window*) window, lock);
}

void* Window::GetProcAddress(const char* name) 
{
    return SDL_GL_GetProcAddress(name);
}

bool Window::InitGraphics()
{
    return gladLoadGL(SDL_GL_GetProcAddress);
}
