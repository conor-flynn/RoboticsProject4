


uniform vec2 lightLocation;
uniform vec3 lightColor;

void main() {
	float distance = length(lightLocation - gl_FragCoord.xy);
	
	// Linear lighting. Very controlled, but very boring.  100 => small slight. 900 => large light.
	float light = 1 - (distance / 300.0); 
	//float light = 1 - (distance / 900.0);

	vec4 color = vec4(light, light, light, pow(light, 3)) * vec4(lightColor, 1);

	gl_FragColor = color;
}


