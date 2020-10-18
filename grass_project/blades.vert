#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in mat4 instanceMatrix;

out vec4 vtxColor;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	// gl_Position = projection * view * model * vec4(pos, 1.0);
   gl_Position = projection * view * model * instanceMatrix * vec4(pos, 1.0);
   Normal = mat3(transpose(inverse(model))) * normal;  
   FragPos = vec3(model * vec4(pos, 1.0));
   vtxColor = color;
}

