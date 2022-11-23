#version 330 core
layout (location = 0) in vec3 aPos_text;
layout (location = 1) in vec3 aColor_text;
layout (location = 2) in vec2 aTexCoord_text;

out vec3 ourColor_text;
out vec2 TexCoord_text;

void main()
{
    gl_Position = vec4(aPos_text, 1.0);
    ourColor_text = aColor_text;
    TexCoord_text = aTexCoord_text;
}