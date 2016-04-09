


uniform vec2 lightLocation;
uniform vec3 lightColor;

void main() {
	float distance = length(lightLocation - gl_FragCoord.xy);
	float light = 0;

// Linear lighting. -----------------------
	// Very controlled, but very boring.  100 => small slight. 900 => large light.
	light = 1 - (distance / 300.0); 
	//light = 1 - (distance / 900.0);

// Experimental lighting --------------------------
	//1
		//light = 100.0f / pow(distance,1.5);
	//2
		/*
		light = 0;
		if (distance < 600) light = .01f;
		if (distance < 500) light = .02f;
		if (distance < 400) light = .05f;
		if (distance < 300) light = .1f;
		if (distance < 200) light = .3f;
		if (distance < 100) light = .5f;
		if (distance <  10) light = .7f;
		if (distance <   5) light = .99f;
		*/

	vec4 color = vec4(light, light, light, pow(light, 3)) * vec4(lightColor, 1);

	gl_FragColor = color;
}


