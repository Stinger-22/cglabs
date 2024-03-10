#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>

#include <shader.hpp>
#include <camera.hpp>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void drawPixel(Shader& shader, int x, int y, glm::mat4& transform);
void drawCircle(Shader& shader, int x, int y, int x1, int y1, glm::mat4& transform);

// ----------------------
// SETTINGS

// User screen
const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 900;

// Time
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Camera
const float radius = 10.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;

int main(int argc, char* argv[])
{
    std::cout << "argc = " << argc << std::endl;
    float x1 = 0.0f, y1 = 0.0f, z1 = 0.0f;
    float x2 = 0.0f, y2 = 0.0f, z2 = 0.0f;
    float x3 = 0.0f, y3 = 0.0f, z3 = 0.0f;
    float circleRadius = 0.0f;
    int drawType = 0;
    int drawNumberOfVertices = 0;
    int currentArgv = 2;
    float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
    float rotate = 0.0f;
    float translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;
    float scaleOX = 1.0f, scaleOY = 1.0f, scaleOZ = 1.0f;
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
    else if (*argv[1] == 'c')
    {
        std::cout << "Show circle" << std::endl;
        x1 = std::stof(std::string(argv[currentArgv++]));
        y1 = std::stof(std::string(argv[currentArgv++]));
        circleRadius = std::stof(std::string(argv[currentArgv++]));
    }
    else
    {
        std::cout << "Wrong usage" << std::endl;
        return 0;
    }
    std::cout << currentArgv << std::endl;
    while (currentArgv < argc)
    {
        if (*argv[currentArgv] == 't')
        {
            currentArgv++;
            translateX = std::stof(std::string(argv[currentArgv++]));
            translateY = std::stof(std::string(argv[currentArgv]));
            userTransform = glm::translate(userTransform, glm::vec3(translateX, translateY, translateZ));
        }
        else if (*argv[currentArgv] == 's')
        {
            currentArgv++;
            scaleX = std::stof(std::string(argv[currentArgv++]));
            scaleY = std::stof(std::string(argv[currentArgv]));
            userTransform = glm::scale(userTransform, glm::vec3(scaleX, scaleY, scaleZ));
        }
        else if (*argv[currentArgv] == 'r')
        {
            currentArgv++;
            char* rotateAxis = argv[currentArgv++];
            float rotateX = 0.0f, rotateY = 0.0f, rotateZ = 0.0f;
            for (size_t i = 0; i < strlen(rotateAxis); i++)
            {
                if (rotateAxis[i] == 'x')
                {
                    rotateX = 1;
                }
                else if (rotateAxis[i] == 'y')
                {
                    rotateY = 1;
                }
                else
                {
                    std::cout << "Wrong usage" << std::endl;
                    return 0;
                }
            }
            rotate = std::stof(std::string(argv[currentArgv]));
            userTransform = glm::rotate(userTransform, glm::radians(rotate), glm::vec3(rotateX, rotateY, rotateZ));
        }
        else if (*argv[currentArgv] == 'm')
        {
            currentArgv++;
            char* mirrorAxis = argv[currentArgv];
            float mirrorX = 1.0f, mirrorY = 1.0f, mirrorZ = 1.0f;
            if (mirrorAxis[0] == 'x')
            {
                mirrorX = -1.0f;
            }
            else if (mirrorAxis[0] == 'y')
            {
                mirrorY = -1.0f;
            }
            else
            {
                std::cout << "Wrong usage" << std::endl;
                return 0;
            }
            userTransform = glm::scale(userTransform, glm::vec3(mirrorX, mirrorY, mirrorZ));
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
         x1, y1, 0.0f,
         x2, y2, 0.0f,
         x3, y3, 0.0f
    };

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CG", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("./src/shader/shader.vert", "./src/shader/shader.frag");

    float points2d[] = {
        -20.5f,  0.0f,  0.0f,
         20.5f,  0.0f,  0.0f,
         0.0f, -20.5f,  0.0f,
         0.0f,  20.5f,  0.0f,
    };

    float squareVertices[] = {
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };
    unsigned int squareIndices[] = {
        0, 1, 3,
        1, 2, 3
    };

    // Plot Object
    unsigned int axisVBO, axisVAO;
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);

    glBindVertexArray(axisVAO);

    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points2d), points2d, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // User input object
    unsigned int VBOui, VAOui;
    glGenVertexArrays(1, &VAOui);
    glGenBuffers(1, &VBOui);

    glBindVertexArray(VAOui);

    glBindBuffer(GL_ARRAY_BUFFER, VBOui);
    glBufferData(GL_ARRAY_BUFFER, sizeof(userInputPoints2d), userInputPoints2d, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Square object
    unsigned int squareVBO, squareVAO, squareEBO;
    glGenVertexArrays(1, &squareVAO);
    glGenBuffers(1, &squareVBO);
    glGenBuffers(1, &squareEBO);

    glBindVertexArray(squareVAO);

    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window))
    {
        // ----------------------
        // Calculate time per frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // ----------------------
        // Input
        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        ourShader.setVec3("ourColor", glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 view          = camera.getViewMatrix();
        glm::mat4 projection    = glm::perspective(glm::radians(camera.Zoom), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 transform     = glm::mat4(1.0f);
        view       = glm::scale(view, glm::vec3(scaleOX, scaleOY, scaleOZ));
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("transform", transform);


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
        ourShader.setMat4("model", model);
        // ----------------------
        // Draw net
        glBindVertexArray(axisVAO);
        for (float y = -20.5f; y <= 20.5f; y += 1.0f)
        {
            ourShader.setMat4("transform", glm::translate(transform, glm::vec3(0.0f, y, 0.0f)));
            glDrawArrays(GL_LINES, 0, 2);
        }
        for (float x = -20.5f; x <= 20.5f; x += 1.0f)
        {
            ourShader.setMat4("transform", glm::translate(transform, glm::vec3(x, 0.0f, 0.0f)));
            glDrawArrays(GL_LINES, 2, 2);
        }

        // ----------------------
        // Draw figures
        if (*argv[1] == 'l')
        {
            float step;
            float dx = x2 - x1;
            float dy = y2 - y1;
            if (fabs(dx) >= fabs(dy))
            {
                step = fabs(dx);
            }
            else
            {
                step = fabs(dy);
            }
            dx = dx / step;
            dy = dy / step;
            float x = x1, y = y1;
            ourShader.setVec3("ourColor", glm::vec3(1.0f, 0.9215, 0.2196));
            glBindVertexArray(squareVAO);
            for (int i = 0; i < step; i++)
            {
                drawPixel(ourShader, x, y, transform);
                x += dx;
                y += dy;
            }
        }
        else if (*argv[1] == 'c')
        {
            ourShader.setVec3("ourColor", glm::vec3(0.5529f, 0.1647f, 0.7804f));
            glBindVertexArray(squareVAO);
            int x = 0, y = circleRadius;
            int d = 3 - 2 * circleRadius;
            drawCircle(ourShader, x, y, x1, y1, transform);
            while (y >= x)
            {
                x++;
                if (d > 0)
                {
                    y--;
                    d = d + 4 * (x - y) + 10;
                }
                else
                {
                    d = d + 4 * x + 6;
                }
                drawCircle(ourShader, x, y, x1, y1, transform);
                drawCircle(ourShader, x, y, x1, y1, transform);
                drawCircle(ourShader, x, y, x1, y1, transform);
                drawCircle(ourShader, x, y, x1, y1, transform);
                drawCircle(ourShader, y, x, x1, y1, transform);
                drawCircle(ourShader, y, x, x1, y1, transform);
                drawCircle(ourShader, y, x, x1, y1, transform);
                drawCircle(ourShader, y, x, x1, y1, transform);
            }
        }
        else if (*argv[1] == 'p')
        {
            glBindVertexArray(squareVAO);
            ourShader.setMat4("transform", glm::translate(transform, glm::vec3(x1, y1, 0.0f)));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else
        {
            ourShader.setVec3("ourColor", glm::vec3(0.5529f, 0.1647f, 0.7804f));
            ourShader.setMat4("transform", userTransform);
            glDrawArrays(drawType, 0, drawNumberOfVertices);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &axisVAO);
    glDeleteBuffers(1, &axisVBO);
    glDeleteVertexArrays(1, &squareVAO);
    glDeleteBuffers(1, &squareVBO);
    glDeleteBuffers(1, &squareEBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    // ----------------------
    // Exit application
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // ----------------------
    // Move camera by keyboard
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.processKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.processKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.processKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.processKeyboard(RIGHT, deltaTime);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    // ----------------------
    // Move camera by mouse
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.processMouseScroll((float) yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void drawPixel(Shader& shader, int x, int y, glm::mat4& transform)
{
    shader.setMat4("transform", glm::translate(transform, glm::vec3(x, y, 0.0f)));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawCircle(Shader& shader, int x, int y, int x1, int y1, glm::mat4& transform)
{
    drawPixel(shader, x1 + x, y1 + y, transform);
    drawPixel(shader, x1 - x, y1 + y, transform);
    drawPixel(shader, x1 + x, y1 - y, transform);
    drawPixel(shader, x1 - x, y1 - y, transform);
    drawPixel(shader, x1 + y, y1 + x, transform);
    drawPixel(shader, x1 - y, y1 + x, transform);
    drawPixel(shader, x1 + y, y1 - x, transform);
    drawPixel(shader, x1 - y, y1 - x, transform);
}