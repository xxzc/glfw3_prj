#include <uilt.h>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
using namespace std;

void filetoString(const char* fname, string& str){
    ifstream files;
    files.open(fname);
    stringstream sstr;
    sstr << files.rdbuf();
    str = sstr.str();
}

Shader::Shader(const char* vShaderPath, const char* fShaderPath)
{
    string vcode, fcode;
    char compileError[1000];
    GLint compileStatus;
    const char* code;
    pFailed = false;
    filetoString(vShaderPath, vcode);
    filetoString(fShaderPath, fcode);
    GLuint vShader, fShader;
    vShader = glCreateShader(GL_VERTEX_SHADER);
    code = vcode.c_str();
    glShaderSource(vShader, 1, &code, NULL);
    glCompileShader(vShader);
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus != GL_TRUE){
        glGetShaderInfoLog(vShader, sizeof(compileError), NULL, compileError);
        cerr << "vShader Error:" << endl;
        cerr << compileError;
        pFailed = true;
        return;
    }
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    code = fcode.c_str();
    glShaderSource(fShader, 1, &code, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus != GL_TRUE){
        glGetShaderInfoLog(fShader, sizeof(compileError), NULL, compileError);
        cerr << "fShader Error:" << endl;
        cerr << compileError;
        pFailed = true;
        return;
    }
    program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus != GL_TRUE){
        glGetProgramInfoLog(program, sizeof(compileError), NULL, compileError);
        cerr << "Program Error:" << endl;
        cerr << compileError;
        pFailed = true;
        return;
    }
    glDeleteShader(vShader);
    glDeleteShader(fShader);
}

void Shader::useProgram()
{
    glUseProgram(program);
}

bool Shader::isFailed()
{
    return pFailed;
}

void Shader::setUniform1i(const char* name, GLint v)
{
    GLuint loc = glGetUniformLocation(program, name);
    glUniform1i(loc, v);
}

void Shader::setUniform3fv(const char *name, const GLfloat *value)
{
    GLuint loc = glGetUniformLocation(program, name);
    glUniform3fv(loc, 1, value);
}

void Shader::setUniformMatrix4fv(const char *name, const GLfloat *value)
{
    GLuint loc = glGetUniformLocation(program, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, value);
}

AppGL::AppGL(const char* title, int width, int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }
    setResizeCallback(defaultResizeCallback);

}

AppGL::~AppGL()
{
    glfwTerminate();
}


void AppGL::loop()
{
    while(!glfwWindowShouldClose(window)){
        if(loopCallBack) loopCallBack(window);
        glfwPollEvents();
    }
}

void AppGL::defaultResizeCallback(GLFWwindow* w, int width, int height)
{
    (void)w;
    glViewport(0, 0, width, height);
}

void AppGL::setLoopCallback(void (*func)(GLFWwindow* w))
{
    loopCallBack = func;
}

void AppGL::setResizeCallback(void (*func)(GLFWwindow* w, int width, int height)){
    resizeCallBack = func;
    glfwSetFramebufferSizeCallback(window, func);
}


Texture::Texture(const char* fname)
{
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load(fname, &width, &height, &nChannels, 0);
    if(!image){
        cerr << "Err loading image";
        return;
    }
    GLenum fmt = (nChannels==3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
}


void Texture::enableOn(unsigned int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texid);
}

Camera* Camera::inst = NULL;

Camera::Camera()
{
    inst = this;
    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    yaw = 0.0f, pitch = 0.0f;
    front = glm::vec3(0.0f, 0.0f, 1.0f);
    left = glm::vec3(1.0f, 0.0f, 0.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    updateTrans();
}

Camera::Camera(float x, float y, float z):Camera()
{
    pos = glm::vec3(x, y, z);
    updateTrans();
}

void Camera::walk(float dx, float dy){
    pos += dx * front + dy * left;
    updateTrans();
}

void Camera::rotate(float dyaw, float dpitch){
    yaw += dyaw;
    pitch += dpitch;
    float halfpi = glm::pi<float>()/2.0f;
    if(pitch > halfpi) pitch = halfpi;
    if(pitch < -halfpi) pitch = -halfpi;
    glm::vec3 f;
    f.x = cos(pitch) * sin(yaw);
    f.y = sin(pitch);
    f.z = cos(pitch) * cos(yaw);
    front = glm::normalize(f);
    left = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), front));
    up = glm::normalize(glm::cross(front, left));
    updateTrans();
}

void Camera::go(float x, float y, float z)
{
    pos = glm::vec3(x, y, z);
}

void Camera::lookat(float x, float y, float z)
{
    front = glm::normalize(glm::vec3(x, y, z) - pos);
    left = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), front));
    up = glm::normalize(glm::cross(front, left));
    updateTrans();
}

void ovec3(glm::vec3 v){
    cout <<v.x<<" "<<v.y<<" "<<v.z<<endl;
}

void Camera::updateTrans()
{
    trans = glm::lookAt(pos, pos+front, up);
}

Model::Model(const char *path)
{
    Assimp::Importer imp;
    scene = imp.ReadFile(path, aiProcess_Triangulate);
    if(!scene || !scene->mRootNode){
        cerr << "Model Import Fail: " << imp.GetErrorString() << endl;
        return;
    }
    for(unsigned int i = 0; i<scene->mNumMeshes;i++){
        DOUT("mesh#"<<i);
        loadMesh(scene->mMeshes[i]);
    }
}

void Model::drawInit()
{
    for(uint p = 0; p<m.size(); p++)
        m[p].drawInit();
}

void Model::draw()
{
    for(uint p = 0; p<m.size(); p++)
        m[p].draw();
}

void Model::loadMesh(aiMesh* mesh)
{
    vector<Vertex> v(mesh->mNumVertices);
    vector<GLuint> i;
    for(unsigned int p = 0; p<mesh->mNumVertices;p++){
        Vertex vertex;
        vertex.pos = glm::vec3(mesh->mVertices[p].x, mesh->mVertices[p].y, mesh->mVertices[p].z);
        vertex.norm = glm::vec3(mesh->mNormals[p].x, mesh->mNormals[p].y, mesh->mNormals[p].z);
        v[p] = vertex;
        DOUT("\tV#"<<p<<" "<<glm::to_string(vertex.pos)<<" "<<glm::to_string(vertex.norm));
    }
    for(unsigned int p = 0; p<mesh->mNumFaces; p++){
        aiFace& face = mesh->mFaces[p];
        DOUTN("\tFace#"<<p<<":"<<face.mNumIndices<<"\n\t\t");
        assert(face.mNumIndices == 3);
        for(unsigned int q = 0; q<face.mNumIndices; q++){
            i.push_back(face.mIndices[q]);
            DOUTN(face.mIndices[q]<<" ");
        }
        DOUT("");
    }
    m.push_back(Mesh(v, i));
}

TextureCubeMap::TextureCubeMap(const char* paths[6])
{
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texid);
    for(int i=0;i<6;i++){
        int w, h, nch;
        stbi_set_flip_vertically_on_load(0);
        unsigned char* data = stbi_load(paths[i], &w, &h, &nch, 0);
        if(!data){
            cerr << "Error loading "<<paths[i]<<endl;
            return;
        }
        GLenum fmt = nch==3?GL_RGB:GL_RGBA;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, fmt,
                     w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxv), boxv, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void TextureCubeMap::draw(GLuint unit)
{
    glDepthMask(GL_FALSE);
    glActiveTexture(GL_TEXTURE0+unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texid);
    glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}
