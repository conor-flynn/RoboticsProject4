


uniform vec2 lightLocation;
uniform vec3 lightColor;
uniform float depthZed;

// Multiple 'light' formulas are used. Just look at the last one which overwrites all the others

void main() {
	float distance = length(lightLocation - gl_FragCoord.xy);
	float light = 0;

	if (distance == 0) distance = 1;
	light = 150.0f / pow(distance, 4.0f);
		

	vec4 color = vec4(light, light, light, pow(light, 3)) * vec4(lightColor, 1);

	gl_FragColor = color;
	gl_FragCoord.z = depthZed;
}


