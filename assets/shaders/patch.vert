#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in mat4 instanceMatrix;
layout (location = 4) in vec2 uvs;

out vec4 vtxColor;
out vec3 Normal;
out vec3 FragPos;
out vec2 UV;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;



void main()
{
   gl_Position = projection * view * model * vec4(pos, 1.0);
   //gl_Position = projection * view * instanceMatrix * vec4(pos, 1.0);
   Normal = mat3(transpose(inverse(model))) * normal;  
   FragPos = vec3(model * vec4(pos, 1.0));
   vtxColor = color;
   UV = uvs;

//   	vec2 texture_pixel = vec2(actual_pos.x+currentTime * windStrength, actual_pos.z+currentTime * windStrength);
//
//
//	//float noise = imageLoad(perlinNoise, ivec2(int((actual_pos.x+currentTime)*512),int((actual_pos.z+currentTime)*512))).x;
//	//float noise_z = imageLoad(perlinNoise, ivec2(int((actual_pos.x)),int((actual_pos.z)))).x;
//	//float noise_y = imageLoad(perlinNoise, texture_pixel).x;
//	float noise = texture(perlinNoise, texture_pixel).r;
//	//float noise = imageLoad(perlinNoise, ivec2(50,50)).x;
//	noise = noise - 0.5f;
//	// vec4(currentTime,0.5,0.5,0.5); // map the noise from (0, 1) to (-0.5, 0.5)
//	//noise = vec4(4.0,1.0,1.0,1.0);
//	//noise = 0.0f;
//	gl_Position = projection * view * model * instanceMatrix * vec4(pos.x + noise, pos.y, pos.z, 1.0);
}

