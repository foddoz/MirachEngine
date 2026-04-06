#include "program.hpp"
#include "time.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

// Constructor that sets up screen dimensions and initializes timing variables
Program::Program(float screenWidth, float screenHeight)
    : m_screenWidth(screenWidth),
    m_screenHeight(screenHeight),
    m_window(nullptr),
    m_graphicsContext(nullptr),
    m_quit(false),
    m_lastFrameTime(0.0f),
    m_deltaTime(0.0f)
{
}

// Initializes SDL window, OpenGL context, and loads GLAD
void Program::Initialise()
{
    // Create the SDL window using the custom Window facade
    m_window = Window::Create("Game", static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight));

    // Create the OpenGL graphics context
    m_graphicsContext = Window::CreateGraphicsContext(m_window);

    // Make the context current for OpenGL calls
    Window::MakeContextCurrent(m_window, m_graphicsContext);

    // Initialize GLAD (or equivalent); exit if failed
    if (!Window::InitGraphics()) 
    {
        std::cerr << "Failed to initialize graphics\n";
        exit(1);
    }

    // Optional: Print OpenGL version info
    // GetOpenGLVersionInfo();
}

void Program::InitialiseGraphics() 
{
    if (!Window::InitGraphics()) 
    {
        std::cerr << "Failed to initialize GL/GLAD\n";
        exit(1);
    }
    // You may create a dummy window/context here if needed for loader
}

void Program::InitialiseWindow() 
{
    m_window = Window::Create("Game", static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight));
    m_graphicsContext = Window::CreateGraphicsContext(m_window);
    Window::MakeContextCurrent(m_window, m_graphicsContext);
}


// Compiles and links vertex + fragment shaders into a program and stores it under a given name
void Program::CreateGraphicsPipeline(const std::string& name, const std::string& vsPath, const std::string& fsPath)
{
    // Load shader source files
    std::string vs = LoadShader(vsPath);
    std::string fs = LoadShader(fsPath);

    // Create the shader program and store it in the map
    m_shaderPrograms[name] = CreateShaderProgram(vs, fs);
}

// Retrieves the OpenGL shader program associated with a name
unsigned int Program::GetShaderProgram(const std::string& name) const
{
    auto it = m_shaderPrograms.find(name);
    if (it != m_shaderPrograms.end())
    {
        return it->second;
    }

    std::cerr << "Shader program \"" << name << "\" not found\n";
    return 0; // Return 0 if program not found
}

// Frees all OpenGL shader programs and destroys the window
void Program::Delete()
{
    // Delete all compiled shader programs
    for (auto& pair : m_shaderPrograms)
    {
        Graphics::DeleteProgram(pair.second);
    }

    m_shaderPrograms.clear();

    // Destroy the SDL window
    Window::Destroy(m_window);
    m_window = nullptr;
}

void Program::DeleteWindow() 
{
    if (m_window) 
    {
        Window::Destroy(m_window);
        m_window = nullptr;
    }
}

void Program::DeleteGraphics() 
{
    DeleteWindow(); // Optionally also destroy window
    for (auto& pair : m_shaderPrograms)
    {
        Graphics::DeleteProgram(pair.second);
    }

    m_shaderPrograms.clear();
    m_graphicsContext = nullptr;
    // If needed: call any GL context cleanup here
}


// Locks or unlocks the mouse to the center of the screen (useful for FPS controls)
void Program::LockMouse(bool lock) const
{
    Window::LockMouse(m_window, lock, static_cast<int>(m_screenWidth), static_cast<int>(m_screenHeight));
}

// Loads the contents of a shader file into a string
std::string Program::LoadShader(const std::string& fileName)
{
    std::ostringstream ss;
    std::ifstream file(fileName);

    // Fail and exit if file can't be opened
    if (!file.is_open())
    {
        std::cout << "Failed to load shader: " << fileName << "\n";
        exit(1);
    }

    // Stream the entire file into the string stream
    ss << file.rdbuf();
    return ss.str();
}

// Verifies shader compilation and prints log if there is an error
void Program::CheckShaderCompile(unsigned int shader) {
    std::string log;
    if (!Graphics::GetShaderCompileStatus(shader, &log)) 
    {
        std::cout << "Shader Compilation Error:\n" << log << std::endl;
    }
}

// Verifies shader program linking and prints log if there is an error
void Program::CheckProgramLink(unsigned int program) 
{
    std::string log;
    if (!Graphics::GetProgramLinkStatus(program, &log)) 
    {
        std::cout << "Shader Linking Error:\n" << log << std::endl;
    }
}

// Compiles a single shader of the given type (vertex or fragment) from source
unsigned int Program::CompileShader(unsigned int type, const std::string& source) 
{
    // Create shader object
    unsigned int shader = Graphics::CreateShader(static_cast<ShaderType>(type));

    // Set the source code and compile
    Graphics::SetShaderSource(shader, source);
    Graphics::CompileShader(shader);

    // Check for compilation errors
    CheckShaderCompile(shader);
    return shader;
}

// Creates a complete shader program by compiling and linking vertex + fragment shaders
unsigned int Program::CreateShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc) 
{
    // Create the shader program
    unsigned int program = Graphics::CreateProgram();

    // Compile vertex and fragment shaders
    unsigned int vertexShader = CompileShader(static_cast<unsigned int>(ShaderType::Vertex), vertexSrc);
    unsigned int fragmentShader = CompileShader(static_cast<unsigned int>(ShaderType::Fragment), fragmentSrc);

    // Attach shaders to program and link
    Graphics::AttachShader(program, vertexShader);
    Graphics::AttachShader(program, fragmentShader);
    Graphics::LinkProgram(program);

    // Shaders can be deleted after linking
    Graphics::DeleteShader(vertexShader);
    Graphics::DeleteShader(fragmentShader);

    // Check for linking errors
    CheckProgramLink(program);

    // Optionally log number of active uniforms
    int uniforms = Graphics::GetActiveUniformCount(program);

    return program;
}

// Updates m_deltaTime by measuring the time between this and the last frame
void Program::UpdateDeltaTime()
{
    // Get current time in seconds
    float currentTime = Time::GetTimeSeconds();

    // Calculate delta time between frames
    m_deltaTime = currentTime - m_lastFrameTime;

    // Store current time for next frame
    m_lastFrameTime = currentTime;
}

// Returns time between current and last frame (in seconds)
float Program::GetDeltaTime() const
{
    return m_deltaTime;
}

// Swaps OpenGL back buffer with front buffer (displays rendered image)
void Program::SwapWindow()
{
    Window::SwapBuffers(m_window);
}

void Program::SetQuit(bool quit)
{
    m_quit = quit;
}

bool Program::GetQuit() const
{
    return m_quit;
}

float Program::GetScreenWidth() const
{
    return m_screenWidth;
}

float Program::GetScreenHeight() const
{
    return m_screenHeight;
}