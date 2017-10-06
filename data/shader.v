#version 330 core
layout (location = 0) in vec3 mpos;
layout (location = 1) in vec3 mnormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 wnormal;
out vec3 wpos;

void main()
{
    gl_Position = proj * view * model * vec4(mpos, 1.0);
    wpos = vec3(model * vec4(mpos, 1.0));
    wnormal = mat3(transpose(inverse(model))) * mnormal;
}
