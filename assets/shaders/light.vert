#version 330 core
layout (location = 0) in vec3 vertex;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
   vec4 pos = projection * view * model * vec4(vertex, 1);
   gl_Position = pos;
}  
