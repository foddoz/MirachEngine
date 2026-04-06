#include "crossAim.hpp"
#include <glad/gl.h>
#include <iostream>
#include <algorithm>

const char* crosshairVS = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)glsl";

const char* crosshairFS = R"glsl(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
void main() {
    FragColor = vec4(uColor, 1.0);
}
)glsl";

static float s_size = 0.001f;
static float s_thick = 0.015f;
static float s_gap = 0.0f;

CrossAim::CrossAim()
    :
    m_vao(0),
    m_vbo(0),
    m_shaderProgram(0)
{

}

void CrossAim::Init() 
{

    const float t = s_thick;
    const float L = s_size;
    const float g = s_gap;

    float vx_top[8] = 
    {
    -t * 0.5f,  g + L,
    -t * 0.5f,  g,
     t * 0.5f,  g,
     t * 0.5f,  g + L
    };

    float vx_bot[8] = 
    {
    -t * 0.5f, -g,
    -t * 0.5f, -g + L,
        t * 0.5f, -g + L ,
        t * 0.5f, -g
    };

    float vx_left[8] = 
    {
    -g - L ,  t * 0.5f,
    -g - L , -t * 0.5f,
    -g, -t * 1.f,
    -g,  t * 1.f
    };

    float vx_right[8] = 
    {
       g,   t * 0.5f,
       g,  -t * 0.5f,
       g + L, -t * 1.f,
       g + L,  t * 1.f
    };

    float verts[32];
    std::memcpy(verts + 0, vx_top, sizeof(vx_top));
    std::memcpy(verts + 8, vx_bot, sizeof(vx_bot));
    std::memcpy(verts + 16, vx_left, sizeof(vx_left));
    std::memcpy(verts + 24, vx_right, sizeof(vx_right));

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &crosshairVS, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &crosshairFS, nullptr);
    glCompileShader(fs);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vs);
    glAttachShader(m_shaderProgram, fs);
    glLinkProgram(m_shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void CrossAim::Render() 
{
    glUseProgram(m_shaderProgram);

    GLint locColor = glGetUniformLocation(m_shaderProgram, "uColor");
    glUniform3f(locColor, 0.0f, 0.0f, 0.0f);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); 
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4); 
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4); 
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void CrossAim::Shutdown() 
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteProgram(m_shaderProgram);
}
