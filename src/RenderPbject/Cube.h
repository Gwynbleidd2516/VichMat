#ifndef CUBE
#define CUBE

#include "RenderObject.h"

#define CUBE_VERTEX_SHADER "#version 330 core\n"                                           \
                           "layout (location = 0) in vec3 aPos;\n"                         \
                           "out vec3 aGeom;\n"                                             \
                           "uniform vec2 mPosition;\n"                                     \
                           "uniform mat4 mView;\n"                                         \
                           "void main()\n"                                                 \
                           "{\n"                                                           \
                           "gl_Position = mView*vec4(aPos.xy + mPosition, aPos.z, 1.0);\n" \
                           "aGeom = aPos;\n"                                               \
                           "}\n"

#define CUBE_FRAGMENT_SHADER "#version 330 core\n"    \
                             "in vec3 aGeom;\n"       \
                             "out vec4 FragColor;\n"  \
                             "uniform vec4 mColor;\n" \
                             "void main()\n"          \
                             "{\n"                    \
                             "FragColor = mColor;\n"  \
                             "}\n"

class Cube : public RenderObject
{
private:
    /* data */
public:
    Cube()
    {
        mShader.loadShaderFromString(CUBE_VERTEX_SHADER, CUBE_FRAGMENT_SHADER);

        // Vertex data for a triangle (positions and colors)
        GLfloat vertices[] = {
            // задняя грань (z = -1)
            -1.0f, -1.0f, -1.0f, // 0
            1.0f, -1.0f, -1.0f,  // 1
            1.0f, 1.0f, -1.0f,   // 2
            -1.0f, 1.0f, -1.0f,  // 3
            // передняя грань (z = 1)
            -1.0f, -1.0f, 1.0f, // 4
            1.0f, -1.0f, 1.0f,  // 5
            1.0f, 1.0f, 1.0f,   // 6
            -1.0f, 1.0f, 1.0f   // 7
        };

        GLuint indices[] = {
            // задняя грань
            0, 2, 1,
            0, 3, 2,
            // передняя грань
            4, 5, 6,
            4, 6, 7,
            // левая грань (x = -1)
            0, 4, 7,
            0, 7, 3,
            // правая грань (x = 1)
            1, 6, 5,
            1, 2, 6,
            // нижняя грань (y = -1)
            0, 1, 5,
            0, 5, 4,
            // верхняя грань (y = 1)
            3, 6, 2,
            3, 7, 6
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

    ~Cube()
    {
        glDeleteVertexArrays(1, &mVAO);
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mEBO);
    }
};

#endif