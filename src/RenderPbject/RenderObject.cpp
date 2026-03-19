#include"RenderObject.h"

void RenderObject::render(glm::mat4 view) const
{
    mShader.setUniformMatrix4fv("mView", view);

    mShader.useShader();
    glBindVertexArray(mVAO);
    glDrawArrays(GL_POINTS, 0, mCount); // Draw the 3 vertices of the triangle
    glBindVertexArray(0);
    glUseProgram(0);
}

// void RenderObject::setView(glm::mat4 view) const
// {
//     mShader.setUniformMatrix4fv("mView", view);
// }
