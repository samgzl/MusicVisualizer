#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 MV;
out vec3 fragNor;
out vec2 fragTex;
out vec3 pos;
uniform sampler2D tex;
out vec3 vColor;
uniform float offset;

void main() {
    

    
    gl_Position = vec4(vertTex, 0.0, 1.0);
    fragTex = (vertTex + 1.0) / 2.0;
    fragTex.x += sin(fragTex.y * 4*2*3.14159 + offset) / 100;

}
