#version 330 core

out vec4 FragColor;

in vec4 vtxColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D billboardTexture;
uniform sampler2D billboardNormal;
uniform vec3 lightPos; 
uniform float ambientStrength; 

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

	float mult = dot(norm, lightDir);
	// Correct light for billboards where the normals are facing away from the point light 
	if (mult < 0.0) {
		mult = -1* mult;
	}
	
	float diff = max(mult, 0.0);
	vec3 diffuse = diff * lightColor;
    vec3 ambient = ambientStrength * lightColor;

	vec4 objectColor = texture(billboardTexture, TexCoord);

	ambient *= attenuation;
	diffuse *= attenuation;
	vec4 result = (vec4(ambient,1.0f) + vec4(diffuse,1.0f)) * objectColor;
	FragColor = result;

	if(FragColor.a < 0.1) discard;
}