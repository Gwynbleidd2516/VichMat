#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// Shader program handle
GLuint shaderProgram;
GLuint VBO; // Vertex Buffer Object
GLuint VAO; // Vertex Array Object

// Function to load shader from file
std::string loadShaderSource(const char* filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        std::cerr << "Could not open file " << filePath << std::endl;
        return "";
    }
    std::stringstream sstr;
    sstr << fileStream.rdbuf();
    content = sstr.str();
    fileStream.close();
    return content;
}

// Function to compile shaders
GLuint CompileShader(GLuint type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, NULL);
    glCompileShader(shader);
    // Error checking (omitted for brevity, but crucial for development)
    return shader;
}

// Function to link shaders into a program
GLuint CreateProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint program = glCreateProgram();
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    // Error checking (omitted for brevity)
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void init() {
    // Load and create shaders
    std::string vertexSource = loadShaderSource("shader.vert");
    std::string fragmentSource = loadShaderSource("shader.frag");
    shaderProgram = CreateProgram(vertexSource, fragmentSource);

    // Vertex data for a triangle (positions and colors)
    float vertices[] = {
        // Positions         // Colors (R, G, B, A)
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, // Top (Red)
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, // Bottom Left (Green)
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f  // Bottom Right (Blue)
    };

    // Generate and bind Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Link vertex attributes
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the 3 vertices of the triangle
    glBindVertexArray(0);
    glUseProgram(0);
    glutSwapBuffers(); // Swap the front and back buffers
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) // Escape key
        glutLeaveMainLoop();
}

int main(int argc, char** argv) {
    std::cout << argv[0] << std::endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("FreeGLUT Shader Example");

    // Initialize GLEW (must be done after window creation)
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }
    std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop(); // Start the FreeGLUT event loop

    return 0;
}
