#ifndef PROGRAM_HPP
#define PROGRAM_HPP

//#include <SDL3/SDL.h>
#include "window.hpp"
//#include <glad/gl.h>
#include "../Graphics/graphics.hpp"

#include <string>
#include <unordered_map>

//#include "Camera/camera.hpp"

/**
 * @class Program
 * @brief Manages core SDL and OpenGL initialization, shader program creation, window rendering, and timing.
 *
 * This class handles window and context setup, shader compilation, delta time calculation, and input locking.
 */
class Program
{
public:
    /**
     * @brief Constructs the Program object with initial screen dimensions.
     * @param screenWidth Width of the application window in pixels.
     * @param screenHeight Height of the application window in pixels.
     */
    Program(float screenWidth, float screenHeight);

    /**
     * @brief Initializes SDL, creates the OpenGL context, and loads GLAD.
     */
    void Initialise();

    void InitialiseGraphics();

    void InitialiseWindow();

    /**
     * @brief Creates and compiles a named graphics pipeline (shader program).
     * @param name Identifier to store the shader program under.
     * @param vsPath File path to the vertex shader.
     * @param fsPath File path to the fragment shader.
     */
    void CreateGraphicsPipeline(const std::string& name, const std::string& vsPath, const std::string& fsPath);

    /**
     * @brief Retrieves a shader program ID previously created with CreateGraphicsPipeline.
     * @param name The name identifier of the shader program.
     * @return OpenGL shader program ID.
     */
    unsigned int GetShaderProgram(const std::string& name) const;

    /**
     * @brief Releases OpenGL and SDL resources.
     */
    void Delete();

    void DeleteWindow();

    void DeleteGraphics();

    /**
     * @brief Locks or unlocks the mouse cursor within the window.
     * @param lock True to lock the mouse; false to unlock.
     */
    void LockMouse(bool lock) const;

    /**
     * @brief Updates internal frame timing variables to compute delta time.
     */
    void UpdateDeltaTime();

    /**
     * @brief Returns the time elapsed since the last frame.
     * @return Time in seconds between the last two frames.
     */
    float GetDeltaTime() const;

    /**
     * @brief Swaps the OpenGL window buffer to display the current frame.
     */
    void SwapWindow();

    void SetQuit(bool quit);

    bool GetQuit() const;

    float GetScreenWidth() const;

    float GetScreenHeight() const;

    //Camera mCamera; ///< [Optional] Main camera object for future implementation.

    friend class Debug;

    

    

    

private:
    /**
     * @brief Loads a text file (typically a shader) into a string.
     * @param fileName Path to the file.
     * @return Contents of the file as a single string.
     */
    std::string LoadShader(const std::string& fileName);

    /**
     * @brief Checks and prints the compile status of a shader.
     * @param shader OpenGL shader ID.
     */
    void CheckShaderCompile(unsigned int shader);

    /**
     * @brief Checks and prints the link status of a shader program.
     * @param program OpenGL program ID.
     */
    void CheckProgramLink(unsigned program);

    /**
     * @brief Compiles a shader of a given type from source code.
     * @param type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
     * @param source The GLSL source code for the shader.
     * @return Compiled shader ID.
     */
    unsigned int CompileShader(unsigned int type, const std::string& source);

    /**
     * @brief Links a complete OpenGL shader program from vertex and fragment shader source.
     * @param vertexShaderSource GLSL source for the vertex shader.
     * @param fragmentShaderSource GLSL source for the fragment shader.
     * @return Linked program ID.
     */
    unsigned int CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

    /// Width of the window in pixels.
    float m_screenWidth;

    /// Height of the window in pixels.
    float m_screenHeight;

    /// Pointer to the window handle abstraction.
    WindowHandle m_window;

    /// Graphics context handle (OpenGL).
    GraphicsContextHandle m_graphicsContext;

    /// Flag indicating if the application is scheduled to quit.
    bool m_quit;

    /// Stores all created shader programs, indexed by name.
    std::unordered_map<std::string, unsigned int> m_shaderPrograms;

    /// Timestamp of the last frame.
    float m_lastFrameTime;

    /// Time between the current and previous frame.
    float m_deltaTime;
};

#endif
