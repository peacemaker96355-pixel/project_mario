#include <iostream>

// في البداية، نستدعي الأدوات الأساسية: GLEW للتواصل مع كارت الشاشة، و GLFW لإدارة النافذة.
#define GLEW_STATIC 
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// هنا نحدد أبعاد الشاشة التي سيتم فتحها (العرض 800 والطول 600 بكسل).
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// هذه هي "المظللات" وهي أكواد صغيرة تُرسل للـ GPU لتحدد إحداثيات النقاط ولونها النهائي.
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
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
    // نبدأ بتجهيز محرك GLFW، ونخبره أننا نريد استخدام OpenGL إصدار 3.3 الحديث.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // نقوم بإنشاء النافذة وتحديد عنوانها، ثم نربط أوامر OpenGL بها ونشغل مكتبة GLEW.
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mario 2D Scene with EBO", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    // الآن نقوم بترجمة أكواد الشيدر (Vertex & Fragment) وربطها معاً لتكوين برنامج واحد يعمل على الـ GPU.
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

    // بعد الربط، نمسح الشيدرات الفردية لتوفير مساحة في الذاكرة.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // نعرف إحداثيات النقاط لـ 4 أشكال: الأرضية، جسم الأنبوب، رأسه، وصندوق المفاجآت.
    float vertices[] = {
        // نقاط الأرضية
        -1.0f, -0.5f, 0.0f,
         1.0f, -0.5f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,

        // نقاط جسم الأنبوب
         0.4f,  0.1f, 0.0f,
         0.8f,  0.1f, 0.0f,
         0.8f, -0.5f, 0.0f,
         0.4f, -0.5f, 0.0f,

         // نقاط رأس الأنبوب
          0.35f, 0.25f, 0.0f,
          0.85f, 0.25f, 0.0f,
          0.85f, 0.1f,  0.0f,
          0.35f, 0.1f,  0.0f,

          // نقاط الصندوق
          -0.5f,  0.4f, 0.0f,
          -0.2f,  0.4f, 0.0f,
          -0.2f,  0.1f, 0.0f,
          -0.5f,  0.1f, 0.0f
    };

    // نحدد كيف نربط هذه النقاط لتكوين مثلثات (كل 6 أرقام هنا ترسم مربعاً واحداً).
    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12
    };

    // ننشئ كائنات التخزين (VAO, VBO, EBO) وننقل بيانات النقاط والفهارس من الرام إلى ذاكرة كارت الشاشة.
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // نخبر OpenGL بكيفية تفسير بيانات النقاط (كل نقطة تتكون من 3 أرقام عشرية).
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // تبدأ هنا حلقة الرسم التي تتكرر باستمرار لتعرض المشهد على الشاشة.
    while (!glfwWindowShouldClose(window))
    {
        // نمسح الشاشة ونصبغها باللون الأزرق الفاتح (لون السماء).
        glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // نفعل برنامج الشيدر ونستعد لإرسال الألوان وتفعيل بيانات النقاط.
        glUseProgram(shaderProgram);
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glBindVertexArray(VAO);

        // نرسم الأرضية باللون البني باستخدام أول 6 فهارس في المصفوفة.
        glUniform4f(vertexColorLocation, 0.5f, 0.3f, 0.1f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

        // نرسم جسم الأنبوب بالأخضر الداكن، مع إزاحة القراءة بمقدار 6 فهارس.
        glUniform4f(vertexColorLocation, 0.0f, 0.5f, 0.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

        // نرسم رأس الأنبوب بالأخضر الفاتح، مع إزاحة القراءة بمقدار 12 فهرساً.
        glUniform4f(vertexColorLocation, 0.0f, 0.8f, 0.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));

        // وأخيراً نرسم الصندوق بالبرتقالي، مع إزاحة القراءة بمقدار 18 فهرساً.
        glUniform4f(vertexColorLocation, 1.0f, 0.6f, 0.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));

        // نبدل الشاشة الخلفية بالأمامية ونعالج أحداث الماوس والكيبورد.
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // بمجرد إغلاق النافذة، نقوم بتنظيف الذاكرة وحذف كل ما أنشأناه.
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}