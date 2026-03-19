#ifndef POINT
#define POINT

#include "RenderObject.h"
#include <glm/glm.hpp>

#define POINT_VERTEX_SHADER "#version 330 core\n"                                \
                            "layout (location = 0) in vec3 aPos;\n"              \
                             "uniform mat4 mView;\n"                          \
                            "void main()\n"                                      \
                            "{\n"                                                \
                            "gl_Position = mView*vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" \
                            "gl_PointSize = 12f;\n"                             \
                            "}\n"

#define POINT_FRAGMENT_SHADER "#version 330 core\n"    \
                              "out vec4 FragColor;\n"  \
                              "uniform vec4 mColor;\n" \
                              "void main()\n"          \
                              "{\n"                    \
                              "FragColor = mColor;\n"  \
                              "}\n"

class Point : public RenderObject
{
private:
    /* data */
public:
    Point()
    {
        mShader.loadShaderFromString(POINT_VERTEX_SHADER, POINT_FRAGMENT_SHADER);

        // Vertex data for a triangle (positions and colors)
        float vertices[] = {0.0, 0.0, 0.0};

        // Generate and bind Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Link vertex attributes
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        mShader.setUniform4f("mColor", glm::vec4(0.0, 1.0, 1.0, 1.0));

        mCount = 1;
    }

    ~Point()
    {
        glDeleteVertexArrays(1, &mVAO);
        glDeleteBuffers(1, &mVBO);
    }
};

#endif