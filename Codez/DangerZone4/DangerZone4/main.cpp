
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
const int num_creatures = 100;
const int num_lights = 500;

vector<Light*> lights;
vector<Creature*> creatures;

int fragmentShader;
int shaderProgram;



float random() {
	return ((float)rand() / RAND_MAX);
}

float readPixel(int x, int y) {

	y = SCREEN_HEIGHT - y;

	unsigned char pixel[3];
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	// Implicitly clamped between 0 and 1, maybe?
	float r = (float)pixel[0] / 255.0f;
	float g = (float)pixel[1] / 255.0f;
	float b = (float)pixel[2] / 255.0f;
	float result = sqrt((r*r) + (g*g) + (b*b));
	result = r + g + b;
	return result;

	/*cout << "R: " << (float)pixel[0] / 255.0 << endl;
	cout << "G: " << (float)pixel[1] / 255.0 << endl;
	cout << "B: " << (float)pixel[2] / 255.0 << endl;
	cout << endl;*/
}

float readPixel(Vector2 v) {
	return readPixel(v.getX(), v.getY());
}

void drawRect(int x0, int y0, int width, int height, float z = 0) {
	glBegin(GL_QUADS); {
		glVertex3f(x0, y0, z);
		glVertex3f(x0, y0 + height, z);
		glVertex3f(x0 + width, y0 + height, z);
		glVertex3f(x0 + width, y0, z);
	} glEnd();
}

void drawLights() {
	float ambience = 0.4f;
	glClearColor(ambience, ambience, ambience,1); // Ambient lighting
	for (Light* light : lights) {

		//// --- Clear / Setup process
		//	glUseProgram(0);

		//	glColorMask(1, 1, 1, 1);
		//	glDepthMask(GL_TRUE);
		//	glEnable(GL_DEPTH_TEST);

		//	glClearDepth(0);
		//	glClear(GL_DEPTH_BUFFER_BIT);	// We only clear the depth bit, not the colors. Otherwise that will override the other lights.

		//	// These four lines repeat a process because 'glClearDepth(0)' doesn't clear to 0....!
		//	glEnable(GL_DEPTH_TEST);
		//	glDepthFunc(GL_ALWAYS);
		//	glColorMask(0, 0, 0, 0);
		//	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		//// Setup for a light is complete.


		//for (Creature* creature : creatures) {
		//	vector<Vector2> vertices = creature->getVertices();
		//	for (int i = 0; i < vertices.size(); i++) {
		//		Vector2 currentVertex = vertices[i];
		//		Vector2 nextVertex = vertices[(i + 1) % vertices.size()];
		//		Vector2 edge = Vector2::subtract(nextVertex, currentVertex);
		//		Vector2 normal = Vector2(edge.getY(), -edge.getX());
		//		Vector2 lightToCurrent = Vector2::subtract(currentVertex, light->location);


		//		if (Vector2::dot(normal, lightToCurrent) > 0) {
		//			Vector2 point1 = Vector2::add(currentVertex, Vector2::subtract(currentVertex, light->location).scale(800));
		//			Vector2 point2 = Vector2::add(nextVertex, Vector2::subtract(nextVertex, light->location).scale(800));

		//			float zed = 1;
		//			glBegin(GL_QUADS); {
		//			glVertex3f(currentVertex.getX(), currentVertex.getY(), zed);
		//			glVertex3f(point1.getX(), point1.getY(), zed);
		//			glVertex3f(point2.getX(), point2.getY(), zed);
		//			glVertex3f(nextVertex.getX(), nextVertex.getY(), zed);
		//			} glEnd();

		//		}
		//	}
		//}
		

		// Shadows are drawn. Now we draw the dank lights

		glEnable(GL_DEPTH_TEST);
		// glDepthFunc(GL_EQUAL);	// Draws light (blending) when the depth of a pixel is equal to 0
		glDepthFunc(GL_ALWAYS);
		glColorMask(1, 1, 1, 1);

		glUseProgram(shaderProgram);
		glUniform2f(glGetUniformLocation(shaderProgram, "lightLocation"), light->location.getX(), SCREEN_HEIGHT - light->location.getY());
		glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), light->red, light->green, light->blue);
		glUniform1f(glGetUniformLocation(shaderProgram, "depthZed"), 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		// Applies the shader for the current light to the ENTIRE screen. The shadowed areas are calculated beforehand
		glBegin(GL_QUADS); {
			glVertex2f(0, 0);
			glVertex2f(0, SCREEN_HEIGHT);
			glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
			glVertex2f(SCREEN_WIDTH, 0);
		} glEnd();

	}
}

void tempSquare(int x, int y) {
	glBegin(GL_QUADS);
	float size = 5.0f;
	glVertex2f(x - size, y - size);
	glVertex2f(x - size, y + size);
	glVertex2f(x + size, y + size);
	glVertex2f(x + size, y - size);
	glEnd();
}

void drawCreatures() {
	
	// We aren't 'blending' the creatures to the light, we just draw the creature
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	// We aren't using a particular shader, just the default.
	glUseProgram(0);

	for (Creature* creature : creatures) {

		vector<Vector2> sensors = creature->getSensors();
		creature->processSensors(readPixel(sensors[0]), readPixel(sensors[1]));

		/*
		// Draws the sensors for debugging (and colors them by how much light they are sensing)
		for (Vector2 sensor : sensors) {
			float color = readPixel(sensor);
			glColor3f(color, 0, 0);
			tempSquare(sensor.getX(), sensor.getY());
		}*/

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
				glVertex2f(vertex.getX(), vertex.getY());
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

	glClearColor(0, 0, 0, 0);
}

void buildLights() {
	////lights.push_back(new Light(Vector2(100, 100), .7f, 0, 0));
	//lights.push_back(new Light(Vector2(500, 100), 0, .7f, 0));
	//lights.push_back(new Light(Vector2(100, 500), .7f, 0, 0));
	////lights.push_back(new Light(Vector2(500, 500), 0, .7f, 0));

	for (int i = 0; i < num_lights; i++) {
		Light* light = new Light(Vector2(random()*SCREEN_WIDTH, random()*SCREEN_HEIGHT), random(), random(), random());
		//Light* light = new Light(Vector2(random()*SCREEN_WIDTH, random()*SCREEN_HEIGHT), 1,1,1);
		

		lights.push_back(light);
	}
}

void buildCreatures() {

	for (int i = 0; i < num_creatures; i++) {
		float locx = (random() * SCREEN_WIDTH);
		float locy = (random() * SCREEN_HEIGHT);
		float direction = (random() * 360);
		float k11 = random();
		float k12 = random();
		float k21 = random();
		float k22 = random();

		k11 = k22 = 0;
		k12 = k21 = 1;

		creatures.push_back(new Creature(Vector2(locx, locy), direction, k11, k12, k21, k22));
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
	glutCreateWindow("Ayy");
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

