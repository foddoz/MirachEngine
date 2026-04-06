#version 330



uniform sampler2D gTextureHeight0;
uniform sampler2D gTextureHeight1;
uniform sampler2D gTextureHeight2;
uniform sampler2D gTextureHeight3;

uniform float gMinHeight;
uniform float gMaxHeight;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 InTex;
layout (location = 2) in vec3 Normal;

uniform mat4 gWVP;

out vec4 Color;
out vec2 Tex;
out vec3 WorldPos;

out vec3 FragNormal;

void main()
{
    gl_Position = gWVP * vec4(Position, 1.0);

    float DeltaHeight = gMaxHeight - gMinHeight;

    float HeightRatio = (Position.y - gMinHeight) / DeltaHeight;

    float c = HeightRatio * 0.8 + 0.2;

    Color = vec4(c, c, c, 1.0);

    Tex = InTex;
    
    WorldPos = Position;

    FragNormal = Normal;
}