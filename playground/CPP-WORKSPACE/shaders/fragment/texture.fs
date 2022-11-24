#version 330 core
out vec4 FragColor_text;
  
in vec3 ourColor_text;
in vec2 TexCoord_text;

uniform sampler2D ourTexture_text;
uniform sampler2D texture_text2;


void main()
{
    FragColor_text = mix(texture(ourTexture_text, TexCoord_text), texture(texture_text2, TexCoord_text), 0.5);
}