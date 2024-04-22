#version 330 core
out vec4 FragColor;

in vec3 TexCoords; // Received from the vertex shader

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);
}