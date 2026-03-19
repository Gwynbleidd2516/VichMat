#ifndef TEST_OBJECT
#define TEST_OBJECT

#include "RenderObject.h"
#include "Shader.h"
#include <string>
#include <fstream>
#include <iostream>

#define VERTEX_SHADER "#version 330 core\n"                                \
                      "layout (location = 0) in vec3 aPos;\n"              \
                      "layout (location = 1) in vec4 aColor;\n"            \
                      "out vec4 vColor;\n"                                 \
                      "void main()\n"                                      \
                      "{\n"                                                \
                      "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" \
                      "vColor = aColor;\n"                                 \
                      "}\n"

#define FRAGMENT_SHADER "#version 330 core\n"   \
                        "in vec4 vColor;\n"     \
                        "out vec4 FragColor;\n" \
                        "void main()\n"         \
                        "{\n"                   \
                        "FragColor = vColor;\n" \
                        "}\n"

class TestObject : public RenderObject
{
private:
public:
    TestObject()
    {
        mShader.loadShaderFromString(VERTEX_SHADER, FRAGMENT_SHADER);

        // Vertex data for a triangle (positions and colors)
        float vertices[] = {
            // Positions         // Colors (R, G, B, A)
            0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,   // Top (Red)
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // Bottom Left (Green)
            0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f   // Bottom Right (Blue)
        };

        // Generate and bind Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Link vertex attributes
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        mCount = 3;
    }

    ~TestObject()
    {
        glDeleteVertexArrays(1, &mVAO);
        glDeleteBuffers(1, &mVBO);
    }
};

#endif