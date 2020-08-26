#version 330 core

#define PI 3.1415926538

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 normal;

out vec4 vtxColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float currentTime;
uniform sampler2D billboardNoise;
uniform int windType;
// 0 = simple trigonometric sway
// 1 = complex trigonometric sway 1
// 2 = complex trigonometric sway 2
// 3 = perlin sway 
uniform float windStrength;
uniform float swayReach;
uniform float perlinSampleScale;

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec3 right = vec3(view[0][0], view[1][0], view[2][0]),
		up = vec3(view[0][1], view[1][1], view[2][1]);

	vec4 world_pos = model * vec4(pos, 1.0);

	vec3 test_pos = world_pos.xyz + (right * world_pos.x);

	float randomNumber = rand(world_pos.xy);

	if(gl_VertexID == 0 || gl_VertexID == 3) { // only sway the top vertices
		float time = currentTime*windStrength;
		if (windType == 0) {
			float z = sin(pos.z + 10 * randomNumber + time) * swayReach;
			gl_Position = projection * view * model * vec4(pos.x, pos.y, z, 1.0);
		} else if (windType == 1) {
			float z = sin(pos.z + 10 * randomNumber);
			float t = 0.01*(-time*130.0);
			z += sin(pos.z*2.1 + time)*4.5;
			z += sin(pos.z*1.72 + time*1.121)*4.0;
			z += sin(pos.z*2.221 + time*0.437)*5.0;
			z += sin(pos.z*3.1122+ time*4.269)*2.5;
			z *= swayReach*0.06;
			//gl_Position = model *  instanceMatrix * vec4(pos.x, pos.y, z, 1.0);
			gl_Position = projection * view * model * vec4(pos.x, pos.y, z, 1.0);
		} else if (windType == 2) {
			float z = (cos(pos.z*PI+time+ 10 * randomNumber)*cos(pos.z*PI+time))*cos(pos.z*3*PI+time+ 10 * randomNumber)*cos(pos.z*5*PI+time+ 10 * randomNumber)*0.5+sin(pos.z*25*PI+time+ 10 * randomNumber)*0.02;
			z = z * swayReach;
			gl_Position = projection * view * model * vec4(pos.x, pos.y, z, 1.0);
		} else if (windType == 3) {
			vec4 actual_pos = model * vec4(pos, 1.0) * perlinSampleScale; // get the world coordinates of the vertices instead of the model coordinates
			vec4 noise = texture(billboardNoise, vec2(actual_pos.x+time,actual_pos.z+time));
			noise = noise - vec4(0.5,0.5,0.5,0.5); // map the noise from (0, 1) to (-0.5, 0.5)
			gl_Position = projection * view * model * vec4(pos.x + noise.x , pos.y, pos.z + noise.z , 1.0);
		} else {
			gl_Position = projection * view * model * vec4(pos, 1.0);
		}
		
	} else {
		gl_Position = projection * view * model * vec4(pos, 1.0);
	}

	vtxColor = color;
	TexCoord = uv;
	Normal = mat3(transpose(inverse(model))) * normal;  
	FragPos = vec3(model * vec4(pos, 1.0));
	
}