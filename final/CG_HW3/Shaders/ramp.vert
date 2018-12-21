#version 330

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 Texcoord;

uniform mat4 MV;
uniform mat4 P;
uniform mat4 V;
uniform vec3 Light_position;
uniform vec3 eye;

uniform float Enablebump;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;

uniform float shiness;

out vec3 position_v;
out vec3 normal_v;
out vec3 light_pos;
out vec3 eye_pos;
out vec2 texcoord;

out vec4 ambient_L;
out vec4 diffuse_L;
out vec4 specular_L;

out float Shiness;

void main() {
	vec4 p = vec4(Position, 1.0);
	vec4 n = vec4(Normal, 0.0);
	vec4 l = vec4(Light_position, 1.0);

	mat4 M = inverse(V) * MV;
	p = M * p;
	n = M * n;

	gl_Position = P * V * p;

	eye_pos = eye;
	position_v = p.xyz;
	normal_v = normalize(n.xyz);
	light_pos = l.xyz;
	texcoord = Texcoord;

	ambient_L = ambient * 0.1;
	diffuse_L = diffuse * 2.5;
	specular_L = specular;
	
	Shiness = shiness;
}

