#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

// === OpenGL ===
#include <glad/gl.h>
#include <string>
#include "../Math/math.hpp"

// === Vertex Attribute Layout Locations ===
#define POSITION_LOCATION 0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION 2

// === Texture Units ===
#define COLOR_TEXTURE_UNIT GL_TEXTURE0
#define COLOR_TEXTURE_UNIT_INDEX_0 0

// === RGB Color Structure ===
struct Color 
{
    float r;
    float g;
    float b;
};

// === Engine-Level Enum Abstractions for GL Constants ===
enum class Capability : unsigned int 
{
    CullFace = 0x0B44,  // GL_CULL_FACE
    DepthTest = 0x0B71,  // GL_DEPTH_TEST
    Blend = 0x0BE2   // GL_BLEND
};

enum class CullFaceMode : unsigned int 
{
    Front = 0x0404,  // GL_FRONT
    Back = 0x0405,  // GL_BACK
    FrontAndBack = 0x0408 // GL_FRONT_AND_BACK
};

enum class FrontFaceOrder : unsigned int 
{
    CW = 0x0900,  // GL_CW
    CCW = 0x0901   // GL_CCW
};

enum class DrawMode : unsigned int 
{
    Triangles = 0x0004,  // GL_TRIANGLES
    Lines = 0x0001   // GL_LINES
};

enum class DataType : unsigned int 
{
    UnsignedByte = 0x1401,  // GL_UNSIGNED_BYTE
    UnsignedShort = 0x1403,  // GL_UNSIGNED_SHORT
    UnsignedInt = 0x1405,   // GL_UNSIGNED_INT
    Float = 0x1406  // GL_FLOAT
};

enum class BufferTarget : unsigned int 
{
    ArrayBuffer = 0x8892,             // GL_ARRAY_BUFFER
    ElementArrayBuffer = 0x8893       // GL_ELEMENT_ARRAY_BUFFER
};

// Buffer usage patterns
enum class BufferUsage : unsigned int 
{
    StaticDraw = 0x88E4,              // GL_STATIC_DRAW
    DynamicDraw = 0x88E8              // GL_DYNAMIC_DRAW (optional)
};

enum class BufferBit : unsigned int 
{
    Color = 0x00004000,
    Depth = 0x00000100,
    Stencil = 0x00000400
};

inline unsigned int operator|(BufferBit a, BufferBit b) 
{
    return static_cast<unsigned int>(a) | static_cast<unsigned int>(b);
}

enum class ShaderType : unsigned int 
{
    Vertex = 0x8B31,  // GL_VERTEX_SHADER
    Fragment = 0x8B30   // GL_FRAGMENT_SHADER
    // Add geometry, compute, etc. as needed
};

enum class TextureTarget : unsigned int 
{
    Texture2D = 0x0DE1,       // GL_TEXTURE_2D
    CubeMap = 0x8513        // GL_TEXTURE_CUBE_MAP (optional)
};

enum class TextureFormat : unsigned int 
{
    Red = 0x1903, // GL_RED
    RGB = 0x1907, // GL_RGB
    RGBA = 0x1908  // GL_RGBA
};

namespace Graphics 
{

    // === Facaded OpenGL Functions ===
    inline void ClearColor(float r, float g, float b, float a) 
    {
        glClearColor(r, g, b, a);
    }

    inline void Viewport(int x, int y, float width, float height) 
    {
        glViewport(x, y, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    inline void Clear(unsigned int mask) 
    {
        glClear(static_cast<GLbitfield>(mask));
    }

    inline void Enable(Capability cap) 
    {
        glEnable(static_cast<GLenum>(cap));
    }

    inline void Disable(Capability cap) 
    {
        glDisable(static_cast<GLenum>(cap));
    }

    inline void SetCullFace(CullFaceMode mode) 
    {
        glCullFace(static_cast<GLenum>(mode));
    }

    inline void SetFrontFace(FrontFaceOrder order) 
    {
        glFrontFace(static_cast<GLenum>(order));
    }

    inline void SetDepthWrite(bool enable) 
    {
        glDepthMask(enable ? GL_TRUE : GL_FALSE);
    }

    inline void UseProgram(unsigned int program) 
    {
        glUseProgram(static_cast<GLuint>(program));
    }

    inline int GetUniformLocation(unsigned int programID, const std::string& name) 
    {
        return glGetUniformLocation(programID, name.c_str());
    }

    inline void SetUniformMat4(int location, const float* matrixPtr) 
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, matrixPtr);
    }

    inline void SetUniform1f(int location, float value) 
    {
        glUniform1f(location, value);
    }

    inline void SetUniform1i(int location, int value) 
    {
        glUniform1i(location, value);
    }

    inline void SetUniform3f(int location, float x, float y, float z) 
    {
        glUniform3f(location, x, y, z);
    }

    inline void GenVertexArray(unsigned int& vao) 
    {
        glGenVertexArrays(1, &vao);
    }

    inline void GenBuffer(unsigned int& buffer) 
    {
        glGenBuffers(1, &buffer);
    }

    inline void BufferSubData(BufferTarget target, size_t offset, size_t size, const void* data) 
    {
        glBufferSubData(static_cast<GLenum>(target), static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
    }


    inline void BindVertexArray(unsigned int vao) 
    {
        glBindVertexArray(vao);
    }

    inline void BindBuffer(BufferTarget target, unsigned int buffer) 
    {
        glBindBuffer(static_cast<GLenum>(target), buffer);
    }

    inline void UnbindVertexArray() 
    {
        glBindVertexArray(0);
    }

    inline void UnbindBuffer(BufferTarget target) 
    {
        glBindBuffer(static_cast<GLenum>(target), 0);
    }

    inline void BufferData(BufferTarget target, size_t size, const void* data, BufferUsage usage) 
    {
        glBufferData(static_cast<GLenum>(target), static_cast<GLsizeiptr>(size), data, static_cast<GLenum>(usage));
    }

    inline void EnableVertexAttrib(unsigned int location) 
    {
        glEnableVertexAttribArray(location);
    }

    inline void SetVertexAttribPointer(unsigned int location, int componentCount, DataType type, bool normalized, size_t stride, const void* offset)
    {
        glVertexAttribPointer(
            location,
            componentCount,
            static_cast<GLenum>(type),
            normalized ? GL_TRUE : GL_FALSE,
            static_cast<GLsizei>(stride),
            offset
        );
    }

    inline void DeleteBuffer(unsigned int& bufferID) 
    {
        if (bufferID != 0) 
        {
            glDeleteBuffers(1, &bufferID);
            bufferID = 0; // Optional: reset to avoid dangling handles
        }
    }

    inline void DeleteVertexArray(unsigned int& vaoID) 
    {
        if (vaoID != 0) 
        {
            glDeleteVertexArrays(1, &vaoID);
            vaoID = 0; // Optional: reset to avoid dangling handles
        }
    }

    inline void GenTexture(unsigned int& id) 
    {
        glGenTextures(1, &id);
    }

    inline void BindTexture(TextureTarget target, unsigned int textureID) 
    {
        glBindTexture(static_cast<GLenum>(target), textureID);
    }

    inline void ActiveTextureUnit(unsigned int unit) 
    {
        glActiveTexture(unit);
    }

    inline void TexImage2D(TextureTarget target, int width, int height, TextureFormat format, const void* data) 
    {
        glTexImage2D(static_cast<GLenum>(target), 0, static_cast<GLint>(format),
            width, height, 0,
            static_cast<GLenum>(format), GL_UNSIGNED_BYTE, data);
    }

    inline void GenerateMipmaps(TextureTarget target) 
    {
        glGenerateMipmap(static_cast<GLenum>(target));
    }

    inline void DeleteTexture(unsigned int& id) 
    {
        if (id) 
        {
            glDeleteTextures(1, &id);
            id = 0;
        }
    }

    inline void DrawElements(DrawMode mode, int count, DataType type, const void* indices) 
    {
        glDrawElements(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices);
    }

    inline void DrawElementsBaseVertex(DrawMode mode, int count, DataType type, const void* indices, int baseVertex) 
    {
        glDrawElementsBaseVertex(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices, baseVertex);
    }

    inline unsigned int GetCurrentVertexArray() 
    {
        GLint vao = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
        return static_cast<unsigned int>(vao);
    }

    inline void UnbindProgram() 
    {
        glUseProgram(0);
    }

    inline void DrawArrays(DrawMode mode, int first, int count) 
    {
        glDrawArrays(
            static_cast<GLenum>(mode),
            static_cast<GLint>(first),
            static_cast<GLsizei>(count)
        );
    }

    inline unsigned int GetError() 
    {
        return static_cast<unsigned int>(glGetError());
    }

    inline bool CheckGraphicsError() 
    {
        return GetError() == static_cast<unsigned int>(GL_NO_ERROR);
    }

    inline unsigned int CreateShader(ShaderType type) 
    {
        return glCreateShader(static_cast<GLenum>(type));
    }

    inline void SetShaderSource(unsigned int shaderID, const std::string& source) 
    {
        const char* src = source.c_str();
        glShaderSource(shaderID, 1, &src, nullptr);
    }

    inline void CompileShader(unsigned int shaderID) 
    {
        glCompileShader(shaderID);
    }

    inline bool GetShaderCompileStatus(unsigned int shaderID, std::string* outLog = nullptr) 
    {
        int success = 0;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

        if (!success && outLog) 
        {
            char infoLog[512];
            glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
            *outLog = infoLog;
        }

        return success != 0;
    }

    inline unsigned int CreateProgram() 
    {
        return glCreateProgram();
    }

    inline void AttachShader(unsigned int programID, unsigned int shaderID) 
    {
        glAttachShader(programID, shaderID);
    }

    inline void LinkProgram(unsigned int programID) 
    {
        glLinkProgram(programID);
    }

    inline bool GetProgramLinkStatus(unsigned int programID, std::string* outLog = nullptr) 
    {
        int success = 0;
        glGetProgramiv(programID, GL_LINK_STATUS, &success);

        if (!success && outLog) 
        {
            char infoLog[512];
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
            *outLog = infoLog;
        }

        return success != 0;
    }

    inline unsigned int CreateEmptyBuffer(size_t size) 
    {
        unsigned int newBuffer;
        glGenBuffers(1, &newBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, newBuffer);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        return newBuffer;
    }

    inline void OverridePositionVBO(unsigned int vbo, int location = POSITION_LOCATION) 
    {
        if (vbo == 0)
        {
            return;
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(
            location,                  // attribute location (default: POSITION_LOCATION)
            3,                         // 3 components per vertex
            static_cast<GLenum>(DataType::Float), // type = float
            GL_FALSE,                  // not normalized
            sizeof(Vector3f),          // stride
            static_cast<void*>(0)      // offset
        );
        glEnableVertexAttribArray(location);
    }



    inline int GetActiveUniformCount(unsigned int programID) 
    {
        int count = 0;
        glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &count);
        return count;
    }

    inline void DeleteShader(unsigned int shaderID) 
    {
        glDeleteShader(shaderID);
    }

    inline void DeleteProgram(unsigned int& programID) 
    {
        if (programID != 0) 
        {
            glDeleteProgram(programID);
            programID = 0; // avoid dangling reference
        }
    }

} // namespace Graphics

#endif // GRAPHICS_HPP
