#include "image_renderer.hpp"
#include <cstdio>
#include <fstream>
#include <sstream>

std::string ImageRenderer::ReadTextFile(const std::string& path) 
{
    std::ifstream ifs(path, std::ios::in);
    if (!ifs) 
    {
        throw std::runtime_error("Failed to open shader file: " + path);
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

unsigned int ImageRenderer::CompileProgramFromFiles(const std::string& vsPath, const std::string& fsPath) 
{
    std::string vs, fs;
    try 
    {
        vs = ReadTextFile(vsPath);
        fs = ReadTextFile(fsPath);
    }
    catch (const std::exception& e) 
    {
        std::fprintf(stderr,"[ImageRenderer] Shader file read failed: %s\n", e.what());
        return 0;
    }

    if (vs.empty() || fs.empty()) 
    {
        std::fprintf(stderr,"[ImageRenderer] Shader file empty: vs(%zu), fs(%zu)\n",vs.size(), fs.size());
        return 0;
    }

    return CompileProgram(vs, fs);
}

unsigned int ImageRenderer::CompileProgram(const std::string& vs, const std::string& fs) 
{
    using namespace Graphics;

    unsigned int v = CreateShader(ShaderType::Vertex);
    SetShaderSource(v, std::string(vs));
    CompileShader(v);
    std::string vlog;
    if (!GetShaderCompileStatus(v, &vlog)) 
    {
        std::fprintf(stderr, "[ImageRenderer] VS compile error:\n%s\n", vlog.c_str());
    }

    unsigned int f = CreateShader(ShaderType::Fragment);
    SetShaderSource(f, std::string(fs));
    CompileShader(f);
    std::string flog;
    if (!GetShaderCompileStatus(f, &flog)) 
    {
        std::fprintf(stderr, "[ImageRenderer] FS compile error:\n%s\n", flog.c_str());
    }

    unsigned int p = CreateProgram();
    AttachShader(p, v);
    AttachShader(p, f);
    LinkProgram(p);
    std::string plog;
    if (!GetProgramLinkStatus(p, &plog)) 
    {
        std::fprintf(stderr, "[ImageRenderer] Link error:\n%s\n", plog.c_str());
    }

    DeleteShader(v);
    DeleteShader(f);
    return p;
}

void ImageRenderer::SetScreenSize(int w, int h) 
{ 
    m_screenW = w; 
    m_screenH = h; 
}

/*
*
* (1)Vertex data (e.g., position, UV coordinates) is provided by the CPU and stored in the VBO (Vertex Buffer Object).
* (2)The drawing order (which vertices form triangles) is defined in the EBO (Element Buffer Object).
* (3)VAO (Vertex Array Object) stores the state of the VBO/EBO bindings and vertex attribute configurations.
* (4)A shader program receives the vertex attributes (e.g., position, UV), processes them in the vertex shader, then passes data to the fragment shader to output final color.
* (5)DrawElements() executes the actual drawing using this information.
*
*/
bool ImageRenderer::Init(int screenW, int screenH, const std::string& vsPath, const std::string& fsPath) 
{
    using namespace Graphics;

    // Store screen dimensions
    m_screenW = screenW;
    m_screenH = screenH;

    // Enable blending (for transparency effects)
    Enable(Capability::Blend);

    // Compile vertex and fragment shaders, and link them into a shader program
    if (!vsPath.empty() && !fsPath.empty())
    {
        m_prog = CompileProgramFromFiles(vsPath, fsPath);
    }
    if (!m_prog)
    {
        return false;
    }

    // Define the quad vertex data: positions (aPos) and texture coordinates (aUV)
    // In VBO
    //| 0,0, 0,0 | 1,0, 1,0 | 1,1, 1,1 | 0,1, 0,1 |
    //    index 0 | index 1 | index 2 | index 3
    // In EBO 
    //Triangle 1 | Triangle 2
    //  { 0, 1, 2, 0, 2, 3 }
    const float verts[] = 
    {
        // aPos   // aUV
        0.f, 0.f,  0.f, 0.f, //index 0
        1.f, 0.f,  1.f, 0.f, //index 1
        1.f, 1.f,  1.f, 1.f, //index 2
        0.f, 1.f,  0.f, 1.f, //index 3
    };

    // Define the two triangles (6 indices) that make up the quad
    // £Õse for EBO
    const unsigned int idx[] = { 0,1,2, 0,2,3 };

    // Enable OpenGL blending with standard alpha transparency mode
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Generate vertex array object (VAO), vertex buffer object (VBO), and element buffer object (EBO)
    GenVertexArray(m_vao);
    GenBuffer(m_vbo);
    GenBuffer(m_ebo);

    // Bind the VAO to store all vertex attribute and buffer state
    BindVertexArray(m_vao);

    // Upload vertex data to the VBO
    BindBuffer(BufferTarget::ArrayBuffer, m_vbo);
    BufferData(BufferTarget::ArrayBuffer, sizeof(verts), verts, BufferUsage::StaticDraw);

    // Upload index data to the EBO
    BindBuffer(BufferTarget::ElementArrayBuffer, m_ebo);
    BufferData(BufferTarget::ElementArrayBuffer, sizeof(idx), idx, BufferUsage::StaticDraw);

    // Enable vertex attribute for position coords (location = 0), next 2 floats per vertex
    // Tell GPU how to read each vertex¡¯s layout
    EnableVertexAttrib(POSITION_LOCATION);
    //POSITION_LOCATION is the location of layout(location = 0) in Shader
    //Each vertices position is actually two float number
    //Data type is float
    //Each vertices take size of 4 float(x,y + u,v) stride
    //(void*)0 | offset start read from zero bytes 
    SetVertexAttribPointer(POSITION_LOCATION, 2, DataType::Float, false, sizeof(float) * 4, (void*)0);

    // Enable vertex attribute for UV coords(location = 1), next 2 floats per vertex
    EnableVertexAttrib(TEX_COORD_LOCATION);
    SetVertexAttribPointer(TEX_COORD_LOCATION, 2, DataType::Float, false, sizeof(float) * 4, (void*)(sizeof(float) * 2));

    // Unbind VAO and VBO to avoid accidental modification later
    UnbindVertexArray();
    UnbindBuffer(BufferTarget::ArrayBuffer);

    // Return true if shader program was successfully compiled and linked
    return m_prog != 0;
}

void ImageRenderer::Shutdown() 
{
    using namespace Graphics;
    if (m_ebo) DeleteBuffer(m_ebo);
    if (m_vbo) DeleteBuffer(m_vbo);
    if (m_vao) DeleteVertexArray(m_vao);
    if (m_prog) DeleteProgram(m_prog);

    if (m_eboWorld) DeleteBuffer(m_eboWorld);
    if (m_vboWorld) DeleteBuffer(m_vboWorld);
    if (m_vaoWorld) DeleteVertexArray(m_vaoWorld);
    if (m_progWorld)DeleteProgram(m_progWorld);

    if (m_eboBB)    DeleteBuffer(m_eboBB);
    if (m_vboBB)    DeleteBuffer(m_vboBB);
    if (m_vaoBB)    DeleteVertexArray(m_vaoBB);
    if (m_progBB)   DeleteProgram(m_progBB);

    m_ebo = m_vbo = m_vao = m_prog = 0;
    m_eboWorld = m_vboWorld = m_vaoWorld = m_progWorld = 0;
    m_eboBB = m_vboBB = m_vaoBB = m_progBB = 0;
}

void ImageRenderer::Draw(Texture& tex, float x, float y, float w, float h) 
{
    using namespace Graphics;

    // Using the default size of the texture image
    if (w <= 0.f)
    {
        w = static_cast<float>(tex.GetWidth());
    }
    if (h <= 0.f)
    {
        h = static_cast<float>(tex.GetHeight());
    }

    // Activate texture unit 0 and bind texture to it
    ActiveTextureUnit(COLOR_TEXTURE_UNIT);
    tex.Bind(COLOR_TEXTURE_UNIT_INDEX_0);

    // Use compiled shader program
    UseProgram(m_prog);
    //Bind VAO(contains vertex and index data)
    BindVertexArray(m_vao);

    //Get uniform locations from shader
    const int locRect = GetUniformLocation(m_prog, std::string("uRect"));
    const int locScreen = GetUniformLocation(m_prog, std::string("uScreen"));
    const int locTex = GetUniformLocation(m_prog, std::string("uTex"));

    //Set rectangle screen position and size (x, y, w, h)
    glUniform4f(locRect, x, y, w, h);
    // Set screen size for NDC conversion in vertex shader
    glUniform2f(locScreen, (float)m_screenW, (float)m_screenH);
    //Tell shader to use texture unit 0 for uTex
    SetUniform1i(locTex, 0);

    //Draw 2 triangles (6 indices total)
    DrawElements(DrawMode::Triangles, 6, DataType::UnsignedInt, nullptr);

    //Unbind VAO and shader program to avoid side effects
    UnbindVertexArray();
    UnbindProgram();
}

// Initialize a textured quad in 3D world space (can be transformed by model/view/projection)
bool ImageRenderer::InitWorldQuad(const std::string& vsPath, const std::string& fsPath) 
{
    using namespace Graphics;

    // 1) Try to load shaders from file; fallback to built-in ones if missing
    if (!vsPath.empty() && !fsPath.empty())
    {
        m_progWorld = CompileProgramFromFiles(vsPath, fsPath);
    }
    if (!m_progWorld)
    {
        return false;
    }

    // 2) Vertex data layout: position(x, y, z) + UV(u, v)
    //    Quad is centered at origin on XY plane, facing +Z
    const float verts[] = {
        -0.5f,-0.5f,0.f,  0.f,0.f,
         0.5f,-0.5f,0.f,  1.f,0.f,
         0.5f, 0.5f,0.f,  1.f,1.f,
        -0.5f, 0.5f,0.f,  0.f,1.f,
    };
    const unsigned int idx[] = { 0,1,2, 0,2,3 }; // Two triangles

    // 3) Generate and upload VAO/VBO/EBO data
    GenVertexArray(m_vaoWorld); GenBuffer(m_vboWorld); GenBuffer(m_eboWorld);
    BindVertexArray(m_vaoWorld);

    BindBuffer(BufferTarget::ArrayBuffer, m_vboWorld);
    BufferData(BufferTarget::ArrayBuffer, sizeof(verts), verts, BufferUsage::StaticDraw);

    BindBuffer(BufferTarget::ElementArrayBuffer, m_eboWorld);
    BufferData(BufferTarget::ElementArrayBuffer, sizeof(idx), idx, BufferUsage::StaticDraw);

    // 4) Attribute layout: location 0 = position, location 1 = UV
    EnableVertexAttrib(0);
    SetVertexAttribPointer(0, 3, DataType::Float, false, sizeof(float) * 5, (void*)0);

    EnableVertexAttrib(1);
    SetVertexAttribPointer(1, 2, DataType::Float, false, sizeof(float) * 5, (void*)(sizeof(float) * 3));

    // 5) Unbind to avoid accidental modifications later
    UnbindVertexArray();
    UnbindBuffer(BufferTarget::ArrayBuffer);
    return true;
}

// Draw a 3D quad using given model/view/projection matrices
void ImageRenderer::DrawQuad3D(Texture& tex, const float* model, const float* view, const float* proj) 
{
    using namespace Graphics;
    if (!m_progWorld)
    {
        return;
    }

    // Enable depth testing and alpha blending
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind texture and shader program
    ActiveTextureUnit(0);
    tex.Bind(0);
    UseProgram(m_progWorld);
    BindVertexArray(m_vaoWorld);

    // Set shader uniforms
    int locM = GetUniformLocation(m_progWorld, "uModel");
    int locV = GetUniformLocation(m_progWorld, "uView");
    int locP = GetUniformLocation(m_progWorld, "uProj");
    int locT = GetUniformLocation(m_progWorld, "uTex");

    glUniformMatrix4fv(locM, 1, GL_FALSE, model);
    glUniformMatrix4fv(locV, 1, GL_FALSE, view);
    glUniformMatrix4fv(locP, 1, GL_FALSE, proj);
    SetUniform1i(locT, 0); // Texture unit 0

    // Render two triangles (6 indices)
    DrawElements(DrawMode::Triangles, 6, DataType::UnsignedInt, nullptr);

    // Cleanup
    UnbindVertexArray();
    UnbindProgram();
}

// Initialize a ¡°billboard¡± image that always faces the player
bool ImageRenderer::InitImageFaceToPlayer(const std::string& vsPath, const std::string& fsPath) 
{
    using namespace Graphics;

    // Load shaders or fallback
    if (!vsPath.empty() && !fsPath.empty())
    {
        m_progBB = CompileProgramFromFiles(vsPath, fsPath);
    }
    if (!m_progBB)
    {
        return false;
    }

    // Vertex layout: local XY + UV (rotation handled in shader)
    const float verts[] = 
    {
        -0.5f,-0.5f,  0.f,0.f,
         0.5f,-0.5f,  1.f,0.f,
         0.5f, 0.5f,  1.f,1.f,
        -0.5f, 0.5f,  0.f,1.f,
    };
    const unsigned int idx[] = { 0,1,2, 0,2,3 };

    // Generate buffers
    GenVertexArray(m_vaoBB); GenBuffer(m_vboBB); GenBuffer(m_eboBB);
    BindVertexArray(m_vaoBB);

    BindBuffer(BufferTarget::ArrayBuffer, m_vboBB);
    BufferData(BufferTarget::ArrayBuffer, sizeof(verts), verts, BufferUsage::StaticDraw);

    BindBuffer(BufferTarget::ElementArrayBuffer, m_eboBB);
    BufferData(BufferTarget::ElementArrayBuffer, sizeof(idx), idx, BufferUsage::StaticDraw);

    // Attribute layout: location 0 = XY, location 1 = UV
    EnableVertexAttrib(0);
    SetVertexAttribPointer(0, 2, DataType::Float, false, sizeof(float) * 4, (void*)0);

    EnableVertexAttrib(1);
    SetVertexAttribPointer(1, 2, DataType::Float, false, sizeof(float) * 4, (void*)(sizeof(float) * 2));

    UnbindVertexArray();
    UnbindBuffer(BufferTarget::ArrayBuffer);
    return true;
}

// Render a image that faces the camera
// position = world-space center of the image
// size = world-space width and height
void ImageRenderer::DrawImageFaceToPlayer(Texture& tex,Vector3f position,Vector2f size, const float* view, const float* proj)
{
    using namespace Graphics;
    if (!m_progBB)
    {
        return;
    }

    // Enable depth test and blending for transparency
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind texture and program
    ActiveTextureUnit(0);
    tex.Bind(0);
    UseProgram(m_progBB);
    BindVertexArray(m_vaoBB);

    // Set shader uniforms
    int locC = GetUniformLocation(m_progBB, "uCenterWS");
    int locS = GetUniformLocation(m_progBB, "uSize");
    int locV = GetUniformLocation(m_progBB, "uView");
    int locP = GetUniformLocation(m_progBB, "uProj");
    int locT = GetUniformLocation(m_progBB, "uTex");

    glUniform3f(locC, position.x, position.y, position.z);
    glUniform2f(locS, size.x, size.y);
    glUniformMatrix4fv(locV, 1, GL_FALSE, view);
    glUniformMatrix4fv(locP, 1, GL_FALSE, proj);
    SetUniform1i(locT, 0); // Texture unit 0

    // Draw the billboard
    DrawElements(DrawMode::Triangles, 6, DataType::UnsignedInt, nullptr);

    // Cleanup
    UnbindVertexArray();
    UnbindProgram();
}

