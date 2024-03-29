#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;
out vec4 VtxColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
   gl_Position = projection * view * model * vec4(pos, 1.0);
   Normal = mat3(transpose(inverse(model))) * normal;  
   FragPos = vec3(model * vec4(pos, 1.0));
   VtxColor = color;
}

