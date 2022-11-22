#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{   
    vec3 position = vec3(aPos.x, -1.0*aPos.y, aPos.z);
    gl_Position = vec4(position, 1.0);
    ourColor = aColor;
}