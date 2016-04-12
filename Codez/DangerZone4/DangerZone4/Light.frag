

uniform vec2 lightLocation;
uniform vec3 lightColor;


void main() {
	float distance = length(lightLocation - gl_FragCoord.xy);
	float light = 0;

	if (distance == 0) distance = 1;
	light = 150.0f / pow(distance, 4.0f);

	vec4 color = vec4(light, light, light, pow(light, 3)) * vec4(lightColor, 1);

	// Fragment == Pixel   :: Sets the color of the pixel
	gl_FragColor = color;
}


