#version 330 core
in vec4 vColor; // Input color from the vertex shader
out vec4 FragColor; // Final output color

void main()
{
    FragColor = vColor; // Set the fragment color
}
