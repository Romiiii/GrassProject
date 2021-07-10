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

uniform float perlinSampleScale;
uniform float currentTime;
uniform float windStrength;
uniform float swayReach;

uniform vec2 windDirection;

float map2(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
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
		vec3 patch_pos = pos + vec3(5.0f, 0.0f, 5.0f);
		vec4 world_pos = model * instanceMatrix * vec4(patch_pos, 1.0);

		actual_pos = (world_pos * perlinSampleScale);// - vec4(5.0f, 0.0f, 5.0f, 0.0f); // get the world coordinates of the vertices instead of the model coordinates
		//actual_pos = world_pos * perlinSampleScale; // get the world coordinates of the vertices instead of the model coordinates
		actual_pos.x = map2(actual_pos.x, -5.0f, 5.0f, 0.0f, 1.0f);
		actual_pos.z = map2(actual_pos.z, -5.0f, 5.0f, 0.0f, 1.0f);


		// texture_pixel = vec2(actual_pos.x + currentTime * windStrength, actual_pos.z + currentTime * windStrength);
		//texture_pixel = vec2(actual_pos.x - 5.0f, actual_pos.z - 5.0f + currentTime * windStrength);
		texture_pixel = vec2(actual_pos.x, actual_pos.z + currentTime * windStrength);

		//float noise = texture(perlinNoise, actual_pos.xz).r;
		float noise = texture(perlinNoise, texture_pixel).r;
		noise = (noise - 0.5f) * 2.0f;
		float swag = swayReach * noise;

		//float noise = 0.0f;
		//gl_Position = projection * view * model * instanceMatrix * vec4(pos.x, pos.y, pos.z + noise, 1.0);
		gl_Position = projection * view * model * instanceMatrix * vec4(pos.x + swag, pos.y, pos.z + swag, 1.0);
		//gl_Position = projection * view * model * instanceMatrix * vec4(pos.x, noise_y, pos.z, 1.0);
		//gl_Position.y = perlinSampleScale;
		// SINUS WAVE WIND
//		vec4 world_pos = model * instanceMatrix * vec4(pos, 1.0);
//		float randomNumber = rand(world_pos.xy);
//		float z = sin(pos.z + 10 * randomNumber + currentTime) * swayReach;
//		gl_Position = projection * view * model * instanceMatrix * vec4(pos.x, pos.y, z, 1.0);
	} else {
		gl_Position = projection * view * model * instanceMatrix * vec4(pos, 1.0);
		//gl_Position = gl_Position.zyxw;
	}
	//gl_Position = projection * view * instanceMatrix * vec4(pos, 1.0);	
	Normal = mat3(model * instanceMatrix) * normal;  
	FragPos = vec3(model * instanceMatrix * vec4(pos, 1.0));
	
	//vtxColor = vec4((instanceMatrix * vec4(pos, 1.0f)).xz, 0.0f, 1.0f);

	

	vtxColor = color;
}

