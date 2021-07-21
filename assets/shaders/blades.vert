#version 420 core
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

uniform sampler2D perlinNoise;

uniform float perlinSampleScale;
uniform float currentTime;
uniform float windStrength;
uniform float swayReach;

uniform vec2 windDirection;

float map2(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main()
{
	vtxColor = color;
	vec4 actual_pos;
	vec2 texture_pixel;
	if(gl_VertexID == 0) {
		vtxColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	if(gl_VertexID == 2) { // only sway the top vertex
		vec3 patch_pos = pos + vec3(5.0f, 0.0f, 5.0f);
		vec4 world_pos = model * instanceMatrix * vec4(patch_pos, 1.0);

		// Get the world coordinates of the vertices instead of the model coordinates
		actual_pos = (world_pos * perlinSampleScale);  
		actual_pos.x = map2(actual_pos.x, -5.0f, 5.0f, 0.0f, 1.0f);
		actual_pos.z = map2(actual_pos.z, -5.0f, 5.0f, 0.0f, 1.0f);

		texture_pixel = vec2(actual_pos.x, actual_pos.z) + (currentTime * windStrength * windDirection);

		float noise = texture(perlinNoise, texture_pixel).r;
		noise = (noise - 0.5f) * 2.0f;
		float swag = swayReach * noise;

		vec4 world_space_position = model * instanceMatrix * vec4(pos.xyz, 1.0);

		gl_Position = projection * view * (world_space_position + vec4(windDirection.x, 0.0f, windDirection.y, 0.0f) * swag);
	} else {
		gl_Position = projection * view * model * instanceMatrix * vec4(pos, 1.0);
	}	
	Normal = mat3(model * instanceMatrix) * normal;  
	FragPos = vec3(model * instanceMatrix * vec4(pos, 1.0));
}

