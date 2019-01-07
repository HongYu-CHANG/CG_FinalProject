#version 330

uniform sampler2D Tex;
uniform sampler2D rampTexture;
in vec3 position_v;
in vec3 normal_v;
in vec3 light_pos;
in vec3 eye_pos;
in vec2 texcoord;

in vec4 ambient_L;
in vec4 diffuse_L;
in vec4 specular_L;

in float Shiness;

out vec4 outColor;
//there should be a out vec4 in fragment shader defining the output color of fragment shader(variable name can be arbitrary)
void main() {

	vec3 red = vec3(1, 0, 0);
	vec3 L = normalize(light_pos - position_v);
	vec3 V = normalize(eye_pos - position_v);
	vec3 d = normalize(position_v - light_pos);
	vec4 tex = texture2D(Tex, texcoord);
	vec3 bump_normal = normal_v;
	
	vec3 I = vec3(0, 0, 0);
	I += ambient_L.xyz;
	
	float dot1 = dot(bump_normal, L);
	if (dot1 > 0) {
		I += diffuse_L.rgb * clamp(dot1, 0, 1);
	}

	I = I * vec3(tex);
	
	//gray scale
	if(I.r > 0.9f && I.r <= 1.0f){
		I.rgb = vec3(I.r*0.5f + 0.9f*0.5f, I.g*0.5f + 0.9f*0.5f, I.b*0.5f + 0.9f*0.5f);
	}
	else if(I.r > 0.75f && I.r <= 0.9f){
		I.rgb = vec3(I.r*0.5f + 0.75f*0.5f, I.g*0.5f + 0.75f*0.5f, I.b*0.5f + 0.75f*0.5f);
	}
	else if(I.r > 0.5f && I.r <= 0.75f){
		I.rgb = vec3(I.r*0.5f + 0.5f*0.5f, I.g*0.5f + 0.5f*0.5f, I.b*0.5f + 0.5f*0.5f);
	}
	else if(I.r > 0.25f && I.r <= 0.5f){
		I.rgb = vec3(I.r*0.5f + 0.25f*0.5f, I.g*0.5f + 0.25f*0.5f, I.b*0.5f + 0.25f*0.5f);
	}
	else if(I.r > 0.0f && I.r <= 0.25f){
		//I.rgb = vec3(I.r*0.5f + 0.0f*0.5f, I.g*0.5f + 0.0f*0.5f, I.b*0.5f + 0.0f*0.5f);
		//I.rgb = vec3(I.r*0.5f + 0.0f*0.5f, I.g*0.5f + 0.0f*0.5f, I.b*0.5f + 0.0f*0.5f);
	}
	
	outColor = vec4(I, 1) + vec4(0.047, 0.533, 0.071, 0);
}

/*
將光源照東西的亮度變成一階一階的
*/


