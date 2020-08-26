#version 330 core
layout (location = 0) in vec3 vertex;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
   TexCoords = vertex;
   vec4 pos = projection * vec4(mat3(view) * vertex, 1.0);
   gl_Position = pos.xyww;
}  
