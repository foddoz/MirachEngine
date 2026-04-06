#pragma once

#include <glad/gl.h>
#include "../../Math/math.hpp"
#include <string>       
#include <fstream>    
#include <sstream>       
#include <iostream>     

class CrossAim 
{
public:
    CrossAim();
    void Init();                        
    void Render(); 
    void Shutdown();    

private:
    GLuint m_vao, m_vbo, m_shaderProgram;
};
