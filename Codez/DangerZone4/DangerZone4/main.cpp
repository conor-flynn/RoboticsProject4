/****************************************************************************

DANGERZONE
CSCE 452 Project 4
main.cpp

Structure for Robot Input File:

		"number_of_robots
		k11 k12 k21 k22 xloc yloc
		...
		k11 k12 k21 k22 xloc yloc"


Structure for Light Input File:

		"number_of_lights
		xloc yloc red green blue
		...
		xloc yloc red green blue"

*****************************************************************************/

#include <stdio.h>
#include <windows.h> 
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

#include <GL/glew.h>
#include <GL/glut.h>

#include "Vector2.h"
#include "Creature.h"
#include "Light.h"

using namespace std;


/****************************************************************************
				GLOBAL VARIABLES
*****************************************************************************/

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int num_creatures = 100;
const int num_lights = 500;
const float ambient_light = 0.4f;

vector<Light*> lights;
vector<Creature*> creatures;

int fragmentShader;
int shaderProgram;

string creatures_file = "none";
string lights_file = "none";


/****************************************************************************
				GENERAL FUNCTIONS
*****************************************************************************/

// randomly generates a float from 0 to 1
float random() {
	return ((float)rand() / RAND_MAX);
}

// gets name of creatures file
void get_creatures_file() {
	cout << "Robot file: ";
	cin >> creatures_file;
}

// gets name of lights file
void get_lights_file() {
	cout << "Light file: ";
	cin >> lights_file;
}


/****************************************************************************
					READ FUNCTIONS
*****************************************************************************/

float readPixel(int x, int y) {

	y = SCREEN_HEIGHT - y;

	unsigned char pixel[3];
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	// Implicitly clamped between 0 and 1, maybe?
	float r = (float)pixel[0] / 255.0f;
	float g = (float)pixel[1] / 255.0f;
	float b = (float)pixel[2] / 255.0f;

	// This has no impact on how the screen is drawn, it just is used to give the sensor-information to the robots
	// If a larger value is returned, then the robots react more strongly (their wheels spin faster).
	float result = sqrt((r*r) + (g*g) + (b*b));
	result = r + g + b;

	return result;
}

float readPixel(Vector2 v) {
	return readPixel(v.getX(), v.getY());
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


/****************************************************************************
				DRAW FUNCTIONS
*****************************************************************************/

void drawLights() {
	glClearColor(ambient_light, ambient_light, ambient_light, 1); // Ambient lighting
	for (Light* light : lights) {

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
		glColorMask(1, 1, 1, 1);

		glUseProgram(shaderProgram);	// Uses Light.frag (Light.fragment : processes pixels
		glUniform2f(glGetUniformLocation(shaderProgram, "lightLocation"), light->location.getX(), SCREEN_HEIGHT - light->location.getY());
		glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), light->red, light->green, light->blue);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		/*
		// 'Redraws' the whole screen. Only useful with VERY large lights
		// This quad is all the pixels that get passed to the shader
		// However, with small lights, there are a lot of pixels processed that could never possibly be impacted by small lights
		glBegin(GL_QUADS); {
		glVertex2f(0, 0);
		glVertex2f(0, SCREEN_HEIGHT);
		glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
		glVertex2f(SCREEN_WIDTH, 0);
		} 
		glEnd();*/

		Vector2 point = light->location;
		float max_light_distance = 300;
		// Only redraws part of the screen that could reasonably be impacted by the light
		// Since we are using VERY small lights, the light probably doesn't go further than 50 pixels in any direction, but its better to be safe
		// Testing: Needs to be larger than 10, otherwise it cuts off light
		glBegin(GL_QUADS); {
			glVertex2f(point.getX() - max_light_distance, point.getY() - max_light_distance);
			glVertex2f(point.getX() - max_light_distance, point.getY() + max_light_distance);
			glVertex2f(point.getX() + max_light_distance, point.getY() + max_light_distance);
			glVertex2f(point.getX() + max_light_distance, point.getY() - max_light_distance);
		} glEnd();
	}
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

		Vector2 location = creature->location;
		if (location.getX() < 0) location.setX(SCREEN_WIDTH);
		if (location.getX() > SCREEN_WIDTH) location.setX(0);
		if (location.getY() < 0) location.setY(SCREEN_HEIGHT);
		if (location.getY() > SCREEN_HEIGHT) location.setY(0);
		creature->location = location;

		// Draws the creature as just an outline
		glBegin(GL_LINE_LOOP); {
			for (Vector2 vertex : creature->getVertices()) {
				glColor3f(0, 0, 0);
				glVertex2f(vertex.getX(), vertex.getY());
			}
		} glEnd();
	}
}


/****************************************************************************
				SETUP AND DISPLAY
*****************************************************************************/

void setupGlut() {
	// Sets up the shader, and sets up OpenGl for the top-down 2D view
	unsigned long size_of_shader_file = 0;
	char* shader_data = readFile("Light.frag", size_of_shader_file);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);					// Declare the shader
	glShaderSource(fragmentShader, 1, (const char **)&shader_data, NULL);	// Define the shader
	glCompileShader(fragmentShader);										// Compile the shader
	shaderProgram = glCreateProgram();					// Create the program (could attach multiple shaders to one program)
	glAttachShader(shaderProgram, fragmentShader);		// Attach the fragment shader
	glLinkProgram(shaderProgram);						// Link the shader

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
	if (stat(pathname, &info) == 0 && (info.st_mode & S_IFDIR)) {
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

void display() {
	// Main display function for the window
	glClear(GL_COLOR_BUFFER_BIT);
	// =~ =~ =~ =~ =~ =~ =~ =~ =~ =~ =~ 
	drawLights();
	drawCreatures();
	// =~ =~ =~ =~ =~ =~ =~ =~ =~ =~ =~ 
	glutSwapBuffers();
	glutPostRedisplay();
}


/****************************************************************************
				BUILD FUNCTIONS
*****************************************************************************/

// generates a list of random lights
void buildLights() {
	for (int i = 0; i < num_lights; i++) {
		// Random position, random color
		Light* light = new Light(Vector2(random()*SCREEN_WIDTH, random()*SCREEN_HEIGHT), random(), random(), random());
		lights.push_back(light);
	}
}

// generates a list of random creatures
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

// builds vector of creatures from file
void build_creatures_from_file(string cf) {

	ifstream infile(cf);
	int numCreatures = 0, k11, k12, k21, k22, xloc, yloc, dir;

	infile >> numCreatures;

	for (int i = 0; i < numCreatures; ++i) {
		infile >> k11 >> k12 >> k21 >> k22 >> xloc >> yloc >> dir;
		//float direction = (random() * 360);
		creatures.push_back(new Creature(Vector2(xloc, yloc), dir, k11, k12, k21, k22));
	}
}

// builds vecotr of lights from file
void build_lights_from_file(string cf) {

	ifstream infile(cf);
	int numLights = 0, r, g, b, x, y;
	infile >> numLights;

	for (int i = 0; i < numLights; ++i) {
		infile >> x >> y >> r >> g >> b;
		float direction = (random() * 360);
		Light* light = new Light(Vector2(x, y), r, g, b);
		lights.push_back(light);
	}
}


/****************************************************************************
		USER INTERACTION FUNCTIONS
*****************************************************************************/

// display menu for options
void displayMenu() {
	cout << endl << endl
		<< "---------------------------------------------" << endl
		<< "                   Options                   " << endl
		<< "---------------------------------------------" << endl
		<< "1) Add Robot File (" << creatures_file << ") " << endl
		<< "2) Add Light File (" << lights_file << ")    " << endl
		<< "3) Start Program                             " << endl << endl;
}

// used to check for bad input, only accepts numbers
int getInput() {
	string input;
	stringstream ss;
	int choice;

	cin >> input;
	ss << input;
	ss >> choice;

	if (!ss.fail())
		return choice;
	else return 0;
}

// main menu loop
void menu() {

	int choice;
	bool start = false;

	do {
		displayMenu();
		choice = getInput();

		switch (choice) {
			case 1:
				get_creatures_file();
				break;
			case 2:
				get_lights_file();
				break;
			case 3:
				start = true;
				break;
			default:
				cout << "ERROR: Invalid choice!" << endl;
				break;
		}
	} while (!start);
}


/****************************************************************************
				CONSTRUCT ENVIRONMENT
*****************************************************************************/

void constructEnvironment() {

	cout << "(Leaving the files as 'none' randomizes robots and lights)" << endl;
	menu();

	if (creatures_file == "none") {
		buildCreatures();
		cout << "~Robots Built Randomly~" << endl;
	}
	else {
		build_creatures_from_file(creatures_file);
		cout << "~Robots Built From File" << endl;
	}

	if (lights_file == "none") {
		buildLights();
		cout << "~Lights Build Randomly~" << endl;
	}
	else {
		build_lights_from_file(lights_file);
		cout << "~Lights Built From File~" << endl;
	}
}


/****************************************************************************
				MAIN FUNCTION
*****************************************************************************/

int main(int argc, char** argv) {
	srand(static_cast <unsigned> (time(0)));

	// Builds creatures and lights
	constructEnvironment();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(800, 100);
	glutCreateWindow("Project 4");
	glutDisplayFunc(display);

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();

	if (glewError == GLEW_OK) {
		printf("\n\n 8^) \n");
		setupGlut();
		glutMainLoop();
	}
	else {
		printf(" :( \n");
	}

	return 0;
}
