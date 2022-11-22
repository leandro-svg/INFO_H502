#version 330 core
layout (location = 0) in vec3 aPos2;
layout (location = 1) in vec3 aColor2;

out vec3 ourColor2;

void main()
{
    gl_Position = vec4(aPos2, 1.0);
    ourColor2 = aColor2;
}