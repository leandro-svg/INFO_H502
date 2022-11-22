#version 330 core
out vec4 FragColor2;

in vec3 ourColor2;

void main()
{
    FragColor2 = vec4(ourColor2, 1.0f);
}