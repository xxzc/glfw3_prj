#version 330 core
layout (location = 0) in vec3 cpos;

uniform mat4 view;
uniform mat4 proj;

out vec3 texpos;

void main()
{
    mat4 v = mat4(mat3(view));
    gl_Position = proj * v * vec4(cpos, 1.0);
    texpos = cpos;
}
