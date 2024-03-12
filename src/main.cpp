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


struct Color
{
    float red;
    float green;
    float blue;
    float alpha;

    Color()
    {
        red = 0.0f;
        green = 0.0f;
        blue = 0.0f;
        alpha = 0.0f;
    }

    Color(float red, float green, float blue)
    {
        this->red = red;
        this->green = green;
        this->blue = blue;
    }

    void print()
    {
        std::cout << "Pixel(" << red << ", " << green << ", " << blue << ")" << std::endl;
    }
};


struct EdgeSample
{
    int minY;
    int maxY;
    float xOfYMin;
    float inverseSlope;

    EdgeSample()
    {
        this->minY = 0;
        this->maxY = 0;
        this->xOfYMin = 0;
        this->inverseSlope = 0.0f;
    }

    EdgeSample(int minY, int maxY, int xOfYMin, float inverseSlope)
    {
        this->minY = minY;
        this->maxY = maxY;
        this->xOfYMin = xOfYMin;
        this->inverseSlope = inverseSlope;
    }

    void print()
    {
        std::cout << "minY = " << minY << " | maxY = " << maxY << " | xOfYMin = " << xOfYMin << " | inverseSlope = " << inverseSlope << std::endl;
    }
};

struct EdgeTable
{
    int maxNumberOfVertices;
    int currentNumberOfVertices;
    struct EdgeSample* table;

    EdgeTable(int maxNumberOfVertices)
    {
        this->maxNumberOfVertices = maxNumberOfVertices;
        table = new struct EdgeSample[maxNumberOfVertices];
        currentNumberOfVertices = 0;
    }

    void add(int x1, int y1, int x2, int y2)
    {
        // Skip horizontal lines
        if (y1 == y2)
        {
            return;
        }

        float slope, inverseSlope;
        int minY, maxY, x;
        if (x2 == x1)
        {
            inverseSlope = 0.0f;
        }
        else
        {
            slope = ((float)(y2 - y1)) / ((float)(x2 - x1));
            inverseSlope = 1.0f / slope;
        }
        if (y1 > y2)
        {
            maxY = y1;
            minY = y2;
            x = x2;
        }
        else
        {
            maxY = y2;
            minY = y1;
            x = x1;
        }
        table[currentNumberOfVertices++] = EdgeSample(minY, maxY, x, inverseSlope);
    }

    void add(EdgeSample edgeSample)
    {
        table[currentNumberOfVertices++] = edgeSample;
    }

    // Bubble sort cause array is small and I'm lazy
    void sortByY()
    {
        for (int i = 0; i < currentNumberOfVertices - 1; i++)
        {
            for (int j = 0; j < currentNumberOfVertices - i - 1; j++)
            {
                if (table[j].minY < table[j + 1].minY)
                {
                    EdgeSample t = table[j];
                    table[j] = table[j + 1];
                    table[j + 1] = t;
                }
                else if (table[j].minY == table[j + 1].minY)
                {
                    if (table[j].maxY < table[j + 1].maxY)
                    {
                        EdgeSample t = table[j];
                        table[j] = table[j + 1];
                        table[j + 1] = t;
                    }
                }
            }
        }
    }

    void sortByX()
    {
        for (int i = 0; i < currentNumberOfVertices - 1; i++)
        {
            for (int j = 0; j < currentNumberOfVertices - i - 1; j++)
            {
                if (table[j].xOfYMin > table[j + 1].xOfYMin)
                {
                    EdgeSample t = table[j];
                    table[j] = table[j + 1];
                    table[j + 1] = t;
                }
            }
        }
    }

    void print()
    {
        std::cout << "EdgeTable:\n";
        for (int i = 0; i < currentNumberOfVertices; i++)
        {
            table[i].print();
            std::cout << std::endl;
        }
    }

    bool isEmpty()
    {
        return currentNumberOfVertices == 0;
    }

    int getMinYMin()
    {
        if (isEmpty())
        {
            return 0;
        }
        return table[currentNumberOfVertices - 1].minY;
    }
    int getMaxYMin()
    {
        if (isEmpty())
        {
            return 0;
        }
        return table[0].minY;
    }

    EdgeSample getLastEdgeSample()
    {
        return table[currentNumberOfVertices - 1];
    }

    EdgeSample removeLastEdgeSample()
    {
        return table[currentNumberOfVertices-- - 1];
    }

    void updateX()
    {
        for (int i = 0; i < currentNumberOfVertices; i++)
        {
            table[i].xOfYMin = (float) table[i].xOfYMin + table[i].inverseSlope;
        }
    }

    void clearYYMax(int y)
    {
        for (int i = 0; i < currentNumberOfVertices; i++)
        {
            if (y >= table[i].maxY)
            {
                for (int j = i; j < currentNumberOfVertices - 1; j++)
                {
                    table[j] = table[j + 1];
                }
                currentNumberOfVertices--;
                i--;
            }
        }
    }

    EdgeTable& operator=(const EdgeTable& other)
    {
        maxNumberOfVertices = other.maxNumberOfVertices;
        currentNumberOfVertices = other.currentNumberOfVertices;
        table = new struct EdgeSample[maxNumberOfVertices];
        for (int i = 0; i < currentNumberOfVertices; i++)
        {
            table[i] = other.table[i];
        }
    }
};

// Window functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
// Drawing functions
void drawPixel(Shader& shader, int x, int y, glm::mat4& transform);
void drawCircle(Shader& shader, int x, int y, int x1, int y1, glm::mat4& transform);
void drawLine(Shader& shader, int x1, int y1, int x2, int y2, glm::mat4& transform);
void floodFill(Shader& shader, int x, int y, glm::mat4& transform);

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

// Colors
struct Color purple(0.5529f, 0.1647f, 0.7804f);
struct Color yellow(1.0f, 0.9215f, 0.2196f);
struct Color black(1.0f, 1.0f, 1.0f);

int cells[41][41] = {0};
void printCells()
{
    for (int i = 0; i < 41; i++)
    {
        for (int j = 0; j < 41; j++)
        {
            std::cout << cells[i][j] << " ";
        }
        std::cout << '\n';
    }
}

void drawCellPixel(int x, int y)
{
    // std::cout << "base cell: " << x << " " << y << std::endl;
    if (y > 0)
    {
        y = 20 - y;
    }
    else if (y <= 0)
    {
        y = abs(y) + 20;
    }
    // std::cout << "drawing in: cells[" << y << "][" << x + 20 << "]" << std::endl;
    cells[y][x + 20] = 1;
}

bool isCellFilled(int x, int y)
{
    if (y > 0)
    {
        y = 20 - y;
    }
    else if (y <= 0)
    {
        y = abs(y) + 20;
    }
    return cells[y][x + 20] == 1;
}

int main(int argc, char* argv[])
{
    std::cout << "test: " << sizeof(cells) << std::endl;
    std::cout << "argc = " << argc << std::endl;
    float x1 = 0.0f, y1 = 0.0f;
    float x2 = 0.0f, y2 = 0.0f;
    float x3 = 0.0f, y3 = 0.0f;
    float circleRadius = 0.0f;
    int drawType = 0;
    int drawNumberOfVertices = 0;
    int currentArgv = 2;
    float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
    float rotate = 0.0f;
    float translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;
    float scaleOX = 1.0f, scaleOY = 1.0f, scaleOZ = 1.0f;
    int polygonNumberOfVertices;
    int* polygonVertices;
    glm::mat4 userTransform = glm::mat4(1.0f);
    struct EdgeTable *originEdgeTable;
    int xStartFill = 0, yStartFill = 0;
    bool toFill = false;

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
    else if (*argv[1] == 'g')
    {
        std::cout << "Show polygon" << std::endl;
        polygonNumberOfVertices = std::stoi(std::string(argv[currentArgv++]));
        polygonVertices = new int[polygonNumberOfVertices * 2];
        for (int i = 0; i < polygonNumberOfVertices * 2; i++)
        {
            polygonVertices[i] = std::stoi(std::string(argv[currentArgv++]));
        }
        originEdgeTable = new struct EdgeTable(polygonNumberOfVertices);
        for (int i = 0; i < polygonNumberOfVertices * 2 - 2; i += 2)
        {
            originEdgeTable->add(polygonVertices[i], polygonVertices[i + 1], polygonVertices[i + 2], polygonVertices[i + 3]);
        }
        originEdgeTable->add(polygonVertices[0], polygonVertices[1], polygonVertices[polygonNumberOfVertices * 2 - 2], polygonVertices[polygonNumberOfVertices * 2 - 1]);
        originEdgeTable->print();
        originEdgeTable->sortByY();
        originEdgeTable->print();
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
        else if (*argv[currentArgv] == 'f')
        {
            currentArgv++;
            std::cout << "Doing fill" << std::endl;
            toFill = true;
            xStartFill = std::stoi(std::string(argv[currentArgv++]));
            yStartFill = std::stoi(std::string(argv[currentArgv]));
            std::cout << "start fill from: " << xStartFill << " " << yStartFill << std::endl;
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

        struct Color currentColor = black;
        // ----------------------
        // Draw figures
        if (*argv[1] == 'l')
        {
            currentColor = purple;
            ourShader.setVec3("ourColor", glm::vec3(currentColor.red, currentColor.green, currentColor.blue));
            glBindVertexArray(squareVAO);
            drawLine(ourShader, x1, y1, x2, y2, transform);
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
            }
        }
        else if (*argv[1] == 'p')
        {
            glBindVertexArray(squareVAO);
            drawPixel(ourShader, x1, y1, transform);
        }
        else if (*argv[1] == 'g')
        {
            currentColor = yellow;
            ourShader.setVec3("ourColor", glm::vec3(currentColor.red, currentColor.green, currentColor.blue));
            glBindVertexArray(squareVAO);
            for (int i = 0; i < polygonNumberOfVertices * 2 - 2; i += 2)
            {
                drawLine(ourShader, polygonVertices[i], polygonVertices[i + 1], polygonVertices[i + 2], polygonVertices[i + 3], transform);
            }
            drawLine(ourShader, polygonVertices[0], polygonVertices[1], polygonVertices[polygonNumberOfVertices * 2 - 2], polygonVertices[polygonNumberOfVertices * 2 - 1], transform);
        }
        else
        {
            ourShader.setMat4("transform", userTransform);
            glDrawArrays(drawType, 0, drawNumberOfVertices);
        }
        if (toFill)
        {
            glBindVertexArray(squareVAO);
            int tempCells[41][41];
            std::memcpy(tempCells, cells, sizeof(cells));
            floodFill(ourShader, xStartFill, yStartFill, transform);
            std::memcpy(cells, tempCells, sizeof(tempCells));
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
    drawCellPixel(x, y);
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

void drawLine(Shader& shader, int x1, int y1, int x2, int y2, glm::mat4& transform)
{
    int dx, dy, i, e;
    int incX, incY, inc1, inc2;
    int x, y;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0)
    {
        dx = -dx;
    }
    if (dy < 0)
    {
        dy = -dy;
    }

    incX = 1;
    if (x2 < x1)
    {
        incX = -1;
    }

    incY = 1;
    if (y2 < y1)
    {
        incY = -1;
    }
    x = x1; y = y1;
    if (dx > dy)
    {
        drawPixel(shader, x, y, transform);
        e = 2 * dy - dx;
        inc1 = 2 * (dy - dx);
        inc2 = 2 * dy;
        for (i = 0; i < dx; i++)
        {
            if (e >= 0)
            {
                y += incY;
                e += inc1;
            }
            else
            {
                e += inc2;
            }
            x += incX;
            drawPixel(shader, x, y, transform);
        }

    }
    else
    {
        drawPixel(shader, x, y, transform);
        e = 2 * dx - dy;
        inc1 = 2 * (dx - dy);
        inc2 = 2 * dx;
        for (i = 0; i < dy; i++)
        {
            if (e >= 0)
            {
                x += incX;
                e += inc1;
            }
            else
            {
                e += inc2;
            }
            y += incY;
            drawPixel(shader, x, y, transform);
        }
    }
}

void floodFill(Shader& shader, int x, int y, glm::mat4& transform)
{
    if (isCellFilled(x, y))
    {
        return;
    }
    drawPixel(shader, x, y, transform);
    floodFill(shader, x + 1, y, transform);
    floodFill(shader, x - 1, y, transform);
    floodFill(shader, x, y + 1, transform);
    floodFill(shader, x, y - 1, transform);
}