#ifndef __uilt_h_
#define __uilt_h_

#include <glad/glad.h>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <assimp/scene.h>
#include <vector>
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
    static Camera* inst;
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
    static Camera& getCamera(){return *inst;}
};


struct Vertex
{
    glm::vec3 pos;
    glm::vec3 norm;
};

class Mesh
{
public:
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> v;
    std::vector<GLuint> i;
    Mesh(std::vector<Vertex>& pv, std::vector<GLuint>& pi):v(pv),i(pi){}
    void genBuf(){}
    void drawInit(){
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*v.size(), v.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, norm));
                glEnableVertexAttribArray(1);
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*i.size(), i.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
    }
    void draw(){
        glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

};
class Model
{
public:
    std::vector<Mesh> m;
    const aiScene* scene;
    void loadMesh(aiMesh* mesh);
public:
    Model(const char* path);
    void drawInit();
    void draw();
};

class TextureCubeMap
{
    GLuint texid;
    GLuint VAO, VBO;
    float boxv[36*3] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
public:
    TextureCubeMap(const char* paths[6]);
    GLuint getID(){return texid;}
    void draw(GLuint unit);
};

class EQMap
{
    GLuint VAO, VBO;
    Shader eqshader;
    float boxv[36*3] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
public:
    Texture tex;
    EQMap(const char* path);
    void draw(const glm::mat4& view, const glm::mat4& proj, GLuint unit=0);
};
#endif
