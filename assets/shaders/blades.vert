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

int map2(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main()
{
	vec4 world_pos = model * instanceMatrix * vec4(pos, 1.0);


	if(gl_VertexID == 2) { // only sway the top vertex
//		float sway = sin(pos.z + currentTime * windStrength) * swayReach;
//		world_pos =  vec4(windDirection.x, 0.0, windDirection.y, 0.0) * sway + world_pos;
//		gl_Position = projection * view * world_pos;
		vec4 actual_pos = model * instanceMatrix * vec4(pos, 1.0) * perlinSampleScale; // get the world coordinates of the vertices instead of the model coordinates
		//float noise = texture(perlinNoise, vec2(actual_pos.x+currentTime,actual_pos.z+currentTime)).x;
		//noise = 0.0f;
		//noise = noise;
		//float number = imageLoad(perlinNoise, ivec2(0,0)).x;
		vec2 texture_pixel = vec2(actual_pos.x+currentTime * windStrength, actual_pos.z+currentTime * windStrength);


		//float noise = imageLoad(perlinNoise, ivec2(int((actual_pos.x+currentTime)*512),int((actual_pos.z+currentTime)*512))).x;
		//float noise_z = imageLoad(perlinNoise, ivec2(int((actual_pos.x)),int((actual_pos.z)))).x;
		//float noise_y = imageLoad(perlinNoise, texture_pixel).x;
		float noise = texture(perlinNoise, texture_pixel).r;
		//float noise = imageLoad(perlinNoise, ivec2(50,50)).x;
		noise = noise - 0.5f;
		// vec4(currentTime,0.5,0.5,0.5); // map the noise from (0, 1) to (-0.5, 0.5)
		//noise = vec4(4.0,1.0,1.0,1.0);
		//noise = 0.0f;
		gl_Position = projection * view * model * instanceMatrix * vec4(pos.x + noise, pos.y, pos.z, 1.0);
		//gl_Position = projection * view * model * instanceMatrix * vec4(pos.x, noise_y, pos.z, 1.0);
	} else {
		gl_Position = projection * view * model * instanceMatrix * vec4(pos, 1.0);
	}
		
   Normal = mat3(model * instanceMatrix) * normal;  
   FragPos = vec3(model * instanceMatrix * vec4(pos, 1.0));
   vtxColor = color;
}

