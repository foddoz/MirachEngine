#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>

/**
 * @brief Opaque structure representing a window handle.
 *
 * Abstracts the platform-specific window representation (e.g., SDL_Window).
 */
using WindowHandle = void*;

/// Alias for a platform-specific graphics context handle (e.g., OpenGL context).
using GraphicsContextHandle = void*;

/**
 * @namespace Window
 * @brief Provides a platform-agnostic facade for window and OpenGL context management.
 */
namespace Window 
{

    /**
     * @brief Creates a new window with the specified title and dimensions.
     * @param title Title of the window.
     * @param width Width of the window in pixels.
     * @param height Height of the window in pixels.
     * @return Pointer to the created WindowHandle.
     */
    WindowHandle Create(const std::string& title, int width, int height);

    /**
     * @brief Destroys a previously created window.
     * @param window Pointer to the WindowHandle to destroy.
     */
    void Destroy(WindowHandle window);

    /**
     * @brief Creates a graphics context (e.g., OpenGL) for the given window.
     * @param window Pointer to the target window.
     * @return Handle to the graphics context.
     */
    GraphicsContextHandle CreateGraphicsContext(WindowHandle window);

    /**
     * @brief Makes the specified graphics context current for the given window.
     * @param window Pointer to the window.
     * @param context Graphics context to make current.
     */
    void MakeContextCurrent(WindowHandle window, GraphicsContextHandle context);

    /**
     * @brief Swaps the front and back buffers of the given window.
     * @param window Pointer to the window.
     */
    void SwapBuffers(WindowHandle window);

    /**
     * @brief Locks or unlocks the mouse cursor to the window center.
     * @param window Pointer to the window.
     * @param lock If true, locks the mouse; otherwise, unlocks it.
     * @param width Width of the window (used for centering).
     * @param height Height of the window (used for centering).
     */
    void LockMouse(WindowHandle window, bool lock, int width, int height);

    /**
     * @brief Retrieves the OpenGL function pointer for the given function name.
     * @param name Name of the OpenGL function.
     * @return Pointer to the function, or nullptr if not found.
     */
    void* GetProcAddress(const char* name);

    /**
     * @brief Initializes platform-specific graphics loaders (e.g., GLAD).
     * @return True if successful, false otherwise.
     */
    bool InitGraphics();

} // namespace Window

#endif // WINDOW_HPP
