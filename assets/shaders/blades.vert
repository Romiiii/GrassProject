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

//readonly layout(r8, binding=1) uniform image2D perlinNoise;
uniform sampler2D perlinNoise;

uniform float currentTime;
uniform float windStrength;
uniform float swayReach;
uniform float perlinSampleScale;
uniform vec2 windDirection;

float map2(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main()
{
//	if(gl_VertexID == 2) { // only sway the top vertex
//		vec4 actual_pos = model * instanceMatrix * vec4(pos, 1.0) * perlinSampleScale; // get the world coordinates of the vertices instead of the model coordinates
//		vec4 noise = texture(perlinNoise, vec2(actual_pos.x+currentTime,actual_pos.z+currentTime));
//		noise = noise - vec4(0.5,0.5,0.5,0.5); // map the noise from (0, 1) to (-0.5, 0.5)
//		gl_Position = projection * view * model * instanceMatrix * vec4(pos.x + noise.x , pos.y, pos.z + noise.z , 1.0);
//	} else {
//		gl_Position = projection * view * model * instanceMatrix * vec4(pos, 1.0);
//	}
//
//
	vec4 actual_pos;
	vec2 texture_pixel;
	if(gl_VertexID == 2) { // only sway the top vertex
		vec4 world_pos = model * instanceMatrix * vec4(pos, 1.0);
		actual_pos = world_pos; //* perlinSampleScale; // get the world coordinates of the vertices instead of the model coordinates


		texture_pixel = vec2(actual_pos.x + currentTime * windStrength, actual_pos.z + currentTime * windStrength);

		float noise = texture(perlinNoise, actual_pos.xz).r;
		noise = noise - 0.5f;

		//float noise = 0.0f;
		gl_Position = projection * view * model * instanceMatrix * vec4(pos.x + noise, pos.y, pos.z + noise, 1.0);
		//gl_Position = projection * view * model * instanceMatrix * vec4(pos.x, noise_y, pos.z, 1.0);
	} else {
		gl_Position = projection * view * model * instanceMatrix * vec4(pos, 1.0);
	}
	gl_Position = projection * view * instanceMatrix * vec4(pos, 1.0);	
	Normal = mat3(model * instanceMatrix) * normal;  
	FragPos = vec3(model * instanceMatrix * vec4(pos, 1.0));
	vtxColor = vec4((instanceMatrix * vec4(pos, 1.0f)).xz, 0.0f, 1.0f);
}

