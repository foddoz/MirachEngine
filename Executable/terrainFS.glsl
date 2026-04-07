#version 330

layout(location = 0) out vec4 FragColor;

uniform vec3 gLightDir;
uniform vec3 gLightColor;

in vec4 Color;
in vec2 Tex;
in vec3 WorldPos;
in vec3 FragNormal;

uniform sampler2D gTextureHeight0;
uniform sampler2D gTextureHeight1;
uniform sampler2D gTextureHeight2;
uniform sampler2D gTextureHeight3;

uniform float gMinHeight;
uniform float gMaxHeight;

vec4 CalcTexColor()
{
    float HeightRatio = clamp((WorldPos.y - gMinHeight) / (gMaxHeight - gMinHeight), 0.0, 1.0);

    vec4 TexColor;

    if (HeightRatio < 0.25) {
        TexColor = texture(gTextureHeight0, Tex); // concrete
    }
    else if (HeightRatio < 0.4) {
        float Factor = (HeightRatio - 0.25) / (0.4 - 0.25);
        vec4 Color0 = texture(gTextureHeight0, Tex);
        vec4 Color1 = texture(gTextureHeight1, Tex);
        TexColor = mix(Color0, Color1, Factor);
    }
    else if (HeightRatio < 0.7) {
        float Factor = (HeightRatio - 0.4) / (0.7 - 0.4);
        vec4 Color0 = texture(gTextureHeight1, Tex);
        vec4 Color1 = texture(gTextureHeight2, Tex);
        TexColor = mix(Color0, Color1, Factor);
    }
    else if (HeightRatio < 0.95) {
        float Factor = (HeightRatio - 0.7) / (0.95 - 0.7);
        vec4 Color0 = texture(gTextureHeight2, Tex);
        vec4 Color1 = texture(gTextureHeight3, Tex);
        TexColor = mix(Color0, Color1, Factor);
    }
    else {
        TexColor = texture(gTextureHeight3, Tex); // snow/water at top
    }

    return TexColor;
}

void main()
{
    vec3 N = normalize(FragNormal);
    vec3 L = normalize(gLightDir);

    float DiffuseFactor = max(dot(N, -L), 0.0);
    vec3 DiffuseColor = gLightColor * DiffuseFactor;

    vec4 TexColor = CalcTexColor();

    vec3 FinalColor = DiffuseColor * TexColor.rgb;

    FragColor = vec4(FinalColor, 1.0);
}