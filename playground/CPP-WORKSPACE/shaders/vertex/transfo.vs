#version 330 core
layout (location = 0) in vec3 aPos_text;
layout (location = 2) in vec2 aTexCoord_text;

out vec2 TexCoord_text;
  
uniform mat4 transform_text;

void main()
{
    gl_Position = transform_text * vec4(aPos_text, 1.0f);
    TexCoord_text = vec2(aTexCoord_text.x, aTexCoord_text.y);
} 