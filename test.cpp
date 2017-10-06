#define debug
#include <iostream>
#include <uilt.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <cstddef>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
    void draw(Shader& shader){
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

int main()
{
    Model model("data/model.obj");

}
