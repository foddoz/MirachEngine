#ifndef IMAGE_RENDERER_HPP
#define IMAGE_RENDERER_HPP

#pragma once
#include <string>
#include <vector>
#include "../graphics.hpp"
#include "../Texturing/texture.hpp"
#include "../../Math/math.hpp"

class ImageRenderer 
{
public:
    // Initialize the image size that passing to the screen
    bool Init(int screenW, int screenH, const std::string& vsPath = "", const std::string& fsPath = "");

    //For spawn in the 3D world
    bool InitWorldQuad(const std::string& vsPath = "", const std::string& fsPath = "");

    //For spawn the image aways face to player
    bool InitImageFaceToPlayer(const std::string& vsPath = "", const std::string& fsPath = "");

    void Shutdown();

    // Draw a rectangle on the screen Pixel Coordinate Top left is (0,0)
    // If  w/h <= 0, then automatically use the original width height of the texture
    void Draw(Texture& tex, float x, float y, float w = 0.f, float h = 0.f);

    void DrawQuad3D(Texture& tex,const float* model,const float* view,const float* proj);

    void DrawImageFaceToPlayer(Texture& tex,Vector3f position, Vector2f size,const float* view,const float* proj);

    // Can update the image size while screen size 
    void SetScreenSize(int w, int h);

private:
    unsigned int m_vao{ 0 };
    unsigned int m_vbo{ 0 };
    unsigned int m_ebo{ 0 };
    unsigned int m_prog{ 0 };

    int m_screenW{ 0 };
    int m_screenH{ 0 };

    unsigned int m_progWorld{ 0 };
    unsigned int m_vaoWorld{ 0 };
    unsigned int m_vboWorld{ 0 };
    unsigned int m_eboWorld{ 0 };

    unsigned int m_progBB{ 0 };
    unsigned int m_vaoBB{ 0 };
    unsigned int m_vboBB{ 0 };
    unsigned int m_eboBB{ 0 };

    //Reading shader file
    static std::string ReadTextFile(const std::string& path);

    unsigned int CompileProgramFromFiles(const std::string& vsPath, const std::string& fsPath);
    unsigned int CompileProgram(const std::string& vs, const std::string& fs);
};

#endif