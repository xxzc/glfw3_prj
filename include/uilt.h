#ifndef __uilt_h_
#define __uilt_h_

#include <glad/glad.h>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifdef debug
#define DOUT(x) do { std::cout << x << std::endl; } while( false )
#define DOUTN(x) do { std::cout << x; } while( false )
#else
#define DOUT(x) do { } while( false )
#define DOUTN(x) do { } while( false )
#endif

void filetoString(const char* fname, std::string& str);
class Texture;
class Shader
{
    GLuint program;
    bool pFailed;
public:
    Shader(const char* vShaderPath, const char* fShaderPath);
    void useProgram();
    bool isFailed();
    void setUniform1i(const char* name, GLint v);
    void setUniform3fv(const char* name, const GLfloat *value);
    void setUniformMatrix4fv(const char* name, const GLfloat *value);
};

class AppGL
{
    GLFWwindow* window;
    void (*resizeCallBack)(GLFWwindow* w, int width, int height);
    void (*loopCallBack)(GLFWwindow* w);
public:
    AppGL(const char* title, int width, int height);
    ~AppGL();
    GLFWwindow* getWindow(){return window;}
    void loop();
    void setLoopCallback(void (*func)(GLFWwindow* w));
    void setResizeCallback(void (*func)(GLFWwindow* w, int width, int height));
    static void defaultResizeCallback(GLFWwindow* w, int width, int height);
};

class Texture
{
    GLuint texid;
    int width, height, nChannels;
public:
    Texture(const char* fname);
    GLuint getID(){return texid;}
    void enableOn(unsigned int unit);
};

class Camera
{

    void updateTrans();
public:
    glm::vec3 pos;
    float yaw, pitch;//lr,ud
    glm::vec3 front, left, up;
    glm::mat4 trans;
    Camera();
    Camera(float x, float y, float z);
    void walk(float dx, float dy);
    void rotate(float dyaw, float dpitch);
    void go(float x, float y, float z);
    void lookat(float x, float y, float z);
};

#endif
