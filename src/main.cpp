#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <shader.hpp>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char* argv[])
{
    float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f, x3 = 0.0f, y3 = 0.0f;
    int drawType = 0;
    int drawNumberOfVertices = 0;
    int currentArgv = 2;
    float scaleX = 1.0f, scaleY = 1.0f;
    float rotate = 0.0f;
    float translateX = 0.0f, translateY = 0.0f;
    float scaleOX = 1.0f, scaleOY = 1.0f;
    glm::mat4 userTransform = glm::mat4(1.0f);
    if (argc <= 1)
    {
        std::cout << "Usage is:" << std::endl;
        std::cout << "cg [p (x1 y1) / l (x1 y1 x2 y2) / t (x1 y1 x2 y2 x3 y3)] [r (degree)] [t (x1 y1 x2 y2)] [s (x1 y1 x2 y2)] [m]" << std::endl;
        return 0;
    }
    else if (*argv[1] == 'p')
    {
        std::cout << "Show point" << std::endl;
        x1 = std::stof(std::string(argv[currentArgv++]));
        y1 = std::stof(std::string(argv[currentArgv++]));
        drawType = GL_POINTS;
        drawNumberOfVertices = 1;
    }
    else if (*argv[1] == 'l')
    {
        std::cout << "Show line" << std::endl;
        x1 = std::stof(std::string(argv[currentArgv++]));
        y1 = std::stof(std::string(argv[currentArgv++]));
        x2 = std::stof(std::string(argv[currentArgv++]));
        y2 = std::stof(std::string(argv[currentArgv++]));
        drawType = GL_LINES;
        drawNumberOfVertices = 2;
    }
    else if (*argv[1] == 't')
    {
        std::cout << "Show triangle" << std::endl;
        x1 = std::stof(std::string(argv[currentArgv++]));
        y1 = std::stof(std::string(argv[currentArgv++]));
        x2 = std::stof(std::string(argv[currentArgv++]));
        y2 = std::stof(std::string(argv[currentArgv++]));
        x3 = std::stof(std::string(argv[currentArgv++]));
        y3 = std::stof(std::string(argv[currentArgv++]));
        drawType = GL_TRIANGLES;
        drawNumberOfVertices = 3;
    }
    else
    {
        std::cout << "Wrong usage" << std::endl;
        return 0;
    }
    std::cout << "argc = " << argc << std::endl;
    std::cout << currentArgv << std::endl;
    while (currentArgv < argc)
    {
        if (*argv[currentArgv] == 't')
        {
            currentArgv++;
            translateX = std::stof(std::string(argv[currentArgv++]));
            translateY = std::stof(std::string(argv[currentArgv]));
            userTransform = glm::translate(userTransform, glm::vec3(translateX, translateY, 0.0f));
        }
        else if (*argv[currentArgv] == 's')
        {
            currentArgv++;
            scaleX = std::stof(std::string(argv[currentArgv++]));
            scaleY = std::stof(std::string(argv[currentArgv]));
            userTransform = glm::scale(userTransform, glm::vec3(scaleX, scaleY, 1.0f));
        }
        else if (*argv[currentArgv] == 'r')
        {
            currentArgv++;
            rotate = std::stof(std::string(argv[currentArgv]));
            userTransform = glm::rotate(userTransform, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        else if (*argv[currentArgv] == 'm')
        {
            userTransform = glm::scale(userTransform, glm::vec3(-1.0f, -1.0f, 1.0f));
        }
        else if (*argv[currentArgv] == 'o')
        {
            currentArgv++;
            scaleOX = std::stof(std::string(argv[currentArgv++]));
            scaleOY = std::stof(std::string(argv[currentArgv]));
        }
        currentArgv++;
    }

    float userInputPoints2d[] = {
         x1, y1,
         x2, y2,
         x3, y3
    };
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("./src/shader/shader.vert", "./src/shader/shader.frag");

    float points2d[] = {
        // Axis X
        -1.0f,  0.0f,
         1.0f,  0.0f,
         // Axis X points
        -0.9f,   0.05f,
        -0.9f,  -0.05f,
        -0.8f,   0.05f,
        -0.8f,  -0.05f,
        -0.7f,   0.05f,
        -0.7f,  -0.05f,
        -0.6f,   0.05f,
        -0.6f,  -0.05f,
        -0.5f,   0.05f,
        -0.5f,  -0.05f,
        -0.4f,   0.05f,
        -0.4f,  -0.05f,
        -0.3f,   0.05f,
        -0.3f,  -0.05f,
        -0.2f,   0.05f,
        -0.2f,  -0.05f,
        -0.1f,   0.05f,
        -0.1f,  -0.05f,
         0.9f,   0.05f,
         0.9f,  -0.05f,
         0.8f,   0.05f,
         0.8f,  -0.05f,
         0.7f,   0.05f,
         0.7f,  -0.05f,
         0.6f,   0.05f,
         0.6f,  -0.05f,
         0.5f,   0.05f,
         0.5f,  -0.05f,
         0.4f,   0.05f,
         0.4f,  -0.05f,
         0.3f,   0.05f,
         0.3f,  -0.05f,
         0.2f,   0.05f,
         0.2f,  -0.05f,
         0.1f,   0.05f,
         0.1f,  -0.05f,
         // Axis Y
         0.0f, -1.0f,
         0.0f,  1.0f,
         // Axis Y points
         0.05f,  -0.9f,
        -0.05f,  -0.9f,
         0.05f,  -0.8f,
        -0.05f,  -0.8f,
         0.05f,  -0.7f,
        -0.05f,  -0.7f,
         0.05f,  -0.6f,
        -0.05f,  -0.6f,
         0.05f,  -0.5f,
        -0.05f,  -0.5f,
         0.05f,  -0.4f,
        -0.05f,  -0.4f,
         0.05f,  -0.3f,
        -0.05f,  -0.3f,
         0.05f,  -0.2f,
        -0.05f,  -0.2f,
         0.05f,  -0.1f,
        -0.05f,  -0.1f,
         0.05f,   0.9f,
        -0.05f,   0.9f,
         0.05f,   0.8f,
        -0.05f,   0.8f,
         0.05f,   0.7f,
        -0.05f,   0.7f,
         0.05f,   0.6f,
        -0.05f,   0.6f,
         0.05f,   0.5f,
        -0.05f,   0.5f,
         0.05f,   0.4f,
        -0.05f,   0.4f,
         0.05f,   0.3f,
        -0.05f,   0.3f,
         0.05f,   0.2f,
        -0.05f,   0.2f,
         0.05f,   0.1f,
        -0.05f,   0.1f,
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points2d), points2d, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBOui, VAOui;
    glGenVertexArrays(1, &VAOui);
    glGenBuffers(1, &VBOui);

    glBindVertexArray(VAOui);

    glBindBuffer(GL_ARRAY_BUFFER, VBOui);
    glBufferData(GL_ARRAY_BUFFER, sizeof(userInputPoints2d), userInputPoints2d, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    ourShader.use();
    ourShader.setVec3("ourColor", glm::vec3(0.0f, 0.0f, 0.0f));
    glPointSize(5.0f);

while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();
        ourShader.setVec3("ourColor", glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        glm::mat4 transform     = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view       = glm::scale(view, glm::vec3(scaleOX, scaleOY, 1.0f));
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("transform", transform);

        glBindVertexArray(VAO);
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);
        glDrawArrays(GL_LINES, 0, 76);

        glBindVertexArray(VAOui);
        ourShader.setVec3("ourColor", glm::vec3(0.0f, 0.7f, 0.7f));
        ourShader.setMat4("transform", userTransform);
        glDrawArrays(drawType, 0, drawNumberOfVertices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}