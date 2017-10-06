#version 330 core
out vec4 fcolor;

in vec3 wnormal;
in vec3 wpos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform samplerCube box;
uniform vec3 cam;
uniform vec3 light;

void main()
{
    vec4 lightc = vec4(1,1,1,1);
    vec4 color = vec4(0.5,0.5,0.5,1);
    vec4 ambient = 0.2 * color * lightc;

    vec3 lightv = normalize(light - wpos);
    vec3 norm = normalize(wnormal);
    vec4 diffuse = max(dot(lightv, norm), 0.0) * color * lightc;

    vec3 camv = normalize(cam - wpos);
    vec3 reflectv = reflect(-lightv, norm);
    vec4 specular = pow(max(dot(camv, reflectv), 0.0), 10) * 1 * color * lightc;

    vec4 skyreflect = texture(box, reflect(-camv, norm));
    
    float frate=1.52;
    vec4 skyrefract = texture(box, refract(-camv, norm, 1/frate));

    fcolor = specular + 0.2*skyreflect + 1*skyrefract;
}
