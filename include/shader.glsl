#version 330 core

layout(location = 0) in vec3 position; // The position variable has attribute position 0
layout(location = 1) in vec3 color;

out vec3 vertex_color;

uniform mat4 transform;

void main()
{
   gl_Position = transform * vec4(position, 1.0f); // See how we directly give a vec3 to vec4's constructor
   vertex_color = color; // Set the output variable to a dark-red color
}