#include <iostream>
#include <uilt.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <cstddef>

using namespace std;

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
        GLfloat light[] = {-2.0f, 2.0f, 3.0f};
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
