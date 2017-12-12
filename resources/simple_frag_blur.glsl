#version 330 core 
in vec3 fragNor;
in vec3 vColor;
in vec3 pos;
in vec2 fragTex;

out vec4 color;

uniform sampler2D tex;
uniform float resolution; 
uniform vec2 dir;

void main()
{
    
    vec4 sum = vec4(0, 0, 0, 1);
    vec2 tc = fragTex;
    float radius = 10.0;
    float blur = radius/resolution;
    float hstep = dir.x;
    float vstep = dir.y;
    
    //apply blurring, using a 9-tap filter with predefined gaussian weights
    /*
    sum += texture(tex, vec2(tc.x - 4.0*blur*hstep, tc.y - 4.0*blur*vstep)) * 0.0162162162;
    sum += texture(tex, vec2(tc.x - 3.0*blur*hstep, tc.y - 3.0*blur*vstep)) * 0.0540540541;
    sum += texture(tex, vec2(tc.x - 2.0*blur*hstep, tc.y - 2.0*blur*vstep)) * 0.1216216216;
    sum += texture(tex, vec2(tc.x - 1.0*blur*hstep, tc.y - 1.0*blur*vstep)) * 0.1945945946;

    sum += texture(tex, vec2(tc.x, tc.y)) * 0.2270270270;

    sum += texture(tex, vec2(tc.x + 1.0*blur*hstep, tc.y + 1.0*blur*vstep)) * 0.1945945946;
    sum += texture(tex, vec2(tc.x + 2.0*blur*hstep, tc.y + 2.0*blur*vstep)) * 0.1216216216;
    sum += texture(tex, vec2(tc.x + 3.0*blur*hstep, tc.y + 3.0*blur*vstep)) * 0.0540540541;
    sum += texture(tex, vec2(tc.x + 4.0*blur*hstep, tc.y + 4.0*blur*vstep)) * 0.0162162162;

    vec4 vertColor = vec4(vColor, 1.0);
    color = vertColor * sum;
 */
    vec4 colorlowres = texture(tex, fragTex, 10);
    color = texture(tex, fragTex) + colorlowres;
    
    
    //color = texture(tex, fragTex);
   // color = vec4(1, 0, 0, 1);
}
     
