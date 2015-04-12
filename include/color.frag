#version 330 core

in vec3 vertex_color;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;

void main()
{
   //color = vec4(vertex_color, 1.0f);
   color = texture(ourTexture, TexCoord);
}