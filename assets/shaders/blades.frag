#version 330 core

out vec4 FragColor;

in vec4 vtxColor;
in vec3 Normal;
in vec3 FragPos;

uniform float ambientStrength; 
uniform vec3 lightPos; 
uniform float lightIntensity;
uniform vec4 lightColor;
uniform bool debugBlades;


void main()
{
    vec3 ambient = vec3(ambientStrength, ambientStrength, ambientStrength);
    

	float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

	float distance = length(lightPos - FragPos);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor.xyz;

	diffuse *= attenuation * lightIntensity;
	
	vec4 objectColor = vtxColor;
	
	vec4 result = vec4(ambient, 1.0f) * objectColor + vec4(diffuse, 1.0f) * objectColor;
	
	FragColor = result; 

	if (debugBlades)
		FragColor = objectColor;
		
}