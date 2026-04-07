#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUV;
uniform vec4 uRect;
uniform vec2 uScreen;
out vec2 vUV;
void main(){
    vec2 px  = vec2(uRect.x + aPos.x * uRect.z,
                    uRect.y + aPos.y * uRect.w);
    vec2 ndc = (px / uScreen) * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
    vUV = aUV;
}

