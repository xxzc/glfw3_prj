#version 330 core
out vec4 fcolor;

in vec3 wnormal;
in vec3 wpos;

uniform sampler2D eqtex;
uniform vec3 cam;
uniform vec3 light;

vec4 eqtexure(sampler2D eqtex, vec3 dir)
{
    float pi = 3.1415926;
    return texture(eqtex, vec2(atan(dir.x, dir.z)/2/pi+0.5, acos(dir.y)/pi));
}

void main()
{
    vec4 lightc = vec4(1,1,1,1);
    vec4 acolor = vec4(0.24725, 0.1995, 0.0745, 1);
    vec4 dcolor = vec4(0.75164, 0.60648, 0.22648, 1);
    vec4 scolor = vec4(0.628281, 0.555802, 0.366065, 1);
    vec4 ambient = acolor * lightc;

    vec3 lightv = normalize(light - wpos);
    vec3 norm = normalize(wnormal);
    vec4 diffuse = max(dot(lightv, norm), 0.0) * dcolor * lightc;

    vec3 camv = normalize(cam - wpos);
    vec3 reflectv = reflect(-lightv, norm);
    vec4 specular = scolor * pow(max(dot(camv, reflectv), 0.0), 25.6) * 1 * scolor * lightc;

    vec4 skyreflect = eqtexure(eqtex, reflect(-camv, norm));
    
    float frate=1.52;
    vec4 skyrefract = eqtexure(eqtex, refract(-camv, norm, 1/frate));

    fcolor = ambient + 0.3*diffuse + specular + 0.5*skyreflect + 0.0*skyrefract;
}

