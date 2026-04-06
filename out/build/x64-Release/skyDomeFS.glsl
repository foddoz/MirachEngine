#version 330

in vec2 Tex;

out vec4 FragColor;

uniform sampler2D gSampler;

void main()
{
    FragColor = texture(gSampler, Tex);
}