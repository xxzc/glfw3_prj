#version 330 core
out vec4 fcolor;

in vec3 texpos;

uniform samplerCube box;
void main()
{
    fcolor = texture(box, texpos);
}
