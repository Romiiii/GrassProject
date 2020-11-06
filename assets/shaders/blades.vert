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

uniform float currentTime;
uniform float windStrength;
uniform float swayReach;
uniform vec2 windDirection;

void main()
{
	vec4 world_pos = model * instanceMatrix * vec4(pos, 1.0);

	if(gl_VertexID == 2) { // only sway the top vertex
		float sway = sin(pos.z + currentTime * windStrength) * swayReach;
		world_pos =  vec4(windDirection.x, 0.0, windDirection.y, 0.0) * sway + world_pos;
		gl_Position = projection * view * world_pos;
	} else {
		gl_Position = projection * view * model * instanceMatrix * vec4(pos, 1.0);
	}

   Normal = mat3(model * instanceMatrix) * normal;  
   FragPos = vec3(model * instanceMatrix * vec4(pos, 1.0));
   vtxColor = color;
}

