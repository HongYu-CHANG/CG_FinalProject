#version 330

uniform sampler2D Tex;
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
		//法向量和light向量不小於0 (有效的光源)
		I += diffuse_L.rgb * clamp(dot1, 0, 1);
	}

	I = I * vec3(tex);

	vec3 R = normalize(-L + 2 * dot(L, bump_normal) * bump_normal);
	float dot2 = dot(R, V);
	if (dot2 > 0) {
		I += specular_L.rgb * pow(dot2, Shiness);
	}
	
	outColor = vec4(I, 1);
}

/*
Phong model = c1*ambient + c2 * diffuse + c3 * specular
ambient:全域基本亮度
diffuse:粗糙面亮度
specular:光滑面亮度
*/