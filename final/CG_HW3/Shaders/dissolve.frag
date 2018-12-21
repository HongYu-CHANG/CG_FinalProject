#version 330

uniform sampler2D Tex;
//uniform sampler2D BumpTex;
in vec3 position_v;
in vec3 normal_v;
in vec3 light_pos;
in vec3 eye_pos;
in vec2 texcoord;

in vec4 ambient_L;
in vec4 diffuse_L;
in vec4 specular_L;


uniform vec4       ambientColor;
uniform vec4       diffuseColor;   
uniform vec4       specularColor;

uniform sampler2D  noiseTexture;           // 存放dissvole纹理
uniform float      dissolveFactor;         // 每帧都变化的阈值

out vec4 outColor;

void main() {

	vec4 vSample = texture(Tex, texcoord);
	vec4 vNoise = texture(noiseTexture, texcoord);
	
    if(vNoise.r < dissolveFactor){
		discard;         // 如果小于阈值，就抛弃该片断	
	}
	
	else if(vNoise.r < dissolveFactor + 0.05f){
		outColor.rgb = vec3(255.0, 255.0, 0.0);
	}
    
	outColor += vSample;
	
}

/*
dissolve factor當render與否的threshold，
改變dissolve factor，讓model隨著texture形狀慢慢消失，
用比dissolve factor邊界大一點點的值當畫黃色邊界的依據
*/
