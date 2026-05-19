#include "glad.h"
#include "glfw3.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

// ================= WINDOW SETTINGS =================
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ================= SHADERS =================

// Vertex Shader
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos, 1.0);\n"
                                 "}\0";

// Fragment Shader
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "uniform vec4 ourColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = ourColor;\n"
                                   "}\n\0";

// ================= FUNCTIONS =================

// Window resize callback
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Keyboard input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Convert degrees to radians
float degToRad(float degrees)
{
    return degrees * 3.1415926f / 180.0f;
}

// ================= MAIN =================
int main()
{
    // ================= GLFW INIT =================
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "Analog Clock - Correct Rotation",
        NULL,
        NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ================= GLAD =================
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // ================= SHADERS =================
    
    // Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // Check vertex shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader Program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check linking
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ================= CLOCK CIRCLE =================
    std::vector<float> circleVertices;
    std::vector<float> filledCircleVertices;

    float centerX = 0.0f;
    float centerY = 0.0f;
    float radius = 0.7f;

    // Generate outline circle points (for LINE_LOOP)
    for (int i = 0; i <= 360; i++)
    {
        float angle = degToRad(i);
        float x = centerX + cos(angle) * radius;
        float y = centerY + sin(angle) * radius;
        circleVertices.push_back(x);
        circleVertices.push_back(y);
        circleVertices.push_back(0.0f);
    }
    
    // Generate filled circle points (for TRIANGLE_FAN)
    filledCircleVertices.push_back(centerX);
    filledCircleVertices.push_back(centerY);
    filledCircleVertices.push_back(0.0f);
    
    for (int i = 0; i <= 360; i++)
    {
        float angle = degToRad(i);
        float x = centerX + cos(angle) * radius;
        float y = centerY + sin(angle) * radius;
        filledCircleVertices.push_back(x);
        filledCircleVertices.push_back(y);
        filledCircleVertices.push_back(0.0f);
    }

    // ================= VAO + VBO for Circle =================
    unsigned int circleVAO, circleVBO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    // ================= VAO + VBO for Filled Circle =================
    unsigned int filledCircleVAO, filledCircleVBO;
    glGenVertexArrays(1, &filledCircleVAO);
    glGenBuffers(1, &filledCircleVBO);
    
    glBindVertexArray(filledCircleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, filledCircleVBO);
    glBufferData(GL_ARRAY_BUFFER, filledCircleVertices.size() * sizeof(float), filledCircleVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ================= RENDER LOOP =================
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Get current system time
        time_t now = time(nullptr);
        struct tm *localTime = localtime(&now);

        int seconds = localTime->tm_sec;
        int minutes = localTime->tm_min;
        int hours = localTime->tm_hour % 12;
        
        // Fix: When hours = 0, it should be 12 o'clock
        float displayHours = (hours == 0) ? 12.0f : (float)hours;
        
        // CORRECT ANGLE CALCULATION:
        // In standard math: 0° = right (3 o'clock), 90° = up (12 o'clock)
        // In OpenGL: Y increases upward, X increases rightward
        // For clock: 12 o'clock should be at 90°, moving CLOCKWISE (decreasing angle)
        
        // Clockwise rotation means angles DECREASE as time increases
        // So: angle = 90° - (percentage * 360°)
        
        // Second hand: 360° per 60 seconds, clockwise
        float secondDegrees = 90.0f - (seconds / 60.0f) * 360.0f;
        
        // Minute hand: 360° per 60 minutes, with smooth movement
        float minuteDegrees = 90.0f - ((minutes + seconds / 60.0f) / 60.0f) * 360.0f;
        
        // Hour hand: 360° per 12 hours, with smooth movement
        float hourDegrees = 90.0f - ((displayHours + minutes / 60.0f) / 12.0f) * 360.0f;
        
        // Convert to radians for OpenGL
        float secondAngle = degToRad(secondDegrees);
        float minuteAngle = degToRad(minuteDegrees);
        float hourAngle = degToRad(hourDegrees);

        // Hand lengths
        float secondLength = 0.65f;
        float minuteLength = 0.55f;
        float hourLength = 0.40f;

        // Calculate hand endpoints using standard cos/sin
        // cos = x coordinate, sin = y coordinate
        float secondX = cos(secondAngle) * secondLength;
        float secondY = sin(secondAngle) * secondLength;
        float minuteX = cos(minuteAngle) * minuteLength;
        float minuteY = sin(minuteAngle) * minuteLength;
        float hourX = cos(hourAngle) * hourLength;
        float hourY = sin(hourAngle) * hourLength;

        // Clear screen
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        int colorLoc = glGetUniformLocation(shaderProgram, "ourColor");

        // ================= DRAW FILLED CIRCLE BACKGROUND =================
        glUniform4f(colorLoc, 0.15f, 0.15f, 0.2f, 0.9f);
        glBindVertexArray(filledCircleVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, filledCircleVertices.size() / 3);

        // ================= DRAW CIRCLE OUTLINE =================
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        glBindVertexArray(circleVAO);
        glDrawArrays(GL_LINE_LOOP, 0, circleVertices.size() / 3);

        // ================= DRAW HOUR MARKERS (1-12) =================
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        
        for (int i = 1; i <= 12; i++)
        {
            // Position for hour i: 12 o'clock = 90°, then subtract 30° for each hour (clockwise)
            float angleDegrees = 90.0f - (i * 30.0f);
            float angleRad = degToRad(angleDegrees);
            float markerX = cos(angleRad) * (radius - 0.08f);
            float markerY = sin(angleRad) * (radius - 0.08f);
            
            // Larger dots for hours
            glPointSize(12.0f);
            
            float markerPoint[] = {markerX, markerY, 0.0f};
            unsigned int tempVBO;
            glGenBuffers(1, &tempVBO);
            glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(markerPoint), markerPoint, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);
            glDrawArrays(GL_POINTS, 0, 1);
            glDeleteBuffers(1, &tempVBO);
        }
        
        // Draw minute markers (smaller dots)
        glPointSize(4.0f);
        glUniform4f(colorLoc, 0.7f, 0.7f, 0.7f, 1.0f);
        
        for (int i = 1; i <= 60; i++)
        {
            if (i % 5 == 0) continue; // Skip hour markers
            
            // Position for minute i: start at 90°, subtract 6° for each minute (clockwise)
            float angleDegrees = 90.0f - (i * 6.0f);
            float angleRad = degToRad(angleDegrees);
            float markerX = cos(angleRad) * (radius - 0.05f);
            float markerY = sin(angleRad) * (radius - 0.05f);
            
            float markerPoint[] = {markerX, markerY, 0.0f};
            unsigned int tempVBO;
            glGenBuffers(1, &tempVBO);
            glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(markerPoint), markerPoint, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);
            glDrawArrays(GL_POINTS, 0, 1);
            glDeleteBuffers(1, &tempVBO);
        }

        // ================= DRAW SECOND HAND =================
        glUniform4f(colorLoc, 1.0f, 0.2f, 0.2f, 1.0f);  // Bright red
        glLineWidth(2.0f);
        
        float secondLineVertices[] = {0.0f, 0.0f, 0.0f, secondX, secondY, 0.0f};
        unsigned int lineVBO;
        glGenBuffers(1, &lineVBO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(secondLineVertices), secondLineVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_LINES, 0, 2);
        
        // Draw tip of second hand
        glPointSize(8.0f);
        float secondTip[] = {secondX, secondY, 0.0f};
        glBufferData(GL_ARRAY_BUFFER, sizeof(secondTip), secondTip, GL_STATIC_DRAW);
        glDrawArrays(GL_POINTS, 0, 1);
        glDeleteBuffers(1, &lineVBO);

        // ================= DRAW MINUTE HAND =================
        glUniform4f(colorLoc, 0.2f, 1.0f, 0.2f, 1.0f);  // Bright green
        glLineWidth(4.0f);
        
        float minuteLineVertices[] = {0.0f, 0.0f, 0.0f, minuteX, minuteY, 0.0f};
        glGenBuffers(1, &lineVBO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(minuteLineVertices), minuteLineVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_LINES, 0, 2);
        glDeleteBuffers(1, &lineVBO);

        // ================= DRAW HOUR HAND =================
        glUniform4f(colorLoc, 0.2f, 0.5f, 1.0f, 1.0f);  // Bright blue
        glLineWidth(6.0f);
        
        float hourLineVertices[] = {0.0f, 0.0f, 0.0f, hourX, hourY, 0.0f};
        glGenBuffers(1, &lineVBO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(hourLineVertices), hourLineVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_LINES, 0, 2);
        glDeleteBuffers(1, &lineVBO);

        // ================= DRAW CENTER POINT =================
        glUniform4f(colorLoc, 1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
        glPointSize(14.0f);
        
        float centerPoint[] = {0.0f, 0.0f, 0.0f};
        unsigned int centerVBO;
        glGenBuffers(1, &centerVBO);
        glBindBuffer(GL_ARRAY_BUFFER, centerVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(centerPoint), centerPoint, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_POINTS, 0, 1);
        glDeleteBuffers(1, &centerVBO);

        // Debug output
        static int lastSecond = -1;
        if (seconds != lastSecond)
        {
            std::cout << "Time: " << (displayHours == 12 ? 12 : (int)displayHours) 
                      << ":" << (minutes < 10 ? "0" : "") << minutes 
                      << ":" << (seconds < 10 ? "0" : "") << seconds << std::endl;
            std::cout << "  Second angle: " << secondDegrees << "° -> Position: (" 
                      << secondX << ", " << secondY << ")" << std::endl;
            std::cout << "  Minute angle: " << minuteDegrees << "° -> Position: (" 
                      << minuteX << ", " << minuteY << ")" << std::endl;
            std::cout << "  Hour angle: " << hourDegrees << "° -> Position: (" 
                      << hourX << ", " << hourY << ")" << std::endl;
            lastSecond = seconds;
        }

        // ================= SWAP BUFFERS =================
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ================= CLEANUP =================
    glDeleteVertexArrays(1, &circleVAO);
    glDeleteBuffers(1, &circleVBO);
    glDeleteVertexArrays(1, &filledCircleVAO);
    glDeleteBuffers(1, &filledCircleVBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}