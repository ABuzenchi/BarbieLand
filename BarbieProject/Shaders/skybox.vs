#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords; // Pass texture coordinates to the fragment shader

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos; // Use the position as texture coordinates
    gl_Position = projection * view * vec4(aPos, 1.0);
}