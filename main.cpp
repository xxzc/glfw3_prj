#include <iostream>
#include <uilt.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <cstddef>

using namespace std;

void fpsstat(double ttime){
    static double mark = -1.0;
    static int nframe;
    nframe++;
    if(mark < 0) mark = ttime;
    if(ttime-mark > 1.0){
        double fps = double(nframe)/(ttime-mark);
        double ftime = 1000.0/fps;
        nframe = 0;
        mark = ttime;
        cout << "Fps: "<<fps<<" avg "<<ftime<<"ms"<<endl;
    }
}

void processInput(GLFWwindow* window, Camera& camera);
void mouseCallback(GLFWwindow* window, double x, double y);

int main()
{
    AppGL app("AppGL APP", 1700, 1000);
    GLFWwindow* window = app.getWindow();
    Camera camera(0.0f, 0.0f, -3.5f);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader shader("data/shader.v", "data/shader.f");
    shader.setUniform1i("eqtex", 0);

    EQMap eqmap("data/eq.jpg");
    shader.useProgram();

    if(shader.isFailed()) return -1;
    Model m("data/ball.dae");
    m.drawInit();

    int f=0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    double ptime, ttime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        ptime = ttime;
        ttime = glfwGetTime();
        fpsstat(ttime);
        processInput(window, camera);

        glm::mat4 model, view, proj;
        model = glm::rotate(model, (float)ttime*0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        view = camera.trans;
        int w,h;
        glfwGetWindowSize(app.getWindow(), &w, &h);
        proj = glm::perspective(glm::radians(45.0f), float(w)/float(h), 0.1f, 100.0f);

        //glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        eqmap.draw(view, proj);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        shader.useProgram();
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
void mouseCallback(GLFWwindow* window, double x, double y)
{
    (void*) window;
    static double ox, oy;
    static bool inited = false;
    if(!inited){
        ox = x;
        oy = y;
        inited = true;
    }
    float xscale = -0.005,yscale = -0.005;
    Camera::getCamera().rotate(xscale*float(x-ox), yscale*float(y-oy));
    ox = x;
    oy = y;
}
void processInput(GLFWwindow* window, Camera& camera)
{
    float ds = 0.1f, dr = 0.03f;
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
