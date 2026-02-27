#include <iostream>
#include <cmath> // أضفنا هذه المكتبة لنتمكن من استخدام دالة sin() للتحريك الناعم

// 1. استدعاء مكتبات OpenGL
#define GLEW_STATIC 
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// --- 2. كود المظللات (Shaders) ---
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform float xOffset;\n" // المتغير الجديد للتحريك
"void main()\n"
"{\n"
 
"   gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = ourColor;\n"
"}\n\0";

int main()
{
    // --- 3. تهيئة GLFW و GLEW ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mario 2D Scene - Moving Box", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    // --- 4. بناء برنامج الشيدر ---
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- 5. تعريف الرؤوس (Vertices) ---
    float vertices[] = {
        // الأرضية
        -1.0f, -0.5f, 0.0f,
         1.0f, -0.5f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,

        // جسم الأنبوب
         0.4f,  0.1f, 0.0f,
         0.8f,  0.1f, 0.0f,
         0.8f, -0.5f, 0.0f,
         0.4f, -0.5f, 0.0f,

         // رأس الأنبوب
          0.35f, 0.25f, 0.0f,
          0.85f, 0.25f, 0.0f,
          0.85f, 0.1f,  0.0f,
          0.35f, 0.1f,  0.0f,

          // الصندوق
          -0.5f,  0.4f, 0.0f,
          -0.2f,  0.4f, 0.0f,
          -0.2f,  0.1f, 0.0f,
          -0.5f,  0.1f, 0.0f
    };

    // --- 6. الفهارس (Indices) ---
    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12
    };

    // --- 7. إعداد Buffer Objects ---
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- 8. حلقة الرسم (الأنيميشن) ---
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // جلب عناوين متغيرات الـ Uniform للون والتحريك
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        int xOffsetLocation = glGetUniformLocation(shaderProgram, "xOffset");

        glBindVertexArray(VAO);

        // --- رسم الأشكال الثابتة ---
        // نعطي قيمة xOffset صفر (0.0f) لكي لا تتحرك الأرضية والأنبوب
        glUniform1f(xOffsetLocation, 0.0f);

        // 1. الأرضية
        glUniform4f(vertexColorLocation, 0.5f, 0.3f, 0.1f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // 2. جسم الأنبوب
        glUniform4f(vertexColorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

        // 3. رأس الأنبوب
        glUniform4f(vertexColorLocation, 0.0f, 0.8f, 0.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));

        // نحسب قيمة الحركة باستخدام الوقت ودالة sin لتكون الحركة يميناً ويساراً (بين -0.5 و 0.5)
        float timeValue = glfwGetTime();
        float moveX = sin(timeValue) * 0.5f;

        // نرسل قيمة الحركة الجديدة للشيدر
        glUniform1f(xOffsetLocation, moveX);

        // نرسم الصندوق
        glUniform4f(vertexColorLocation, 1.0f, 0.6f, 0.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}
