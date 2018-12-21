#version 400 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

out vec3 position;
out vec3 position_object;
out vec3 normal;
out vec2 texcoord;

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;

void main() {
	gl_Position = P * V * M * vec4(Position, 1.0);
	normal = mat3(transpose(inverse(M))) * Normal;
	//normal:https://learnopengl-cn.readthedocs.io/zh/latest/02%20Lighting/02%20Basic%20Lighting/
	texcoord = TexCoord;
	position = vec3(M * vec4(Position, 1.0));
	position_object = Position;
}