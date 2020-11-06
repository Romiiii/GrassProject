#version 330 core

out vec4 FragColor;

in  vec4 vtxColor;
in vec3 Normal;
in vec3 FragPos;

uniform float ambientStrength; 
uniform vec3 lightPos; 


void main()
{
	float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

	float distance = length(lightPos - FragPos);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
    vec3 ambient = ambientStrength * lightColor;

	vec4 objectColor = vtxColor;

	//ambient *= attenuation;
	diffuse *= attenuation;

	vec4 result = (vec4(ambient,1.0f) + vec4(diffuse,1.0f)) * objectColor;
	FragColor = result; 
  
	if(FragColor.a < 0.1) discard;
}