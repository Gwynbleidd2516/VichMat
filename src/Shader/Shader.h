#ifndef SHADER
#define SHADER

#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string_view>
#include <glm/glm.hpp>

class Shader
{
protected:
    unsigned int mShaderProgram;

    void generateVertexShader(unsigned int &, std::string_view);

    void generateFragmentShader(unsigned int &, std::string_view);

    GLint __getLocation(std::string name) const;

public:
    Shader();

    void loadShaderFromString(std::string_view vertex, std::string_view fragment);

    void useShader() const;
    // ------------------------------------------------------------- //
    void setUniform1i(std::string name, int value) const;

    void setUniform2i(std::string name, glm::vec<2, int> value) const;

    void setUniform3i(std::string name, glm::vec<3, int> value) const;

    void setUniform4i(std::string name, glm::vec<4, int> value) const;
    // ------------------------------------------------------------- //
    void setUniform1ui(std::string name, unsigned int value) const;

    void setUniform2ui(std::string name, glm::vec<2, unsigned int> value) const;

    void setUniform3ui(std::string name, glm::vec<3, unsigned int> value) const;

    void setUniform4ui(std::string name, glm::vec<4, unsigned int> value) const;
    // ------------------------------------------------------------- //
    void setUniform1i64(std::string name, GLint64 value) const;

    void setUniform2i64(std::string name, glm::vec<2, GLint64> value) const;

    void setUniform3i64(std::string name, glm::vec<3, GLint64> value) const;

    void setUniform4i64(std::string name, glm::vec<4, GLint64> value) const;
    // ------------------------------------------------------------- //
    void setUniform1ui64(std::string name, GLuint64 value) const;

    void setUniform2ui64(std::string name, glm::vec<2, GLuint64> value) const;

    void setUniform3ui64(std::string name, glm::vec<3, GLuint64> value) const;

    void setUniform4ui64(std::string name, glm::vec<4, GLuint64> value) const;
    // ------------------------------------------------------------- //
    void setUniform1d(std::string name, double value) const;

    void setUniform2d(std::string name, glm::vec<2, double> value) const;

    void setUniform3d(std::string name, glm::vec<3, double> value) const;

    void setUniform4d(std::string name, glm::vec<4, double> value) const;
    // ------------------------------------------------------------- //
    void setUniform1f(std::string name, float value) const;

    void setUniform2f(std::string name, glm::vec2 value) const;

    void setUniform3f(std::string name, glm::vec3 value) const;

    void setUniform4f(std::string name, glm::vec4 value) const;

    ~Shader();
};

#endif