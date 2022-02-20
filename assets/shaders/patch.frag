#version 330 core

out vec4 FragColor;

in  vec4 vtxColor;
in vec3 Normal;
in vec3 FragPos;
in vec3 vtxPos;

uniform float ambientStrength; 
uniform vec3 lightPos; 
uniform float lightIntensity;
uniform vec4 lightColor;
uniform float windStrength;
uniform float currentTime;
uniform float textureScale;
uniform bool visualizeTexture;
uniform vec2 windDirection;

uniform sampler2D perlinNoise;
uniform sampler2D fluidGridDensity;
uniform sampler2D fluidGridVelX;
uniform sampler2D fluidGridVelY;


float map2(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main()
{
	float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

	float distance = length(lightPos - FragPos);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor.xyz;
    vec3 ambient = ambientStrength * lightColor.xyz;

	vec4 objectColor = vtxColor;


		//vec2 uv = (UV * textureScale) + vtxPos.xz;
	//uv += currentTime * windStrength * windDirection.yx;
		
	vec4 actual_pos = vec4(vtxPos, 1.0f) * textureScale;
	actual_pos.x = map2(actual_pos.x, -5.0f, 5.0f, 0.0f, 1.0f);
	actual_pos.z = map2(actual_pos.z, -5.0f, 5.0f, 0.0f, 1.0f);
		
	vec2 texture_pixel = actual_pos.xz + (currentTime * windStrength * windDirection);
	vec4 textureColor;
	if (visualizeTexture) {	// TODO:: make this a mode, int 0 none int 1 perlin int 2 fluidgrid
		textureColor = texture(perlinNoise, texture_pixel);
	} else {
		textureColor = texture(fluidGridDensity, texture_pixel);
	}
	objectColor = textureColor; 
	

	diffuse *= attenuation * lightIntensity;

	
	vec4 result = (vec4(ambient,1.0f) + vec4(diffuse,1.0f)) * objectColor;
	FragColor = result; 


  
	if(FragColor.a < 0.1) discard;
}