#version 330 core 
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;

uniform float time;
uniform sampler2D tex;


void main() {
    
    color = vec4(137.0/255, 189./255, 255.0/255, 1)*pos.z;

}
