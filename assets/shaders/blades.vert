#version 420 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uvs;
// instanceMatrix also uses locations 5, 6 & 7
layout (location = 4) in mat4 instanceMatrix;


out vec4 vtxColor;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform sampler2D windX;
uniform sampler2D windY;

uniform float textureScale;
uniform float currentTime;
uniform float windStrength;
uniform float swayReach;

uniform vec2 windDirection;

uniform bool debugBlades;

float map2(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main()
{
	vtxColor = color;

	// Get the world coordinates of the vertices instead of the model coordinates
	vec4 uv_noise_texture = vec4(pos, 1.0f) * textureScale;
	vec2 wind_direction = windDirection;
	vec2 texture_pixel = uv_noise_texture.xz + (currentTime * windStrength * wind_direction);
	vec2 noise;

	noise.r = texture(windX, texture_pixel).r;
	noise.g = texture(windY, texture_pixel).r;
	noise = (noise - 0.5f) * 2.0f;

	// Multiply by the y value of the uv which represents how the wind affects the specific vertex
	// Multiply by the y value twice to increase the effect of the wind 
	vec2 swag = swayReach * noise * pow(uvs.y, 2);
	
	vec4 world_space_position = model * instanceMatrix * vec4(pos, 1.0);
	vec4 wind_contribution = vec4(wind_direction.x * swag.x, 0.0f, wind_direction.y * swag.y, 0.0f);

	gl_Position = projection * view * (world_space_position + wind_contribution);

	if (debugBlades)
		vtxColor = vec4(0.0f, 0.0f, noise.r, 1.0f);
	
	Normal = mat3(transpose(inverse(model))) * normal;  
	FragPos = world_space_position.xyz;
}

