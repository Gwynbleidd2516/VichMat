#ifndef CIRCLE
#define CIRCLE

#include "RenderObject.h"

#define CIRCLE_VERTEX_SHADER "#version 330 core\n"                                     \
                             "layout (location = 0) in vec3 aPos;\n"                   \
                             "out vec3 aGeom;\n"                                       \
                             "uniform vec2 mPosition;\n"                               \
                             "void main()\n"                                           \
                             "{\n"                                                     \
                             "gl_Position = vec4(aPos.xy + mPosition, aPos.z, 1.0);\n" \
                             "aGeom = aPos;\n"                                         \
                             "}\n"

#define CIRCLE_FRAGMENT_SHADER "#version 330 core\n"                             \
                               "in vec3 aGeom;\n"                                \
                               "out vec4 FragColor;\n"                           \
                               "uniform vec4 mColor;\n"                          \
                               "void main()\n"                                   \
                               "{\n"                                             \
                               "if(aGeom.x*aGeom.x + aGeom.y*aGeom.y <= 0.25)\n" \
                               "{FragColor = mColor;}\n"                         \
                               "else\n"                                          \
                               "{FragColor = vec4(0.0);}\n"                      \
                               "}\n"

class Circle : public RenderObject
{
public:
    Circle()
    {
        mShader.loadShaderFromString(CIRCLE_VERTEX_SHADER, CIRCLE_FRAGMENT_SHADER);

        // Vertex data for a triangle (positions and colors)
        GLfloat vertices[] = {
            0.5f, 0.5f, 0.0f,   // Top Right
            0.5f, -0.5f, 0.0f,  // Bottom Right
            -0.5f, -0.5f, 0.0f, // Bottom Left
            -0.5f, 0.5f, 0.0f   // Top Left
        };

        GLuint indices[] = {
            0, 1, 3, // First triangle (Top Right, Bottom Right, Top Left)
            1, 2, 3  // Second triangle (Bottom Right, Bottom Left, Top Left)
        };

        // Generate and bind Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);

        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Generate and bind an Element Buffer Object (EBO)
        glGenBuffers(1, &mEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Link vertex attributes
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        mShader.setUniform4f("mColor", glm::vec4(0.0, 1.0, 1.0, 1.0));

        mCount = 6;
    }

    void render() const override
    {
        mShader.useShader();
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void setPosition(glm::vec2 pos)
    {
        mShader.setUniform2f("mPosition", pos);
    }

    ~Circle()
    {
        glDeleteVertexArrays(1, &mVAO);
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mEBO);
    }
};

#endif