#include "Shader.h"
#include <iostream>

void Shader::generateVertexShader(unsigned int &shader, std::string_view txt)
{
    shader = glCreateShader(GL_VERTEX_SHADER);
    const char *src = txt.data();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
}

void Shader::generateFragmentShader(unsigned int &shader, std::string_view txt)
{
    shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *src = txt.data();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
}

GLint Shader::__getLocation(std::string name) const
{
    glUseProgram(mShaderProgram);
    GLint loc = glGetUniformLocation(mShaderProgram, name.c_str());
    if (loc == -1)
    {
        std::string ans = "Cannot find Uniform: ";
        ans += name;
        std::cerr << ans << std::endl;
    }
    return loc;
}

Shader::Shader()
{
    mShaderProgram = glCreateProgram();
}

void Shader::loadShaderFromString(std::string_view vertex, std::string_view fragment)
{
    unsigned int vertexShader;
    generateVertexShader(vertexShader, vertex);
    unsigned int fragmentShader;
    generateFragmentShader(fragmentShader, fragment);

    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);
    glLinkProgram(mShaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::useShader() const
{
    glUseProgram(mShaderProgram);
}

void Shader::setUniform1i(std::string name, int value) const
{
    glUniform1i(__getLocation(name), value);
}

void Shader::setUniform2i(std::string name, glm::vec<2, int> value) const
{
    glUniform2i(__getLocation(name), value.x, value.y);
}

void Shader::setUniform3i(std::string name, glm::vec<3, int> value) const
{
    glUniform3i(__getLocation(name), value.x, value.y, value.z);
}

void Shader::setUniform4i(std::string name, glm::vec<4, int> value) const
{
    glUniform4i(__getLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setUniform1ui(std::string name, unsigned int value) const
{
    glUniform1ui(__getLocation(name), value);
}

void Shader::setUniform2ui(std::string name, glm::vec<2, unsigned int> value) const
{
    glUniform2ui(__getLocation(name), value.x, value.y);
}

void Shader::setUniform3ui(std::string name, glm::vec<3, unsigned int> value) const
{
    glUniform3ui(__getLocation(name), value.x, value.y, value.z);
}

void Shader::setUniform4ui(std::string name, glm::vec<4, unsigned int> value) const
{
    glUniform4ui(__getLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setUniform1i64(std::string name, GLint64 value) const
{
    glUniform1i64ARB(__getLocation(name), value);
}

void Shader::setUniform2i64(std::string name, glm::vec<2, GLint64> value) const
{
    glUniform2i64ARB(__getLocation(name), value.x, value.y);
}

void Shader::setUniform3i64(std::string name, glm::vec<3, GLint64> value) const
{
    glUniform3i64ARB(__getLocation(name), value.x, value.y, value.z);
}

void Shader::setUniform4i64(std::string name, glm::vec<4, GLint64> value) const
{
    glUniform4i64ARB(__getLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setUniform1ui64(std::string name, GLuint64 value) const
{
    glUniform1ui64ARB(__getLocation(name), value);
}

void Shader::setUniform2ui64(std::string name, glm::vec<2, GLuint64> value) const
{
    glUniform2ui64ARB(__getLocation(name), value.x, value.y);
}

void Shader::setUniform3ui64(std::string name, glm::vec<3, GLuint64> value) const
{
    glUniform3ui64ARB(__getLocation(name), value.x, value.y, value.z);
}

void Shader::setUniform4ui64(std::string name, glm::vec<4, GLuint64> value) const
{
    glUniform4ui64ARB(__getLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setUniform1d(std::string name, double value) const
{
    glUniform1d(__getLocation(name), value);
}

void Shader::setUniform2d(std::string name, glm::vec<2, double> value) const
{
    glUniform2d(__getLocation(name), value.x, value.y);
}

void Shader::setUniform3d(std::string name, glm::vec<3, double> value) const
{
    glUniform3d(__getLocation(name), value.x, value.y, value.z);
}

void Shader::setUniform4d(std::string name, glm::vec<4, double> value) const
{
    glUniform4d(__getLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setUniform1f(std::string name, float value) const
{
    glUniform1f(__getLocation(name), value);
}

void Shader::setUniform2f(std::string name, glm::vec2 value) const
{
    glUniform2f(__getLocation(name), value.x, value.y);
}

void Shader::setUniform3f(std::string name, glm::vec3 value) const
{
    glUniform3f(__getLocation(name), value.x, value.y, value.z);
}

void Shader::setUniform4f(std::string name, glm::vec4 value) const
{
    glUniform4f(__getLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setUniformMatrix2fv(std::string name, glm::mat2 value) const
{
    glUniformMatrix2fv(__getLocation(name), 1, false, glm::value_ptr(value));
}

void Shader::setUniformMatrix3fv(std::string name, glm::mat3 value) const
{
    glUniformMatrix3fv(__getLocation(name), 1, false, glm::value_ptr(value));
}

void Shader::setUniformMatrix4fv(std::string name, glm::mat4 value) const
{
    glUniformMatrix4fv(__getLocation(name), 1, false, glm::value_ptr(value));
}

Shader::~Shader()
{
    glDeleteProgram(mShaderProgram);
}
