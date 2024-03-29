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


// 0/1: Perlin/Checker: windX/windY are magnitude only.
// 2:	Fluid Grid:		windX/windY are velocities.
uniform int simulationMode;
uniform sampler2D windX;
uniform sampler2D windY;

uniform sampler2D oldWindX;
uniform sampler2D oldWindY;

uniform float currentTime;
uniform float windStrength;
uniform float swayReach;
uniform float velocityMultiplier;
uniform vec2 velocityClampRange;
uniform float worldMin;
uniform float worldMax;

uniform vec2 windDirection;

uniform bool debugBlades;

uniform float patchSize;

vec2 sample_velocity(vec2 texture_pixel);

float map2(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main()
{
	vtxColor = color;
	vec4 world_space_position = model * instanceMatrix * vec4(pos, 1.0);
	
	// Map the world space position to the texture coordinate
	// So that texture maps to all patches instead of one
	vec2 actual_pos = world_space_position.xz;
	actual_pos.x = map2(actual_pos.x, worldMin, worldMax, 0.0f, 1.0f);
	actual_pos.y = map2(actual_pos.y, worldMax, worldMin, 0.0f, 1.0f); // y axis is flipped

	

	if(simulationMode == 0 || simulationMode == 1) // PERLIN_NOISE, CHECKER_PATTERN,
	{
		// Get the world coordinates of the vertices instead of the model coordinates
		//vec4 uv_noise_texture = ((instanceMatrix * vec4(pos, 1.0f)) / patchSize) * textureScale;

		vec2 wind_direction = normalize(windDirection);
		vec2 texture_pixel = actual_pos + (currentTime * windStrength * wind_direction);
		vec2 noise;
		noise.r = texture(windX, texture_pixel).r;
		noise.g = texture(windY, texture_pixel).r;
		noise = (noise - 0.5f) * 2.0f;

		// Multiply by the y value of the uv which represents how the wind affects the specific vertex
		// Multiply by the y value twice to increase the effect of the wind 
		vec2 swag = swayReach * noise * pow(uvs.y, 2);
	
		vec4 wind_contribution = vec4(wind_direction.x * swag.x, 0.0f, wind_direction.y * swag.y, 0.0f);

		gl_Position = projection * view * (world_space_position + wind_contribution);
		
		if (debugBlades)
			vtxColor = vec4(0, 0, noise.r, 1.0f);
	}
	if(simulationMode == 2) // FLUID_GRID
	{
		// Get the world coordinates of the vertices instead of the model coordinates
		//vec4 uv_noise_texture = ((instanceMatrix * vec4(pos, 1.0f)) / patchSize);
		vec2 texture_pixel = actual_pos;
		
		vec2 velocity = sample_velocity(texture_pixel);

		velocity *= velocityMultiplier;
		velocity = clamp(velocity, vec2(0, 0), velocityClampRange); 
		// Multiply by the y value of the uv which represents how the wind affects the specific vertex
		// Multiply by the y value twice to increase the effect of the wind 
		vec2 swag = swayReach * velocity * pow(uvs.y, 2);
		vec4 wind_contribution = vec4(swag.x, 0, swag.y, 0);
		gl_Position = projection * view * (world_space_position + wind_contribution);

		if (debugBlades)
			vtxColor = vec4(velocity.rg, 0, 1.0f);
	}
	
		
	Normal = mat3(transpose(inverse(model))) * normal;  
	FragPos = world_space_position.xyz;
}

vec2 sample_velocity(vec2 texture_pixel)
{
	float stepsize = 0.01f;
	float base_x = texture_pixel.x - stepsize;
	float base_y = texture_pixel.y - stepsize;
	
	int root_samples = 3;
	float total_samples = root_samples * root_samples;

	vec2 velocity = vec2(0, 0);
	
	for(int y = 0; y < root_samples; y++)
	{
		for(int x = 0; x < root_samples; x++)
		{
			vec2 sample_pos = vec2(base_x + x * stepsize, base_y + y * stepsize);
			velocity.x += texture(windX, sample_pos).r;
			velocity.y += texture(windY, sample_pos).r;
		}
	}
	velocity /= total_samples;

	return velocity;
}