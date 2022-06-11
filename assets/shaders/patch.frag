#version 330 core

out vec4 FragColor;

in vec4 VtxColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 Uv;

uniform float ambientStrength; 
uniform vec3 lightPos; 
uniform float lightIntensity;
uniform vec4 lightColor;
uniform float windStrength;
uniform float currentTime;
uniform float textureScale;
uniform bool visualizeTexture;
uniform vec2 windDirection;
//uniform int simulationMode;

//uniform sampler2D perlinNoise;
//uniform sampler2D fluidGridDensity;
//uniform sampler2D fluidGridVelX;
//uniform sampler2D fluidGridVelY;

uniform sampler2D windX;
uniform sampler2D windY;


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
    vec3 ambient = vec3(ambientStrength, ambientStrength, ambientStrength);
	diffuse *= attenuation * lightIntensity;

		//vec2 uv = (UV * textureScale) + vtxPos.xz;
	//uv += currentTime * windStrength * windDirection.yx;
		
	vec2 actual_pos = Uv * textureScale;
	//actual_pos.x = map2(actual_pos.x, -5.0f, 5.0f, 0.0f, 1.0f);
	//actual_pos.z = map2(actual_pos.z, -5.0f, 5.0f, 0.0f, 1.0f);
		
	vec2 texture_pixel = actual_pos + (currentTime * windStrength * windDirection);
	vec4 patchColor;
	if (visualizeTexture) {	
		patchColor.r = abs(texture(windX, texture_pixel).r);
		patchColor.b = abs(texture(windY, texture_pixel).r);

		FragColor = patchColor;
		
		return;	
	}

	patchColor = VtxColor;
	
	vec4 result = vec4(ambient,1.0f) + vec4(diffuse,1.0f) * patchColor;
	FragColor = result;
	
}