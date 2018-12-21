#version 400 core

uniform sampler2D honeyTex;
uniform sampler2D depthTex;
uniform vec3 eye;
uniform float time;

in vec3 position;
in vec3 position_object;
in vec3 normal;
in vec2 texcoord;

//Bonus
uniform float mycolor;
uniform float frequency;

out vec4 FragColor;

//intersection highlight
//ref:you need to linearize depth to [0, 1] first.
//ref: http://www.ozone3d.net/blogs/lab/20090206/how-to-linearize-the-depth-value/
float linearize(float raw) 
{
	float f = 1000;                               // camera far plane
	float n = 0.1f;                               // camera near plane
	return (2.0 * n) / (f + n - raw * (f - n));   // convert to linear values   
	//大部分公式還要再乘上f，但是此處乘上f會變的很不明顯
}


//intersection highligh:
//ref: Object Intersects means depth intersects. If Object is occluded,  depth value is larger than the value from depth texture

float checkIntersect(float rawLinear) {
	float currentDepth = linearize(gl_FragCoord.z);
	if(rawLinear > currentDepth) 
	{
		if ( (rawLinear - currentDepth) < 0.01) 
		{
			return 1 - 100 * (rawLinear - currentDepth);
			// return 0.9;
		}
		else
		{
			return 0;
		}
	} 
	else 
	{
		return 0;
	}
}


void drawBarrier(vec4 _Color)
{
	// vec4 _Color = vec4(16, 59, 159, 4) / 255.0;
	// vec4 _Color = vec4(16, 59, 159, 200) / 255.0;

	// pulsing
		// create tiling texture
		vec2 uv = vec2(texcoord.x * 5, texcoord.y * 9);
		vec4 mainTex = texture2D(honeyTex, uv);
			//ref:multiplier is affected by fragment’s position in object space and system time
			//ref: https://gist.github.com/thallippoli/10549185
			// float frequency = 6;
		float multiplier = 0.5f * sin( position_object.y * 10 + frequency * time ) + 0.5f;
		vec4 pulsedTexColor  = multiplier * mainTex * _Color;

	// pole
			//ref:Use object space position Y to determine the pole.
			//ref:The further a fragment is from the north pole (object space (0, 1, 0))
				//the smaller this value is:
				//float northPole = pole_value(objectPosition.y);
			//ref:Please notice that the upper and lower bound of this model in object space
			//ref:Is 1 and -1 respectively, and the “northPole” value should be between 0 and 1
		float Pole = (position_object.y + 1.0)/2;
		// vec4 pole = vec4(Pole , Pole , Pole , Pole);	


	//rim
			//ref:use view vector to check is it silhouette or not
			//ref:please notice that both position need to be under the same space whether it’s object or world
				// vec3 viewDir = camera_position - position;
				// float r = func(normal, viewDir);
				// vec4 rim = vec4(r, r, r, 1);
			//ref: http://roxlu.com/2014/037/opengl-rim-shader (OpenGL Rim Shader)
			//ref: https://gist.github.com/patriciogonzalezvivo/3a81453a24a542aabc63 (加了abs之後就不會有那麼多奇怪的橢圓)
				// float cosTheta = abs( dot( normalize(vEye), N) );
				// float fresnel = pow(1.0 - cosTheta, 4.0);
		vec3 viewDir = normalize(eye - position);
		float r = 1 - abs(dot(normalize(normal), viewDir));
		// float r = 1 - dot(normalize(normal), viewDir);
		// vec4 rim = vec4(r, r, r, 1);


	//intersection highlight
			//ref:you need to linearize depth to [0, 1] first.
				//float raw = texture2D(depthTexture, some_coordinate);
				//float isIntersect = checkIntersect(linearize(raw));
				//vec4 intersect = vec4(isIntersect, isIntersect, isIntersect, 1);
			//ref: https://stackoverflow.com/questions/23362076/opengl-how-to-access-depth-buffer-values-or-gl-fragcoord-z-vs-rendering-d
			//ref: https://github.com/KaimaChen/Unity-Shader-Demo/blob/master/UnityShaderProject/Assets/Depth/Shaders/ForceField.shader
			//ref: https://stackoverflow.com/questions/12204128/opengl-depth-buffer-to-texture-for-various-image-sizes
		float windowSize = 512;
		// depth
		float raw = texture2D(depthTex, gl_FragCoord.xy / windowSize).x;
		if (gl_FragCoord.z > raw) discard;
		float rawLinear = linearize(raw);
		float isIntersect = checkIntersect(rawLinear);
		// vec4 intersect = vec4(isIntersect , isIntersect , isIntersect , 1);	

	//summarize all effect
			//ref:the last page
				//float glow = max(max(Intersect, Rim), Pole);
				//vec4 glowColor = interpolate (_Color) to (1,1,1) with respect to glow
				//outColor = pulsedTexColor + (northPole + rim + intersect) * glowColor;
			//ref: https://github.com/KaimaChen/Unity-Shader-Demo/blob/master/UnityShaderProject/Assets/Depth/Shaders/ForceField.shader
		float glow = max(max(isIntersect, r), Pole*1.5);
		vec4 glowColor = mix(_Color, vec4(1), glow);
		vec4 outColor = pulsedTexColor + glowColor;
		// vec4 outColor = intersect;

	FragColor = mainTex;
	//FragColor = vec4(16, 59, 159, 200) / 255.0;

	// FragColor = outColor;
	// FragColor = vec4(normal, 1);
	// FragColor = 2*pulsedTexColor +vec4(0,0,0,0.5f);
	// FragColor = rim;
	// FragColor = pole;
	// FragColor = intersect;
	// FragColor = vec4(16, 59, 159, 4) / 255.0; //alpha值太小所以會看不太到(太透明)
	//FragColor = texture(honeyTex, texcoord);
	//FragColor = texture(depthTex, texcoord);
}

void main() 
{
	// Bonus
	if (mycolor == 0)
	{	
		vec4 _Color = vec4(16, 59, 159, 4)/ 255.0;//blue
		drawBarrier(_Color);
	}
	else if (mycolor == 1) 
	{
		vec4 _Color = vec4(216, 59, 159, 4)/ 255.0;//pink
		drawBarrier(_Color);
	}
	else 
	{
		vec4 _Color = vec4(16, 209, 159, 4)/ 255.0;//green
		drawBarrier(_Color);
	}


}