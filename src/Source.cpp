#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "../header/stb_image.h"
#endif
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <fstream>
#include "../header/shader_m.h"
#include "../header/camera.h"
#include "../header/model.h"
#include "../header/fogGrid.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void genSphereVolume();


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    Shader fogShader("./src/fog.vs", "./src/fog.fs");

    // build and compile shaders
    // -------------------------
    unsigned int uniformBlockIndexFog = glGetUniformBlockIndex(fogShader.ID, "Matrices");

    glUniformBlockBinding(fogShader.ID, uniformBlockIndexFog, 0);
   
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    // 2 Mat4
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    unsigned int wallText = Util::loadTexture("./res/textures/wall.jpg",true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, wallText);
    fogShader.setInt("texture1", wallText);
    genSphereVolume();
    FogGrid fogBox = FogGrid("./res/fog/sphere.txt");
    fogBox.assignUniform(fogShader);
    // render loop
    // -----------
    unsigned int cubeVAO = 0, cubeVBO = 0;
    
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
         // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));

        glm::mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        fogShader.use();
        fogShader.setVec3("viewPos", camera.Position);
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0,1,0));
        model = glm::scale(model, glm::vec3(20, 20, 20));
        fogShader.setMat4("model", model);
        Util::renderCube(cubeVAO, cubeVBO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

   
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


void genSphereVolume() {
    float radius = 1.f;
    glm::vec3 min(-radius),max(radius),num(6,6,6),sphereCenter = (min+max)/2.0f;
    glm::vec3 s(0.5f), a(0.9f);
    glm::vec3 step = (max - min) / num,step2 = step/2.0f;
    float distance = 0;
    vector<float> density(num.x*num.y*num.z);
    bool isIntersect = true;
    for (int i = 0; i < num.x; i++) {
        float offsetX = i * step.x;
        for (int j = 0; j < num.y; j++) {
            float offsetY = j * step.y;
            for (int k = 0; k < num.z; k++) {
                float offsetZ = k * step.z;
                glm::vec3 center = min + glm::vec3(step2.x + offsetX, step2.y + offsetY, step.z + offsetZ);
                if (glm::length(center) <= radius) {
                    density[i + num.x * j + (num.x * num.y) * k] = 1.0f;
                }
            }
        }
    }
    ofstream myfile("./res/fog/sphere.txt");
    if (myfile.is_open()){
        myfile << "min " + to_string(min.x) + " "  +to_string(min.y) + " " + to_string(min.z)<< "\n";
        myfile << "max " + to_string(max.x) + " "  +to_string(max.y) + " " + to_string(max.z) << "\n";
        myfile << "num " + to_string(num.x) + " "  +to_string(num.y) + " " + to_string(num.z) << "\n";

        myfile << "s " + to_string(s.x) + " " + to_string(s.y) + " " + to_string(s.z) << "\n";
        myfile << "a " + to_string(a.x) + " " + to_string(a.y) + " " + to_string(a.z) << "\n";
        for (unsigned int i = 0; i < density.size(); i++) {
            myfile << "d " + to_string(density[i]) << "\n";
        }
        myfile.close();
    }
    else cout << "Unable to open file";


}