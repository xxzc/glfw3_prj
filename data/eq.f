#version 330 core
out vec4 fcolor;

in vec3 texpos;

uniform sampler2D eqtex;

void main()
{
    vec3 dir = normalize(texpos);
    float pi = 3.1415926;
    vec2 pos = vec2(atan(dir.x, dir.z)/2/pi+0.5, acos(dir.y)/pi);
    fcolor = texture(eqtex, pos);
}
