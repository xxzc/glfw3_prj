#include <iostream>
#include <uilt.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <cstddef>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <stb_image.h>
using namespace std;

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 norm;
};

class Mesh
{
public:
    GLuint VAO, VBO, EBO;
    vector<Vertex> v;
    vector<GLuint> i;
    Mesh(vector<Vertex>& pv, vector<GLuint>& pi):v(pv),i(pi){}
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
    vector<Mesh> m;
    const aiScene* scene;
    Model(const char* path){
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
    void drawInit(){
        for(uint p = 0; p<m.size(); p++)
            m[p].drawInit();
    }
    void draw(){
        for(uint p = 0; p<m.size(); p++)
            m[p].draw();
    }
    void loadMesh(aiMesh* mesh){
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
    TextureCubeMap(const char* paths[6]){
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
    GLuint getID(){return texid;}
    void draw(GLuint unit){
        glDepthMask(GL_FALSE);
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texid);
        glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
    }
};
void processInput(GLFWwindow* window, Camera& camera);
int main()
{
    AppGL app("AppGL APP", 800, 600);
    GLFWwindow* window = app.getWindow();

    Texture texture1("data/wall.jpg");

    Shader shader("data/shader.v", "data/shader.f");
    Shader cubeShader("data/cube.v", "data/cube.f");

    const char* skyboxpic[6] = {
        "data/skybox/right.jpg", "data/skybox/left.jpg",
        "data/skybox/top.jpg", "data/skybox/bottom.jpg",
        "data/skybox/back.jpg", "data/skybox/front.jpg"
    };
    TextureCubeMap cube(skyboxpic);
    shader.useProgram();
    shader.setUniform1i("texture1", 0);
    shader.setUniform1i("box", 0);

    cubeShader.useProgram();
    cubeShader.setUniform1i("box", 0);
    if(shader.isFailed()) return -1;
    Model m("data/model.dae");
    m.drawInit();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int f=0;
    Camera camera(0.0f, 0.0f, -5.0f);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window, camera);

        glm::mat4 model, view, proj;
        model = glm::rotate(model, (float)glfwGetTime()*0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        view = camera.trans;
        int w,h;
        glfwGetWindowSize(app.getWindow(), &w, &h);
        proj = glm::perspective(glm::radians(45.0f), float(w)/float(h), 0.1f, 100.0f);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeShader.useProgram();
        cubeShader.setUniformMatrix4fv("view", glm::value_ptr(view));
        cubeShader.setUniformMatrix4fv("proj", glm::value_ptr(proj));
        cube.draw(0);

        shader.useProgram();
        texture1.enableOn(0);
        shader.setUniform3fv("cam", glm::value_ptr(camera.pos));
        GLfloat light[] = {-2.0f, 2.0f, -1.0f};
        shader.setUniform3fv("light", light);
        shader.setUniformMatrix4fv("model", glm::value_ptr(model));
        shader.setUniformMatrix4fv("view", glm::value_ptr(view));
        shader.setUniformMatrix4fv("proj", glm::value_ptr(proj));

        m.draw();

        glfwSwapBuffers(window);
        f++;
        glfwPollEvents();
    }

}
void processInput(GLFWwindow* window, Camera& camera)
{
    float ds = 0.1f, dr = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.walk(ds, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.walk(-ds, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.walk(0.0f, ds);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.walk(0.0f, -ds);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.rotate(0.0f, dr);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.rotate(0.0f, -dr);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.rotate(dr, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.rotate(-dr, 0.0f);

}
