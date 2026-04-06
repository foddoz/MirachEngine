#version 330 core
layout(location=0) in vec2 aCorner; // (-0.5,-0.5) ~ (0.5,0.5)
layout(location=1) in vec2 aUV;     // (0,0) ~ (1,1)

uniform vec3 uCenterWS;   
uniform vec2 uSize;       
uniform mat4 uView;
uniform mat4 uProj;

out vec2 vUV;

void main() {
    vec3 right = vec3(uView[0][0], uView[1][0], uView[2][0]);
    vec3   up  = vec3(uView[0][1], uView[1][1], uView[2][1]);

    vec3 worldPos = uCenterWS
                  + right * (aCorner.x * uSize.x)
                  + up    * (aCorner.y * uSize.y);

    gl_Position = uProj * uView * vec4(worldPos, 1.0);
    vUV = aUV;
}
