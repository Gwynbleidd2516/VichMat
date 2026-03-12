#include"RenderObject.h"

void RenderObject::render() const
{
    mShader.useShader();
    // glUseProgram(mShaderProgram);
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, mCount); // Draw the 3 vertices of the triangle
    glBindVertexArray(0);
    glUseProgram(0);
}