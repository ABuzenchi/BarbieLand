#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
uniform samplerCube skyboxDay;
uniform samplerCube skyboxNight;
uniform float mixValue;

void main()
{
    vec4 dayColor = texture(skyboxDay, TexCoords);
    vec4 nightColor = texture(skyboxNight, TexCoords);
    FragColor = mix(dayColor, nightColor, mixValue);
}