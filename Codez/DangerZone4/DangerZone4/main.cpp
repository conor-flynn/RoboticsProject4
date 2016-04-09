
#include <stdio.h>
#include <windows.h> 
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include "Vector2.h"
#include "Creature.h"
#include "Light.h"

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int num_creatures = 20;
const int num_lights = 15;

vector<Light*> lights;
vector<Creature*> creatures;

int fragmentShader;
int shaderProgram;



float random() {
	return ((float)rand() / RAND_MAX);
}

float readPixel(int x, int y) {

	unsigned char pixel[3];
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	// Implicitly clamped between 0 and 1, maybe?
	float r = (float)pixel[0] / 255.0f;
	float g = (float)pixel[1] / 255.0f;
	float b = (float)pixel[2] / 255.0f;

	return sqrt((r*r) + (g*g) + (b*b));

	/*cout << "R: " << (float)pixel[0] / 255.0 << endl;
	cout << "G: " << (float)pixel[1] / 255.0 << endl;
	cout << "B: " << (float)pixel[2] / 255.0 << endl;
	cout << endl;*/
}

float readPixel(Vector2 v) {
	return readPixel(v.getX(), v.getY());
}

void drawLights() {
	for (Light* light : lights) {

		/*
		// General idea: 
			// Calculate shadows and draw them to the stencil buffer. 
			// For the pixels that are within these shadows, set the stencil value to 1
				// When we draw the lights, only color a pixel if the stencil value != 1 
				// (Only drawing the lit portion when it isn't in a shadow)

		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glColorMask(false, false, false, false);

		for (Creature* creature : creatures) {
			vector<Vector2> vertices = creature->getVertices();
			for (int i = 0; i < vertices.size(); i++) {
				Vector2 currentVertex = vertices[i];
				Vector2 nextVertex = vertices[(i + 1) % vertices.size()];
				Vector2 edge = Vector2::subtract(nextVertex, currentVertex);
				Vector2 normal = Vector2(edge.getY(), -edge.getX());
				Vector2 lightToCurrent = Vector2::subtract(currentVertex, light->location);


				if (Vector2::dot(normal, lightToCurrent) > 0) {
					Vector2 point1 = Vector2::add(currentVertex, Vector2::subtract(currentVertex, light->location).scale(800));
					Vector2 point2 = Vector2::add(nextVertex, Vector2::subtract(nextVertex, light->location).scale(800));
					glBegin(GL_QUADS); {
					glVertex2f(currentVertex.getX(), currentVertex.getY());
					glVertex2f(point1.getX(), point1.getY());
					glVertex2f(point2.getX(), point2.getY());
					glVertex2f(nextVertex.getX(), nextVertex.getY());
					} glEnd();
				}
			}
		}
		*/

		// TODO : try a GL_NEVER stencil funtion
		glStencilFunc(GL_EQUAL, 1, 1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glColorMask(true, true, true, true);

		glUseProgram(shaderProgram);
		glUniform2f(glGetUniformLocation(shaderProgram, "lightLocation"), light->location.getX(), SCREEN_HEIGHT - light->location.getY());
		glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), light->red, light->green, light->blue);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		// Applies the shader for the current light to the ENTIRE screen. The shadowed areas are calculated beforehand
		glBegin(GL_QUADS); {
			glVertex2f(0, 0);
			glVertex2f(0, SCREEN_HEIGHT);
			glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
			glVertex2f(SCREEN_WIDTH, 0);
		} glEnd();

		glDisable(GL_BLEND);
		glUseProgram(0);
		glClear(GL_STENCIL_BUFFER_BIT);
	}
}

void drawCreatures() {
	
	// We aren't 'blending' the creatures to the light, we just draw the creature
	glDisable(GL_BLEND);

	// We aren't using a particular shader, just the default.
	glUseProgram(0);

	for (Creature* creature : creatures) {

		vector<Vector2> sensors = creature->getSensors();
		creature->processSensors(readPixel(sensors[0]), readPixel(sensors[1]));

		Vector2 location = creature->location;
		if (location.getX() < 0) location.setX(SCREEN_WIDTH);
		if (location.getX() > SCREEN_WIDTH) location.setX(0);
		if (location.getY() < 0) location.setY(SCREEN_HEIGHT);
		if (location.getY() > SCREEN_HEIGHT) location.setY(0);
		creature->location = location;

		glBegin(GL_LINE_LOOP); {
			for (Vector2 vertex : creature->getVertices()) {

				if (random() > 0.5f) {
					glColor3f(0, 0, 0);
				}
				else {
					glColor3f(1, 1, 1);
				}
				glColor3f(0, 0, 0);
				glVertex2f(vertex.getX(), SCREEN_HEIGHT - vertex.getY());
			}
		} glEnd();
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	// =~ =~ =~ =~ =~ =~ =~ =~ =~ =~ =~ 
	drawLights();
	drawCreatures();
	// =~ =~ =~ =~ =~ =~ =~ =~ =~ =~ =~ 
	glutSwapBuffers();
	glutPostRedisplay();
}

char* readFile(char* file_name, unsigned long & size) {
	FILE* sourceFile = fopen(file_name, "r");
	fseek(sourceFile, 0, SEEK_END);
	size = ftell(sourceFile);
	rewind(sourceFile);

	char* data = (char *)malloc(size + 1);
	size = fread(data, sizeof(char), size, sourceFile);
	data[size] = 0;
	return data;
}

void testingInitialization() {

	unsigned long size_of_shader_file = 0;
	char* shader_data = readFile("Light.frag", size_of_shader_file);

	// Declare the shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Define the shader
	glShaderSource(fragmentShader, 1, (const char **)&shader_data, NULL);

	// Compile the shader
	glCompileShader(fragmentShader);

	// Create the program (could attach multiple shaders to one program)
	shaderProgram = glCreateProgram();

	// Attach the fragment shader
	glAttachShader(shaderProgram, fragmentShader);

	// Link the shader
	glLinkProgram(shaderProgram);

	// Did the shader properly link to program?
	GLint linked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);

	/*if (linked) {
		printf("Linked!\n");	// Everything is working
	}
	else {
		printf("Not linked!\n");	// Idfk
	}*/

	glValidateProgram(shaderProgram);

	struct stat info;
	char* pathname = "Debug";
	if ( stat(pathname, &info) == 0 && (info.st_mode & S_IFDIR) ) {
		cout << "Overwritting Light.frag" << endl;

		FILE* debug_shader = fopen("../Debug/Light.frag", "w");
		fwrite(shader_data, sizeof(char), size_of_shader_file, debug_shader);
		fclose(debug_shader);
	}
	free(shader_data);
	

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_STENCIL_TEST);
	glClearColor(0, 0, 0, 0);
}

void buildLights() {
	////lights.push_back(new Light(Vector2(100, 100), .7f, 0, 0));
	//lights.push_back(new Light(Vector2(500, 100), 0, .7f, 0));
	//lights.push_back(new Light(Vector2(100, 500), .7f, 0, 0));
	////lights.push_back(new Light(Vector2(500, 500), 0, .7f, 0));

	for (int i = 0; i < num_lights; i++) {
		Light* light = new Light(Vector2(random()*SCREEN_WIDTH, random()*SCREEN_HEIGHT), random(), random(), random());
		//Light* light = new Light(Vector2(((float)rand() / RAND_MAX)*SCREEN_WIDTH,((float)rand() / RAND_MAX)*SCREEN_HEIGHT), (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
		lights.push_back(light);
	}
}

void buildCreatures() {

	for (int i = 0; i < num_creatures; i++) {
		float locx = (random() * SCREEN_WIDTH);
		float locy = (random() * SCREEN_HEIGHT);
		float k11 = random();
		float k12 = random();
		float k21 = random();
		float k22 = random();

		creatures.push_back(new Creature(Vector2(locx, locy), k11, k12, k21, k22));
	}
}

void inputTesting(unsigned char key, int x, int y) {
	Creature* creature = creatures[0];
	int step = 10;
	if (key == 'w') {
		creature->location.addY(step);
	}
	else if (key == 's') {
		creature->location.addY(-step);
	}
	else if (key == 'a') {
		creature->direction += step;
	}
	else if (key == 'd') {
		creature->direction -= step;
	}
}

int main(int argc, char** argv) {
	srand(static_cast <unsigned> (time(0)));
	glutInit(&argc, argv);

	buildLights();
	buildCreatures();

	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(800, 100);
	glutCreateWindow("Lighting, robot motion, normal robot drawing, no shadows");
	glutDisplayFunc(display);
	glutKeyboardFunc(inputTesting);

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();

	if (glewError == GLEW_OK) {
		printf(" 8^) \n");
		testingInitialization();
		glutMainLoop();
	}
	else {
		printf(" :( \n");
	}

	return 0;
}

