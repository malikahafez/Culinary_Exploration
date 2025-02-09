#include <math.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <random>
#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"

#include <SFML/Audio.hpp>
#include <glut.h>
#include <iostream>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)//convert degrees to radians
#define GLUT_KEY_SPACE 32

int WIDTH = 1000;//window height
int HEIGHT = 600;//window width

GLuint tex;//texture pointer
GLuint tex2;//texture pointer
GLuint tex3;//texture pointer
GLuint tex4;//texture pointer
GLuint tex5;//texture pointer
GLuint tex6;//texture pointer
GLuint tex7;//texture pointer
GLuint tex8;//texture pointer
GLuint tex9;//texture pointer
GLuint tex10;//texture pointer





char title[] = "Culinary Exploration";//window title

//level state controls
int lvl = 1;//to indicate which level to display
int times1 = 60;//time of first level (1:00 min)
int times2 = 60;//time of second level (1:00 min)
int timer = 1000;//switch to 5000 to play game loss or game win sound
bool end = false;//time ran out or not
bool win = false;//collected all goal objects or not
int score = 0;//score of player
int hp = 3;//lives decrease after collision with specific obstacles 3 lives initially

//camera view
bool firstP = false;
bool thirdP = true;

//player
double pR = 90;//player rotation angle
double pX = 5.0;//player x position start at 1.5 to collect should be at 2.5 collide with right fence at 2.7 collide with left fence at 0.3
double pZ = 9;//player z position start at 4.5 to collect should be at 1.6 collide with back fence at -0.15 player goes up if <1.2
double pY = 0;//player Height in space (for jumping)
bool jump = false;//player jumping
int jumpCount = 8;//timed jump

//collectibles + obstacles
float ballHeight = 0.8;
float fruitHeight = 0.8;
float vegHeight = 0.8;
float dishHeight = 0.8;
float jumpSpeed = 2.0;
float ballRadius = 0.5;
float animRadius = 0.5;
float playerRadius = 0.7;
float minX = -8.0;
float maxX = 8;
float minZ = -8;
float maxZ = 8;
double cRot = 0;


//sounds and music
bool moving = false;
//footsteps on grass
sf::Sound move;
sf::SoundBuffer sound_Buffer1;
sf::Sound move2;
sf::SoundBuffer sound_Buffer2;
sf::Sound move3;
sf::SoundBuffer sound_Buffer3;
sf::Sound move4;
sf::SoundBuffer sound_Buffer4;
//game win
sf::Sound winner;
sf::SoundBuffer sound_Buffer5;
//game lose
sf::Sound loser;
sf::SoundBuffer sound_Buffer6;
//jump sfx
sf::Sound jumpSnd;
sf::SoundBuffer sound_Buffer7;
//collect sound
sf::Sound cltSnd;
sf::SoundBuffer sound_Buffer8;
//duck
sf::Sound duckSnd;
sf::SoundBuffer sound_Buffer9;
//dog
sf::Sound dogSnd;
sf::SoundBuffer sound_Buffer10;
//man
sf::Sound manSnd;
sf::SoundBuffer sound_Buffer11;
//woman
sf::Sound wmanSnd;
sf::SoundBuffer sound_Buffer12;
//death
sf::Sound dieSnd;
sf::SoundBuffer sound_Buffer13;
// collide
sf::Sound collideSnd;
sf::SoundBuffer sound_Buffer14;

//background music farm
sf::Music farmMusic;
//background music restaurant
sf::Music restoMusic;

bool resto = false;

//environment and lighting
double bR = 1;
double bG = 1;
double bB = 1;
float sunR = 1.0f;//full sun 1.0f sunset 1.0f moon 0.063f
float sunG = 1.0f;//full sun 1.0f sunset 0.714f moon 0.302
float sunB = 0.741f;//full sun 0.741f  sunset 0.02f moon 0.788
double sunRot = 0;
bool day = true;//if false turn on the lamp lights
bool day2 = true;
double lR = 0;
double lR2 = 0;

// 3D Projection Options for perspective
GLdouble fovy = 45.0;//field of view
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;//aspect ratio of window
GLdouble zNear = 0.1;
GLdouble zFar = 100;

//camera EYE vector:
float ex = 2.0f;
float ey = 1.8f;
float ez = 10.0f;
//camera CENTER vector:
float cx = 2.0f;
float cy = 1.8f;
float cz = 0.0f;
//camera UP vector
float ux = 0.0f;
float uy = 1.0f;
float uz = 0.0f;

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};
class Camera {
public:
	Vector3f eye, center, up;
	//to adjust distance of camera from subject, change values of eye originally {1.0f,1.0f,1.0f}
	//to adjust where the camera is looking change values of center originally {0.0f, 0.0f, 0.0f}
	Camera(float eyeX = ex, float eyeY = ey, float eyeZ = ez, float centerX = cx, float centerY = cy, float centerZ = cz, float upX = ux, float upY = uy, float upZ = uz) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}
	void topV() {//get top view
		eye = { 2.0f, 5.0f, 3.0f };//top view
		center = { 2.0f, 0.0f, 1.5f };//looking down
	}
	void frontV() {//get front view
		eye = { 2.0f, 0.8f, 5.7f };//front view
		center = { 2.0f, 0.5f, 0.0f };//horizontal center of game scene
	}
	void opfrontV() {//opposite of front view
		eye = { 2.0f, 0.5f, 1.0f };//horizontal center of game scene
		center = { 2.0f, 0.8f, 5.7f };//front view
	}
	void sideV() {//get side view
		eye = { 3.5f, 0.8f, 3.0f };//side view
		center = { 0.0f, 0.8f, 3.0f };//looking to the left
	}
	void backV() {//get opposite of side view
		eye = { 0.1f, 0.8f, 3.0f };//opposite of side view
		center = { 3.5f, 0.8f, 3.0f };//looking to right
	}
	void thirdPerson() {
		eye.z = pZ + 2.5 ;
		eye.y = ey;
		eye.x = pX;
		
		thirdP = true;
		firstP = false;
	}
	void firstPerson() {
		eye.z = pZ - 0.1;
		eye.y = 1.3;
		eye.x = pX;
		firstP = true;
		thirdP = false;
	}
	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
	void setCenter(float cenX, float cenY, float cenZ) {
		center = { cenX,cenY,cenZ };
	}
	void setEye(float eyeX, float eyeY, float eyeZ){
		eye = { eyeX, eyeY, eyeZ };
	}
};

Camera camera;
Camera camera2;

int cameraZoom = 0;//for zooming in



// Model Variables
Model_3DS model_house;//house model
Model_3DS model_tree;//tree model


Model_3DS model_barn;//barn model
Model_3DS farmer;
Model_3DS waiter;
Model_3DS apple;
Model_3DS banana;
Model_3DS carrot;
Model_3DS pumpkin;
Model_3DS fence;
Model_3DS chick;
Model_3DS duck;
Model_3DS dog;
Model_3DS sushi;
Model_3DS dango;
Model_3DS rice;
Model_3DS rice2;
Model_3DS woman;
Model_3DS table;
Model_3DS wall;


// Textures
GLTexture tex_ground;//ground texture
GLTexture tex_barn;//ground texture
GLTexture tex_floor;//floor texture


void InitSunMoon() {
	//sunlight/moonlight
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient0[] = { 0.7, 0.7, 0.7, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);

	// Define Light source 0 diffuse light change colour of light here
	GLfloat diffuse0[] = { sunR, sunG, sunB, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);

	// Define Light source 0 Specular light
	GLfloat specular0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

	// Finally, define light source 0 position in World Space
	GLfloat light_position0[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

}

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSources()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	
	
	if (!day) {
		glPushMatrix();
		glRotated(lR, 0, 1, 0);
		//lamp light 1
		glEnable(GL_LIGHT1);

		//// Define Light source 1 ambient light
		//GLfloat ambient1[] = { 0.1f, 0.1f, 0.1, 1.0f };
		//glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);

		// Define Light source 1 diffuse light change colour of light here
		GLfloat diffuse1[] = { 1.0f, 0.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);

		// Define Light source 1 Specular light
		GLfloat specular1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);

		// Finally, define light source 1 position in World Space
		GLfloat light_position1[] = { 20, 5.0f, 10, 0.0f };
		glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

		//
		GLfloat Direction1[] = { -1.0, 0.0, 0.0 };

		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 100.0);
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 100.0);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, Direction1);
		glPopMatrix();
		glPushMatrix();
		glRotated(lR2, 0, 0, 1);
		//lamp light 2
		glEnable(GL_LIGHT2);

		//// Define Light source 2 ambient light
		//GLfloat ambient2[] = { 0.1f, 0.1f, 0.1, 1.0f };
		//glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);

		// Define Light source 2 diffuse light change colour of light here
		GLfloat diffuse2[] = { 1.0f, 0.0f, 0.0f, 1.0f };;
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);

		// Define Light source 2 Specular light
		GLfloat specular2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);

		// Finally, define light source 2 position in World Space
		GLfloat light_position2[] = { -20, 5.0f, 10, 1.0f };
		glLightfv(GL_LIGHT2, GL_POSITION, light_position2);

		GLfloat Direction2[] = { 1.0, 0.0, 0.0 };

		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 100.0);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 100.0);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, Direction2);
		glPopMatrix();

	//	//lamp light 3
	//	glEnable(GL_LIGHT3);

	//	// Define Light source 3 ambient light
	//	GLfloat ambient3[] = { 0.1f, 0.1f, 0.1, 1.0f };
	//	glLightfv(GL_LIGHT3, GL_AMBIENT, ambient3);

	//	// Define Light source 3 diffuse light change colour of light here
	//	GLfloat diffuse3[] = { 0.82f, 0.0f, 1.0f, 1.0f };
	//	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse3);

	//	// Define Light source 3 Specular light
	//	GLfloat specular3[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//	glLightfv(GL_LIGHT3, GL_SPECULAR, specular3);

	//	// Finally, define light source 3 position in World Space
	//	GLfloat light_position3[] = { 10, 5.0f, 20, 1.0f };
	//	glLightfv(GL_LIGHT3, GL_POSITION, light_position3);

	//	GLfloat Direction3[] = { 0.0, 0.0, -1.0 };

	//	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 50.0);
	//	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 90.0);
	//	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, Direction3);

	//	//lamp light 4
	//	glEnable(GL_LIGHT4);

	//	// Define Light source 4 ambient light
	//	GLfloat ambient4[] = { 0.1f, 0.1f, 0.1, 1.0f };
	//	glLightfv(GL_LIGHT4, GL_AMBIENT, ambient4);

	//	// Define Light source 4 diffuse light change colour of light here
	//	GLfloat diffuse4[] = { 1.0f, 0.0f, 0.129f, 1.0f };
	//	glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuse4);

	//	// Define Light source 4 Specular light
	//	GLfloat specular4[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//	glLightfv(GL_LIGHT4, GL_SPECULAR, specular4);

	//	// Finally, define light source 4 position in World Space
	//	GLfloat light_position4[] = { 10, 5.0f, -20, 1.0f };
	//	glLightfv(GL_LIGHT4, GL_POSITION, light_position4);

	//	GLfloat Direction4[] = { 0.0, 0.0, 1.0 };

	//	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 50.0);
	//	glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 90.0);
	//	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, Direction4);
	}
	else if (!day2) {
		glPushMatrix();
		glRotated(lR, 0, 1, 0);
		//lamp light 1
		glEnable(GL_LIGHT1);

		//// Define Light source 1 ambient light
		//GLfloat ambient1[] = { 0.1f, 0.1f, 0.1, 1.0f };
		//glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);

		// Define Light source 1 diffuse light change colour of light here
		GLfloat diffuse1[] = { 1.0f, 0.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);

		// Define Light source 1 Specular light
		GLfloat specular1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);

		// Finally, define light source 1 position in World Space
		GLfloat light_position1[] = { 20, 5.0f, 10, 0.0f };
		glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

		//
		GLfloat Direction1[] = { -1.0, 0.0, 0.0 };

		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 100.0);
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 90.0);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, Direction1);
		glPopMatrix();
		glPushMatrix();
		glRotated(lR2, 0, 0, 1);
		//lamp light 2
		glEnable(GL_LIGHT2);

		//// Define Light source 2 ambient light
		//GLfloat ambient2[] = { 0.1f, 0.1f, 0.1, 1.0f };
		//glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);

		// Define Light source 2 diffuse light change colour of light here
		GLfloat diffuse2[] = { 0.0f, 1.0f, 1.0f, 1.0f };;
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);

		// Define Light source 2 Specular light
		GLfloat specular2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);

		// Finally, define light source 2 position in World Space
		GLfloat light_position2[] = { -20, 5.0f, 10, 1.0f };
		glLightfv(GL_LIGHT2, GL_POSITION, light_position2);

		GLfloat Direction2[] = { 1.0, 0.0, 0.0 };

		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 100.0);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 90.0);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, Direction2);
		glPopMatrix();

	}
	else {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}

}
//print text in 3d space
void print(double x, double y, double z, void* font, std::string string) {
	glRasterPos3d(x, y, z);
	for (int i = 0; i < string.length(); i++) {
		glutBitmapCharacter(font, string[i]);
	}
}
void printTime(int tim) {
	glColor3f(0, 0, 0);
	int min = tim / 60;
	int sec = tim % 60;
	print(38, 2.4,0, GLUT_BITMAP_HELVETICA_18, "Time");
	std::string t = std::to_string(min) + " : " + ((std::to_string(sec).length() < 2) ? ("0" + std::to_string(sec)) : std::to_string(sec));
	print(38, 0.5, 0, GLUT_BITMAP_HELVETICA_18, t);

}
void printScore() {
	glColor3f(0, 0, 0);
	int ss = score;
	std::string s1 = std::to_string(ss % 10);
	ss = ss / 10;
	std::string s2 = std::to_string(ss % 10);
	ss = ss / 10;
	std::string s3 = std::to_string(ss % 10);
	ss = ss / 10;
	std::string s4 = std::to_string(ss % 10);
	std::string s = s4 + s3 + s2 + s1;
	print(32, 2.4,0, GLUT_BITMAP_HELVETICA_18, "Score");
	print(32.05, 0.5,0, GLUT_BITMAP_HELVETICA_18, s);
}


//collectibles
float getRandomFloat(float min, float max) {
	static std::mt19937 rng(12345);
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

float ballx = getRandomFloat(minX, maxX);
float ballz = getRandomFloat(minZ, maxZ);

void resetBallPosition() {
	ballx = getRandomFloat(minX, maxX);
	ballz = getRandomFloat(minZ, maxZ);
}

void drawBall(float ballx, float ballz) { //fruit
	//glColor3d(1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(ballx, ballHeight, ballz);
	glRotated(cRot, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	glScaled(15, 15, 15);
	pumpkin.Draw();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

float ball1x = getRandomFloat(minX, maxX);
float ball1z = getRandomFloat(minZ, maxZ);

void resetBall1Position() {
	ball1x = getRandomFloat(minX, maxX);
	ball1z = getRandomFloat(minZ, maxZ);
}

void drawBall1(float ball1x, float ball1z) { //fruit
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(ball1x, ballHeight, ball1z);
	glRotated(cRot, 0, 1, 0);

	glScaled(5, 5, 5);
	carrot.Draw();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

float fruitx = getRandomFloat(minX, maxX);
float fruitz = getRandomFloat(minZ, maxZ);

void resetFruitPosition() {
	fruitx = getRandomFloat(minX, maxX);
	fruitz = getRandomFloat(minZ, maxZ);
}

void drawFruit(float fruitx, float fruitz) { //fruit
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(fruitx, ballHeight, fruitz);
	glRotated(cRot, 0, 1, 0);

	glScaled(25, 25, 25);

	banana.Draw();
	
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

float fruit1x = getRandomFloat(minX, maxX);
float fruit1z = getRandomFloat(minZ, maxZ);

void resetFruit1Position() {
	fruit1x = getRandomFloat(minX, maxX);
	fruit1z = getRandomFloat(minZ, maxZ);
}

void drawFruit1(float fruit1x, float fruit1z) { //fruit
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(fruit1x, ballHeight, fruit1z);
	glRotated(cRot, 0, 1, 0);

	glScaled(25, 25, 25);

	banana.Draw();
	
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

float vegx = getRandomFloat(minX, maxX);
float vegz = getRandomFloat(minZ, maxZ);

void resetVegPosition() {
	vegx = getRandomFloat(minX, maxX);
	vegz = getRandomFloat(minZ, maxZ);
}

void drawVeg(float vegx, float vegz) { //Veggie
	//glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(vegx, vegHeight, vegz);
	glRotated(cRot, 0, 1, 0);

	glScaled(5, 5, 5);
	carrot.Draw();
	glPopMatrix();
	//glDisable(GL_TEXTURE_2D);
}

float veg1x = getRandomFloat(minX, maxX);
float veg1z = getRandomFloat(minZ, maxZ);

void resetVeg1Position() {
	veg1x = getRandomFloat(minX, maxX);
	veg1z = getRandomFloat(minZ, maxZ);
}

void drawVeg1(float veg1x, float veg1z) { //Veggie
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(veg1x, ballHeight, veg1z);
	glRotated(cRot, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	glScaled(15, 15, 15);
	pumpkin.Draw();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

float Specialfruitx = 0;
float Specialfruity = 2;
float Specialfruitz = 0;
void resetFruitSPosition() {
	Specialfruitx = getRandomFloat(minX, maxX);
	Specialfruitz = getRandomFloat(minZ, maxZ);
}
void drawFruitSpecial(float x, float z) {
	glPushMatrix();
	glTranslatef(x, Specialfruity, z); // Position the dish in 3D space
	glTranslated(-6, 0, 1);
	//glColor3f(1.0f, 1.0f, 1.0f); // White color for plate (you can change this)
	//glRotated(cRot, 0, 1, 0);
	//glTranslated(6, 0, -1);
	glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate to make it flat
	//glScaled(3, 3, 3);

	dango.Draw();
	glPopMatrix();


}


void drawTree() {
	glColor3d(0.6, 0.3, 0.1);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(-0.1, 0.3, -0.1);
	glScaled(1, 6, 1);
	glutSolidCube(0.1);     //stand
	glPopMatrix();
	glColor3d(0, 0.5, 0);
	glPushMatrix();
	glTranslated(-0.2, 0.6, 0.0);
	glutSolidSphere(0.07, 15, 15);      //leaf1
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.1, 0.6, 0.0);
	glutSolidSphere(0.07, 15, 15);      //leaf2
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.0, 0.6, 0.0);
	glutSolidSphere(0.07, 15, 15);      //leaf3
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.2, 0.6, -0.09);
	glutSolidSphere(0.07, 15, 15);      //leaf4
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.2, 0.6, -0.18);
	glutSolidSphere(0.07, 15, 15);      //leaf5
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.1, 0.6, -0.18);
	glutSolidSphere(0.07, 15, 15);      //leaf6
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.0, 0.6, -0.18);
	glutSolidSphere(0.07, 15, 15);      //leaf7
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.0, 0.6, -0.09);
	glutSolidSphere(0.07, 15, 15);      //leaf8
	glPopMatrix();
	glPushMatrix();
	glPushMatrix();
	glTranslated(-0.1, 0.6, -0.09);
	glutSolidSphere(0.07, 15, 15);      //leaf9
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.15, 0.7, -0.045);
	glutSolidSphere(0.07, 15, 15);      //leaf10
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.15, 0.7, -0.125);
	glutSolidSphere(0.07, 15, 15);      //leaf11
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.05, 0.7, -0.125);
	glutSolidSphere(0.07, 15, 15);      //leaf12
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.05, 0.7, -0.045);
	glutSolidSphere(0.07, 15, 15);      //leaf13
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.1, 0.8, -0.09);
	glutSolidSphere(0.07, 15, 15);      //leaf14
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

void drawBarn() { //barn
	glColor3d(1.0, 1.0, 1.0);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(1, 0.2, 0);
	glutSolidCube(0.35);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}
void drawChicken() {
	glColor3d(1.0, 1.0, 0.0);
	glPushMatrix();
	glTranslated(0, 0.07, 1);
	glScaled(1, 0.7, 1);
	glutSolidSphere(0.04, 15, 15);
	glPopMatrix();

	//glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslated(0.02, 0.11, 1);
	/*GLUquadricObj* qobj11;
	qobj11 = gluNewQuadric();*/
	//glTranslated(0, 0, 0);
	//glRotated(90, 1, 0, 1);
	/*glBindTexture(GL_TEXTURE_2D, tex9);
	gluQuadricTexture(qobj11, true);
	gluQuadricNormals(qobj11, GL_SMOOTH);
	gluSphere(qobj11, 0.025, 15, 15);
	gluDeleteQuadric(qobj11);*/
	glutSolidSphere(0.025, 15, 15);
	glPopMatrix();
	//glDisable(GL_TEXTURE_2D);

	glColor3d(0.7, 0.7, 0.0);
	glPushMatrix();
	glTranslated(0, 0.07, 1);
	glScaled(0.8, 0.5, 1.1);
	glutSolidSphere(0.04, 15, 15);
	glPopMatrix();

	glColor3d(0.0, 0.0, 0.0);
	glPushMatrix();
	glTranslated(0.01, 0.05, 1.0);
	glScaled(1, 3, 1);
	glutSolidCube(0.01);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.01, 0.05, 1.0);
	glScaled(1, 3, 1);
	glutSolidCube(0.01);
	glPopMatrix();

	glColor3d(0.5, 0.5, 0.0);
	glPushMatrix();
	glTranslated(0.042, 0.11, 1.0);
	glRotatef(90, 0, 1, 0);
	glScaled(1, 3, 1);
	glutSolidCone(0.005, 0.01, 15, 15);
	glPopMatrix();


}

float chickX = getRandomFloat(minX, maxX);
float chickZ = getRandomFloat(minZ, maxZ);

void resetanimalPosition() {
	chickX = getRandomFloat(minX, maxX);
	chickZ = getRandomFloat(minZ, maxZ);
}
void drawAnimal() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(chickX, 0.08, chickZ);
	glScaled(0.8, 0.8, 0.8);
	glRotated(180, 0, 1, 0);

	glRotated(90, 1, 0, 0);

	dog.Draw();

	glPopMatrix();
}
float dogX = getRandomFloat(minX, maxX);
float dogZ = getRandomFloat(minZ, maxZ);

void resetanimal1Position() {
	dogX = getRandomFloat(minX, maxX);
	dogZ = getRandomFloat(minZ, maxZ);
}

void drawAnimal1() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(dogX, 0.08, dogZ);
	glScaled(0.8, 0.8, 0.8);

	glRotated(180, 0, 1, 0);

	glRotated(90, 1, 0, 0);



	dog.Draw();

	glPopMatrix();
}

float duckX = getRandomFloat(minX, maxX);
float duckZ = getRandomFloat(minZ, maxZ);

void resetanimal2Position() {
	duckX = getRandomFloat(minX, maxX);
	duckZ = getRandomFloat(minZ, maxZ);
}

void drawAnimal2() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(duckX, 0.08, duckZ);
	glScaled(0.008, 0.008, 0.008);

	glRotated(15, 1, 0, 0);


	duck.Draw();
	glPopMatrix();
}
float duck2X = getRandomFloat(minX, maxX);
float duck2Z = getRandomFloat(minZ, maxZ);

void resetanimal3Position() {
	duck2X = getRandomFloat(minX, maxX);
	duck2Z = getRandomFloat(minZ, maxZ);
}

void drawAnimal3() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(duck2X, 0.08, duck2Z);
	glScaled(0.008, 0.008, 0.008);
	glRotated(15, 1, 0, 0);




	duck.Draw();
	glPopMatrix();
}

float dog2X = getRandomFloat(minX, maxX);
float dog2Z = getRandomFloat(minZ, maxZ);

void resetanimal4Position() {
	dog2X = getRandomFloat(minX, maxX);
	dog2Z = getRandomFloat(minZ, maxZ);
}

void drawAnimal4() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(dog2X, 0.08, dog2Z);
	glScaled(0.8, 0.8, 0.8);

	glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);


	dog.Draw();
	glPopMatrix();
}

float fenceX = 8;
float fenceZ = 10;
void drawfence() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(fenceX, 0, fenceZ);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fenceX-2, 0, fenceZ);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fenceX - 4, 0, fenceZ);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fenceX - 6, 0, fenceZ);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fenceX - 8, 0, fenceZ);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fenceX - 10, 0, fenceZ);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fenceX - 12, 0, fenceZ);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fenceX - 14, 0, fenceZ);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fenceX - 16, 0, fenceZ);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
}

float fence1X = 8;
float fence1Z = -10;
void drawfence1() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(fence1X, 0, fence1Z);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence1X - 2, 0, fence1Z);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence1X - 4, 0, fence1Z);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence1X - 6, 0, fence1Z);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence1X - 8, 0, fence1Z);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence1X - 10, 0, fence1Z);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence1X - 12, 0, fence1Z);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence1X - 14, 0, fence1Z);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence1X - 16, 0, fence1Z);
	glScaled(20, 20, 20);
	//glRotated(180, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
}

float fence2X = -10;
float fence2Z = 8;
void drawfence2() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(fence2X, 0, fence2Z);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence2X , 0, fence2Z - 2);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence2X, 0, fence2Z - 4);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence2X, 0, fence2Z - 6);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence2X, 0, fence2Z - 8);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence2X, 0, fence2Z - 10);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence2X, 0, fence2Z - 12);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence2X, 0, fence2Z - 14);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence2X, 0, fence2Z - 16);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
}

float fence3X = 10;
float fence3Z = 8;
void drawfence3() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(fence3X, 0, fence3Z);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence3X, 0, fence3Z - 2);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence3X, 0, fence3Z - 4);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence3X, 0, fence3Z - 6);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence3X, 0, fence3Z - 8);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence3X, 0, fence3Z - 10);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence3X, 0, fence3Z - 12);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence3X, 0, fence3Z - 14);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence3X, 0, fence3Z - 16);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);

	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
}

float fence4X = 6;
float fence4Z = 10;
void drawfence4() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(fence4X, 0, fence4Z);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence4X, 0, fence4Z - 2);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
}

float fence5X = -4;
float fence5Z = 9;
void drawfence5() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(fence5X, 0, fence5Z);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence5X, 0, fence5Z - 2);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
}

float fence6X = -4;
float fence6Z = -6;
void drawfence6() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(fence6X, 0, fence6Z);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
	glPushMatrix();
	glTranslated(fence6X, 0, fence6Z - 2);
	glScaled(20, 20, 20);
	glRotated(90, 0, 1, 0);
	glRotated(90, 1, 0, 0);
	fence.Draw();
	glPopMatrix();
}




void drawCircle(double r) {
	GLUquadric* quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 20, 20);
}
void drawCircleH(double r, double rin) {
	GLUquadric* quadObj = gluNewQuadric();
	gluDisk(quadObj, rin, r, 20, 20);
}
void drawTriangle(int x, int y, double h, int b) {

	glBegin(GL_TRIANGLES);
	glVertex2d(x, y);
	glVertex2d(x, (y + h));
	glVertex2d((x + b), y);
	glEnd();
}
void drawEquilTriangle(int x, int y, int a) {
	int h = (sqrt(3) / 2) * a;
	int b = a / 2;
	drawTriangle(x, y, h, b);
	drawTriangle(x, y, h, -b);

}
void drawHexagon(int x, int y, int w1, int w2, int h) {
	int h1 = h / 2;
	glBegin(GL_POLYGON);
	glVertex2d(x, y);
	glVertex2d((x + w2), (y - h1));
	glVertex2d((x + w2 + w1), (y - h1));
	glVertex2d((x + w2 + w1 + w2), y);
	glVertex2d((x + w2 + w1), (y + h1));
	glVertex2d((x + w2), (y + h1));
	glEnd();
}
void drawHealth() {
	
	glPushMatrix();
	glTranslated(1, 2, 0);
	glScaled(1, 1.5, 1);
	glColor3f(1, 1, 1);
	drawCircle(1);
	glPopMatrix();

	
	glPushMatrix();
	glTranslated(1, 2, 0);
	glScaled(1, 1.5, 1);
	glColor3f(0.5, 0.5, 0.5);
	drawCircleH(0.7,0.6);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1, 2, 0);
	glScaled(1, 1.5, 1);
	glColor3f(0.5, 0.5, 0.5);
	drawCircleH(1, 0.9);
	glPopMatrix();

}
void health() {


	glColor3f(0.9, 0.9, 0.792);
	drawHexagon(0, 2, 5, 2, 8);
	double m = 1;//x position of each health token
	for (int i = 0; i < hp; i++) {
		glPushMatrix();
		glTranslated(m, 0, 0);
		drawHealth();
		glPopMatrix();
		m += 2.5;
	}
}

bool checkCollision(float startx, float startz, float ballx, float ballz, float ballRadius, float playerRadius) {

	if (startx < ballx + ballRadius && startx + playerRadius > ballx && startz < ballz + ballRadius && startz + playerRadius > ballz) {
		return true;
	}

	return false;
}

bool checkfCollision(float startz, float ballz, float ballRadius, float playerRadius) {

	if (startz < ballz + ballRadius && startz + playerRadius > ballz) {
		return true;
	}

	return false;
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void Init(void)
{
	//background colour
	glClearColor(bR, bG, bB, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
	//gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//
	InitSunMoon();
	InitLightSources();

	InitMaterial();



	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	//glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-10, 0, -10);
	glTexCoord2f(5, 0);
	glVertex3f(10, 0, -10);
	glTexCoord2f(5, 5);
	glVertex3f(10, 0, 10);
	glTexCoord2f(0, 5);
	glVertex3f(-10, 0, 10);
	glEnd();
	glPopMatrix();

	//glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void RenderFloor()
{
	//glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_floor.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-10, 0, -10);
	glTexCoord2f(5, 0);
	glVertex3f(10, 0, -10);
	glTexCoord2f(5, 5);
	glVertex3f(10, 0, 10);
	glTexCoord2f(0, 5);
	glVertex3f(-10, 0, 10);
	glEnd();
	glPopMatrix();

	//glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void drawTopBorder() {
	glColor3f(0.565, 0.737, 0.922);
	glPushMatrix();
	glTranslated(30, -1,0);
	glScaled(5, 2, 1);
	glBegin(GL_QUADS);
	glVertex2d(-6, 0);
	glVertex2d(-6, 10);
	glVertex2d(6, 10);
	glVertex2d(6, 0);
	glEnd();
	glPopMatrix();

}

//=======================================================================
// Display Function
//=======================================================================
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, 1000, 550);
	gluPerspective(60, 1000 / 550, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();

}
void setupCamera2() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 550, 1000, 50);
	glOrtho(-2.0, 2.0 * 1000 / 50, -2.0, 2.0, 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera2.look();
}


void drawCylinder(float radius, float height, int slices, int stacks) {
	// Draw the bottom cap of the cylinder
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, 0.0f); // Center of the bottom circle
	for (int i = 0; i <= slices; ++i) {
		float angle = i * 2.0f * 3.14159f / slices;
		float x = cos(angle) * radius;
		float z = sin(angle) * radius;
		glVertex3f(x, 0.0f, z);
	}
	glEnd();

	// Draw the side of the cylinder
	for (int i = 0; i < stacks; ++i) {
		float y0 = i * height / stacks;
		float y1 = (i + 1) * height / stacks;
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j <= slices; ++j) {
			float angle = j * 2.0f * 3.14159f / slices;
			float x = cos(angle) * radius;
			float z = sin(angle) * radius;
			glVertex3f(x, y0, z);
			glVertex3f(x, y1, z);
		}
		glEnd();
	}

	// Draw the top cap of the cylinder
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, height, 0.0f); // Center of the top circle
	for (int i = 0; i <= slices; ++i) {
		float angle = i * 2.0f * 3.14159f / slices;
		float x = cos(angle) * radius;
		float z = sin(angle) * radius;
		glVertex3f(x, height, z);
	}
	glEnd();
}

void drawWalls() {
	glColor3f(0.8f, 0.8f, 0.8f);
	glPushMatrix();
	glTranslated(0, -1, -10);
	glScaled(4, 2, 6);
	glBegin(GL_QUADS);

	// Back wall
	
	glVertex3f(-5.0f, 0.0f, -5.0f);
	glVertex3f(5.0f, 0.0f, -5.0f);
	glVertex3f(5.0f, 5.0f, -5.0f);
	glVertex3f(-5.0f, 5.0f, -5.0f);
	

	// Left wall
	glVertex3f(-5.0f, 0.0f, -5.0f);
	glVertex3f(-5.0f, 0.0f, 5.0f);
	glVertex3f(-5.0f, 5.0f, 5.0f);
	glVertex3f(-5.0f, 5.0f, -5.0f);

	// Right wall
	glVertex3f(5.0f, 0.0f, -5.0f);
	glVertex3f(5.0f, 0.0f, 5.0f);
	glVertex3f(5.0f, 5.0f, 5.0f);
	glVertex3f(5.0f, 5.0f, -5.0f);

	glEnd();
	glPopMatrix();
}

void drawTable(float x, float z) {
	glPushMatrix();
	glTranslatef(x, 0.5f, z);

	// Tabletop
	glColor3f(0.6f, 0.3f, 0.0f); // Brown
	glPushMatrix();
	glScalef(1.5f, 0.1f, 1.5f); // Scale to make it flat
	glutSolidCube(1.0);
	glPopMatrix();

	// Table legs
	glColor3f(0.4f, 0.2f, 0.0f);
	for (float dx = -0.6f; dx <= 0.6f; dx += 1.2f) {
		for (float dz = -0.6f; dz <= 0.6f; dz += 1.2f) {
			glPushMatrix();
			glTranslatef(dx, -0.5f, dz);
			glScalef(0.1f, 1.0f, 0.1f);
			glutSolidCube(1.0);
			glPopMatrix();
		}
	}
	glPopMatrix();
}
void drawChair(float x, float z) {
	glPushMatrix();
	glTranslatef(x, 0.5f, z);

	// Draw chair seat (a small flat square)
	glColor3f(0.5f, 0.25f, 0.0f); // Brown color for the seat
	glPushMatrix();
	glScalef(0.6f, 0.05f, 0.6f); // Scale to make it flat
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw chair legs
	glColor3f(0.4f, 0.2f, 0.0f); // Darker brown for the legs
	// Front left leg
	glPushMatrix();
	glTranslatef(-0.25f, -0.5f, -0.25f);
	glScalef(0.1f, 1.0f, 0.1f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Front right leg
	glPushMatrix();
	glTranslatef(0.25f, -0.5f, -0.25f);
	glScalef(0.1f, 1.0f, 0.1f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Back left leg
	glPushMatrix();
	glTranslatef(-0.25f, -0.5f, 0.25f);
	glScalef(0.1f, 1.0f, 0.1f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Back right leg
	glPushMatrix();
	glTranslatef(0.25f, -0.5f, 0.25f);
	glScalef(0.1f, 1.0f, 0.1f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

void drawWaiter(float x, float z) {
	glPushMatrix();
	glTranslatef(x, 0.5f, z); // Position the waiter at (x, z)
	
	glScaled(0.3, 0.3, 0.3);
	glRotated(pR, 0, 1, 0);
	glRotated(90, 0, 1, 0);
	

	glPopMatrix();
}

void drawDish(float x, float y, float z) {
	// Draw plate (a flat circle)
	glPushMatrix();
	glTranslatef(x, y, z); // Position the dish in 3D space
	glColor3f(1.0f, 1.0f, 1.0f); // White color for plate (you can change this)
	//glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate to make it flat
	glScaled(1.2, 0.2, 1.2);
	glutSolidSphere(0.3, 20, 20); // Create the plate (torus shape)
	glPopMatrix();

	// Draw food (a sphere placed on top of the plate)
	glPushMatrix();
	glTranslatef(x, y + 0.1f, z); // Position food slightly above plate
	glColor3f(1.0f, 0.5f, 0.0f); // Orange color for food (you can change this)
	glScaled(1.5, 1, 1);
	glutSolidSphere(0.2, 10, 10); // Create food (sphere shape)
	glPopMatrix();
}

float dishx = getRandomFloat(minX, maxX);
float dishz = getRandomFloat(minZ, maxZ);
void resetDishPosition() {
	dishx = getRandomFloat(minX, maxX);
	dishz = getRandomFloat(minZ, maxZ);
}
void drawDishClt(float x, float z) {
	// Draw plate (a flat circle)
	glPushMatrix();
	glTranslatef(x, dishHeight, z); // Position the dish in 3D space
	glRotated(cRot, 0, 1, 0);

	glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate to make it flat
	//glScaled(3, 3, 3);
	glColor3f(1.0f, 1.0f, 1.0f); // White color for plate (you can change this)

	sushi.Draw();
	glPopMatrix();
}

float dish1x = getRandomFloat(minX, maxX);
float dish1z = getRandomFloat(minZ, maxZ);
void resetDish1Position() {
	dish1x = getRandomFloat(minX, maxX);
	dish1z = getRandomFloat(minZ, maxZ);
}
void drawDish1Clt(float x, float z) {
	// Draw plate (a flat circle)
	glPushMatrix();
	glTranslatef(x, dishHeight, z); // Position the dish in 3D space
	glRotated(cRot, 0, 1, 0);

	glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate to make it flat
	glColor3f(1.0f, 1.0f, 1.0f); // White color for plate (you can change this)
	//glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate to make it flat
	//glScaled(3, 3, 3);

	sushi.Draw();
	glPopMatrix();

	
}

float dish2x = getRandomFloat(minX, maxX);
float dish2z = getRandomFloat(minZ, maxZ);
void resetDish2Position() {
	dish2x = getRandomFloat(minX, maxX);
	dish2z = getRandomFloat(minZ, maxZ);
}
void drawDish2Clt(float x, float z) {
	// Draw plate (a flat circle)
	glPushMatrix();
	glTranslatef(x, dishHeight, z); // Position the dish in 3D space
	//glColor3f(1.0f, 1.0f, 1.0f); // White color for plate (you can change this)
	glRotated(cRot, 0, 1, 0);

	glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate to make it flat
	//glScaled(3, 3, 3);
	glScaled(2, 2, 2);
	rice.Draw();
	glPopMatrix();

	
}

float dish3x = getRandomFloat(minX, maxX);
float dish3z = getRandomFloat(minZ, maxZ);
void resetDish3Position() {
	dish3x = getRandomFloat(minX, maxX);
	dish3z = getRandomFloat(minZ, maxZ);
}
void drawDish3Clt(float x, float z) {
	// Draw plate (a flat circle)
	glPushMatrix();
	glTranslatef(x, dishHeight, z); // Position the dish in 3D space
	//glColor3f(1.0f, 1.0f, 1.0f); // White color for plate (you can change this)
	glRotated(cRot, 0, 1, 0);

	glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate to make it flat
	//glScaled(3, 3, 3);
	glScaled(2, 2, 2);

	rice2.Draw();
	glPopMatrix();

	
}

float dish4x = getRandomFloat(minX, maxX);
float dish4z = getRandomFloat(minZ, maxZ);
void resetDish4Position() {
	dish4x = getRandomFloat(minX, maxX);
	dish4z = getRandomFloat(minZ, maxZ);
}
void drawDish4Clt(float x, float z) {
	// Draw plate (a flat circle)
	glPushMatrix();
	glTranslatef(x, dishHeight, z); // Position the dish in 3D space
	//glColor3f(1.0f, 1.0f, 1.0f); // White color for plate (you can change this)
	glRotated(cRot, 0, 1, 0);

	glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate to make it flat
	//glScaled(3, 3, 3);
	glScaled(2, 2, 2);

	rice.Draw();
	glPopMatrix();

	
}

float Specialdishx = 0;
float Specialdishy = 2;
float Specialdishz = 0;
void resetDishSPosition() {
	Specialdishx = getRandomFloat(minX, maxX);
	Specialdishz = getRandomFloat(minZ, maxZ);
}
void drawDishSpecial(float x, float z) {
	// Draw plate (a flat circle)
	glPushMatrix();
	glTranslatef(x, Specialdishy, z); // Position the dish in 3D space
	glTranslated(-6, 0, 1);
	//glColor3f(1.0f, 1.0f, 1.0f); // White color for plate (you can change this)
	//glRotated(cRot, 0, 1, 0);
	//glTranslated(6, 0, -1);
	glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate to make it flat
	//glScaled(3, 3, 3);

	dango.Draw();
	glPopMatrix();


}


float pplX = getRandomFloat(minX, maxX);
float pplZ = getRandomFloat(minZ, maxZ);

void resetpplPosition() {
	pplX = getRandomFloat(minX, maxX);
	pplZ = getRandomFloat(minZ, maxZ);
}
void drawppl() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(pplX, 0, pplZ);
	glScaled(0.03, 0.03, 0.03);
	//glRotated(180, 0, 1, 0);

	glRotated(90, 1, 0, 0);

	woman.Draw();

	glPopMatrix();
}

float ppl1X = getRandomFloat(minX, maxX);
float ppl1Z = getRandomFloat(minZ, maxZ);

void resetppl1Position() {
	ppl1X = getRandomFloat(minX, maxX);
	ppl1Z = getRandomFloat(minZ, maxZ);
}
void drawppl1() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(ppl1X, 0, ppl1Z);
	glScaled(0.03, 0.03, 0.03);
	//glRotated(180, 0, 1, 0);

	glRotated(90, 1, 0, 0);

	woman.Draw();

	glPopMatrix();
}
float ppl2X = getRandomFloat(minX, maxX);
float ppl2Z = getRandomFloat(minZ, maxZ);

void resetppl2Position() {
	ppl2X = getRandomFloat(minX, maxX);
	ppl2Z = getRandomFloat(minZ, maxZ);
}
void drawppl2() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(ppl2X, 0, ppl2Z);
	glScaled(0.008, 0.008, 0.008);
	//glRotated(180, 0, 1, 0);

	glRotated(90, 1, 0, 0);

	farmer.Draw();

	glPopMatrix();
}

float ppl3X = getRandomFloat(minX, maxX);
float ppl3Z = getRandomFloat(minZ, maxZ);

void resetppl3Position() {
	ppl3X = getRandomFloat(minX, maxX);
	ppl3Z = getRandomFloat(minZ, maxZ);
}
void drawppl3() {
	//glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(ppl3X, 0, ppl3Z);
	glScaled(0.008, 0.008, 0.008);
	//glRotated(180, 0, 1, 0);

	glRotated(90, 1, 0, 0);

	farmer.Draw();

	glPopMatrix();
}

float tableX = 0;
float tableZ = 4;

void resettablePosition() {
	tableX = getRandomFloat(minX, maxX);
	tableZ = getRandomFloat(minZ, maxZ);
}
void drawtable() {
	glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(tableX, 0, tableZ);
	glScaled(2.5, 2.5, 2.5);
	glRotated(90, 1, 0, 0);
	table.Draw();
	glPopMatrix();
}

float table1X = 5;
float table1Z = 5;

void resettable1Position() {
	table1X = getRandomFloat(minX, maxX);
	table1Z = getRandomFloat(minZ, maxZ);
}
void drawtable1() {
	glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(table1X, 0, table1Z);
	glScaled(2.5, 2.5, 2.5);
	glRotated(90, 1, 0, 0);
	table.Draw();
	glPopMatrix();
}

float table2X = -5;
float table2Z = 7;

void resettable2Position() {
	table2X = getRandomFloat(minX, maxX);
	table2Z = getRandomFloat(minZ, maxZ);
}
void drawtable2() {
	glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(table2X, 0, table2Z);
	glScaled(2.5, 2.5, 2.5);
	glRotated(90, 1, 0, 0);
	table.Draw();
	glPopMatrix();
}

float table3X = -5;
float table3Z = -2;

void resettable3Position() {
	table3X = getRandomFloat(minX, maxX);
	table3Z = getRandomFloat(minZ, maxZ);
}
void drawtable3() {
	glColor3d(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslated(table3X, 0, table3Z);
	glScaled(2.5, 2.5, 2.5);
	glRotated(90, 1, 0, 0);
	table.Draw();
	glPopMatrix();
}




void Display(void)
{
	glPushMatrix();
	glRotated(sunRot, 0, 0, 1);
	InitSunMoon();
	glPopMatrix();
	InitLightSources();
	if (hp == 0) {
		win = false;
	}
	else
		win = true;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };//light intensity (how bright)
	//GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };//light position (position of light source in the space)
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);//set position of light source 0
	//glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);//set intensity of light source 0

	if (!win) {//time ran out before collecting goal(GAME LOSE)
		setupCamera();
		camera.frontV();
		glColor3f(1, 0, 0);
		print(1.5, 0.5, 0, GLUT_BITMAP_HELVETICA_18, "GAME OVER");
		glColor3f(0, 0, 0);
		int ss = score;
		std::string s1 = std::to_string(ss % 10);
		ss = ss / 10;
		std::string s2 = std::to_string(ss % 10);
		ss = ss / 10;
		std::string s3 = std::to_string(ss % 10);
		ss = ss / 10;
		std::string s4 = std::to_string(ss % 10);
		std::string s = s4 + s3 + s2 + s1;
		print(1.5, 0, 0, GLUT_BITMAP_HELVETICA_18, "Score");
		print(1.5, -0.5, 0, GLUT_BITMAP_HELVETICA_18, s);
		times1 = 0;
		times2 = 0;
	}
	else if (end && win) {//collected goal(GAME WIN)
		setupCamera();
		camera.frontV();
		glColor3f(0, 1, 0);
		print(1.5, 0.5, 0, GLUT_BITMAP_HELVETICA_18, "YOU WIN");
		glColor3f(0, 0, 0);
		int ss = score;
		std::string s1 = std::to_string(ss % 10);
		ss = ss / 10;
		std::string s2 = std::to_string(ss % 10);
		ss = ss / 10;
		std::string s3 = std::to_string(ss % 10);
		ss = ss / 10;
		std::string s4 = std::to_string(ss % 10);
		std::string s = s4 + s3 + s2 + s1;
		print(1.5, 0, 0, GLUT_BITMAP_HELVETICA_18, "Score");
		print(1.5, -0.5, 0, GLUT_BITMAP_HELVETICA_18, s);
	}
	else if (lvl == 1) {//level 1
		// Draw Ground
		setupCamera();
		RenderGround();
	

		// Draw Tree Model
		glPushMatrix();
		glTranslatef(8, 0, -8);
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();

		glPushMatrix();
		glTranslated(-8, 0, -8);
		glScaled(40, 40, 40);

		glRotated(90, 0, 1, 0);
		glRotated(90, 1, 0, 0);
		model_barn.Draw();
		glPopMatrix();
		
		if (day) {
			//sky box
			glPushMatrix();

			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			glTranslated(0, 0, 0);
			glRotated(90, 1, 0, 1);
			glBindTexture(GL_TEXTURE_2D, tex);
			gluQuadricTexture(qobj, true);
			gluQuadricNormals(qobj, GL_SMOOTH);
			gluSphere(qobj, 70, 100, 100);
			gluDeleteQuadric(qobj);

			glPopMatrix();
		}
		else {
			//sky box
			glPushMatrix();

			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			glTranslated(0, 0, 0);
			glRotated(90, 1, 0, 1);
			glBindTexture(GL_TEXTURE_2D, tex2);
			gluQuadricTexture(qobj, true);
			gluQuadricNormals(qobj, GL_SMOOTH);
			gluSphere(qobj, 70, 100, 100);
			gluDeleteQuadric(qobj);

			glPopMatrix();
		}

		//collectibles
		drawBall(ballx, ballz);
		drawBall1(ball1x, ball1z);
		drawVeg(vegx, vegz);
		drawVeg1(veg1x, veg1z);
		drawFruit(fruitx, fruitz);
		drawFruit1(fruit1x, fruit1z);
		drawFruitSpecial(Specialfruitx, Specialfruitz);

		//obstacles
		drawAnimal();
		drawAnimal1();
		drawAnimal2();
		drawAnimal3();
		drawAnimal4();

		drawfence();
		drawfence1();
		drawfence2();
		drawfence3();
		drawfence4();
		drawfence5();
		drawfence6();





		//collectibles collision
		if (times1>0 == true && checkCollision(pX, pZ, ballx, ballz, ballRadius, playerRadius)) {
			resetBallPosition();
			score += 10;
			cltSnd.play();
		}

		if (times1 > 0 == true && checkCollision(pX, pZ, ball1x, ball1z, ballRadius, playerRadius)) {
			resetBall1Position();
			score += 10;
			cltSnd.play();
		}

		if (times1 > 0 == true && checkCollision(pX, pZ, fruitx, fruitz, ballRadius, playerRadius)) {
			resetFruitPosition();
			score += 10;
			cltSnd.play();
		}

		if (times1 > 0 == true && checkCollision(pX, pZ, fruit1x, fruit1z, ballRadius, playerRadius)) {
			resetFruit1Position();
			score += 10;
			cltSnd.play();
		}

		if (times1 > 0 == true && checkCollision(pX, pZ, vegx, vegz, ballRadius, playerRadius)) {
			resetVegPosition();
			score += 10;
			cltSnd.play();
		}

		if (times1 > 0 == true && checkCollision(pX, pZ, veg1x, veg1z, ballRadius, playerRadius)) {
			resetVeg1Position();
			score += 10;
			cltSnd.play();
		}

		//obstacle collision
		if (times1 > 0 == true && checkCollision(pX, pZ, chickX, chickZ, animRadius, playerRadius) && pY == 0) {
			resetanimalPosition();
			hp -= 1;
			dogSnd.play();
		}

		if (times1 > 0 == true && checkCollision(pX, pZ, dogX, dogZ, animRadius, playerRadius) && pY == 0) {
			resetanimal1Position();
			hp -= 1;

			dogSnd.play();
		}

		if (times1 > 0 == true && checkCollision(pX, pZ, duckX, duckZ, animRadius, playerRadius) && pY == 0) {
			resetanimal2Position();
			hp -= 1;

			duckSnd.play();
		}

		if (times1 > 0 == true && checkCollision(pX, pZ, duck2X, duck2Z, animRadius, playerRadius) && pY == 0) {
			resetanimal3Position();
			hp -= 1;

			duckSnd.play();
		}
		if (times1 > 0 == true && checkCollision(pX, pZ, dog2X, dog2Z, ballRadius, playerRadius) && pY == 0) {
			resetanimal4Position();
			hp -= 1;

			dogSnd.play();
		}
		//boundaries
		if (times1 > 0 == true && checkfCollision(pZ,fenceZ,ballRadius,playerRadius) ) {
			pZ -= 0.1;
			collideSnd.play();
		}
		if (times1 > 0 == true && checkfCollision(pZ, fence1Z, ballRadius, playerRadius) ) {
			pZ += 0.1;
			collideSnd.play();
		}
		if (times1 > 0 == true && checkfCollision(pX, fence2X, ballRadius, playerRadius) ) {
			pX += 0.1;
			collideSnd.play();
		}
		if (times1 > 0 == true && checkfCollision(pX, fence3X, ballRadius, playerRadius) ) {
			pX -= 0.1;
			collideSnd.play();
		}
		//obstacle fences
		if (times1 > 0 == true && checkfCollision(pX, fence4X, ballRadius, playerRadius) ) {
			if (pZ < 10 && pZ>5) {
				if (checkfCollision(pX, fence4X, ballRadius, playerRadius)) {
					if(pX < fence4X)
						pX -= 0.1;
					else
						pX += 0.1;
					collideSnd.play();
				}
			}
		}
		if (times1 > 0 == true && checkfCollision(pX, fence5X, ballRadius, playerRadius) ) {
			if (pZ < 10 && pZ>5) {
				if (checkfCollision(pX, fence5X, ballRadius, playerRadius)) {
					if (pX < fence5X)
						pX -= 0.1;
					else
						pX += 0.1;
					collideSnd.play();
				}
			}
		}
		if (times1 > 0 == true && checkfCollision(pX, fence6X, ballRadius, playerRadius) ) {
			if (pZ < -6 && pZ>-10) {
				if (checkfCollision(pX, fence6X, ballRadius, playerRadius)) {
					if (pX < fence6X)
						pX -= 0.1;
					else
						pX += 0.1;
					collideSnd.play();
				}
			}
		}
		if (times1 > 0 == true && checkCollision(pX, pZ, Specialfruitx, Specialfruitz, ballRadius, playerRadius)&& Specialfruity == 0.5) {
			Specialfruity = -5;
			score += 100;
			cltSnd.play();
		}


		glPushMatrix();
		glTranslated(pX, pY, pZ);
		glRotated(pR, 0, 1, 0);
		glRotated(90, 0, 1, 0);
		glRotated(90, 1, 0, 0);
		glScaled(0.008, 0.008, 0.008);
		glDisable(GL_COLOR_MATERIAL);
		farmer.Draw();
		glEnable(GL_COLOR_MATERIAL);
		glPopMatrix();
	}

	else {//level 2
		setupCamera();
		RenderFloor();

		if (day2) {
			//sky box
			glPushMatrix();

			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			glTranslated(0, 0, 0);
			glRotated(90, 1, 0, 1);
			glBindTexture(GL_TEXTURE_2D, tex);
			gluQuadricTexture(qobj, true);
			gluQuadricNormals(qobj, GL_SMOOTH);
			gluSphere(qobj, 70, 100, 100);
			gluDeleteQuadric(qobj);

			glPopMatrix();
		}
		else {
			//sky box
			glPushMatrix();

			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			glTranslated(0, 0, 0);
			glRotated(90, 1, 0, 1);
			glBindTexture(GL_TEXTURE_2D, tex2);
			gluQuadricTexture(qobj, true);
			gluQuadricNormals(qobj, GL_SMOOTH);
			gluSphere(qobj, 70, 100, 100);
			gluDeleteQuadric(qobj);

			glPopMatrix();
		}
		
		//collectibles
		drawDishClt(dishx, dishz);
		drawDish1Clt(dish1x, dish1z);
		drawDish2Clt(dish2x, dish2z);
		drawDish3Clt(dish3x, dish3z);
		drawDish4Clt(dish4x, dish4z);
		drawDishSpecial(Specialdishx, Specialdishz);

		//obstacles
		drawppl();
		drawppl1();
		drawppl2();
		drawppl3();

		drawtable();
		drawtable1();
		drawtable2();
		drawtable3();

		/*drawfence();
		drawfence1();
		drawfence2();
		drawfence3();*/

		glPushMatrix();
		glTranslated(pX, pY, pZ);
		glRotated(pR, 0, 1, 0);
		glRotated(90, 0, 1, 0);
		glRotated(90, 1, 0, 0);
		glScaled(0.008, 0.008, 0.008);
		glDisable(GL_COLOR_MATERIAL);
		waiter.Draw();
		glEnable(GL_COLOR_MATERIAL);
		glPopMatrix();

		if (times2 > 0 == true && checkCollision(pX, pZ, dishx, dishz, ballRadius, playerRadius)) {
			resetDishPosition();
			score += 10;
			cltSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, dish1x, dish1z, ballRadius, playerRadius)) {
			resetDish1Position();
			score += 10;
			cltSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, dish2x, dish2z, ballRadius, playerRadius)) {
			resetDish2Position();
			score += 10;
			cltSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, dish3x, dish3z, ballRadius, playerRadius)) {
			resetDish3Position();
			score += 10;
			cltSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, dish4x, dish4z, ballRadius, playerRadius)) {
			resetDish4Position();
			score += 10;
			cltSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, Specialdishx, Specialdishz, ballRadius, playerRadius)&& Specialdishy == 0.5) {
			Specialdishy = -5;
			score += 100;
			cltSnd.play();
		}

		//obstacles collision
		if (times2 > 0 == true && checkCollision(pX, pZ, pplX, pplZ, ballRadius, playerRadius)&&pY==0) {
			resetpplPosition();
			hp -= 1;
			wmanSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, ppl1X, ppl1Z, ballRadius, playerRadius) && pY == 0) {
			resetppl1Position();
			hp -= 1;
			wmanSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, ppl2X, ppl2Z, ballRadius, playerRadius) && pY == 0) {
			resetppl2Position();
			hp -= 1;
			manSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, ppl3X, ppl3Z, ballRadius, playerRadius) && pY == 0) {
			resetppl3Position();
			hp -= 1;
			manSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX,pZ,tableX,tableZ,1,playerRadius) && pY == 0) {
			resettablePosition();
			hp -= 1;
			collideSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, table1X, table1Z, 1, playerRadius) && pY == 0) {
			resettable1Position();
			hp -= 1;
			collideSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, table2X, table2Z, 1, playerRadius) && pY == 0) {
			resettable2Position();
			hp -= 1;
			collideSnd.play();
		}
		if (times2 > 0 == true && checkCollision(pX, pZ, table3X, table3Z, 1, playerRadius) && pY == 0) {
			resettable3Position();
			hp -= 1;
			collideSnd.play();
		}

		//boundaries
		if (times2 > 0 == true && checkfCollision(pZ, fenceZ, ballRadius, playerRadius)) {
			pZ -= 0.1;
			collideSnd.play();
		}
		if (times2 > 0 == true && checkfCollision(pZ, fence1Z, ballRadius, playerRadius)) {
			pZ += 0.1;
			collideSnd.play();
		}
		if (times2 > 0 == true && checkfCollision(pX, fence2X, ballRadius, playerRadius)) {
			pX += 0.1;
			collideSnd.play();
		}
		if (times2 > 0 == true && checkfCollision(pX, fence3X, ballRadius, playerRadius)) {
			pX -= 0.1;
			collideSnd.play();
		}

	}
	
	//top border
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);

	if (!end && win) {
		setupCamera2();
		drawTopBorder();
		health();
		if(lvl == 1)
			printTime(times1);
		else
			printTime(times2);
		printScore();
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	glMatrixMode(GL_MODELVIEW);

	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================

void Keyboard(unsigned char button, int x, int y)
{
	float d = 0.1;
	float a = 1.0;

	switch (button)
	{
	//case 'w'://change view mode to wires using w key
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	break;
	//case 'r'://change view mode back to solid using r key
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//	break;
	//case 27://close the window using esc key
	//	exit(0);
	//	break;
	case 'q'://move cam up
		camera.moveY(d);
		break;
	case 'e'://move cam down
		camera.moveY(-d);
		break;
	case 'a'://move cam right
		camera.moveX(d);
		break;
	case 'd'://move cam left
		camera.moveX(-d);
		break;
	case 'w'://move cam forward
		camera.moveZ(d);
		break;
	case 's'://move cam backward 
		camera.moveZ(-d);
		break;
	case GLUT_KEY_ESCAPE://close game
		exit(EXIT_SUCCESS);
		break;
	case 't'://get top view
		camera.topV();
		break;
	case 'f'://get front view
		camera.frontV();
		break;
	case 'h':
		camera.opfrontV();
		break;
	case 'g'://get side view
		camera.sideV();
		break;
	case 'b'://get opposite of side view
		camera.backV();
		break;
	case 'i'://rotate camera up
		camera.rotateX(a);
		break;
	case 'k'://rotate camera down
		camera.rotateX(-a);
		break;
	case 'j'://rotate camera right
		camera.rotateY(a);
		break;
	case 'l'://rotate camera left
		camera.rotateY(-a);
		break;
	case 'm'://third person view
		camera.center = { (float)pX ,1.8f, -1000.0f };
		camera.thirdPerson();
		break;
	case 'n'://first person view
		camera.firstPerson();
		break;
	case GLUT_KEY_SPACE://character jumping button
		if (!jump && pY == 0) {
			pY = 1;
			jump = true;
			jumpSnd.play();
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {


	switch (key) {
	case GLUT_KEY_UP://move character forward +z
			pZ -= 0.1;
			if (thirdP) {
				camera.moveZ(0.01);
				camera.setEye(pX, camera.eye.y, pZ + 2.5);
				camera.setCenter(pX, 1.8, -1000.0);
			}
			else if (firstP) {//rotate camera with player if first person view
				camera.moveZ(0.01);
				camera.setCenter(pX, 1.3, -1000);
				camera.setEye(pX, camera.eye.y, pZ - 0.1);
			}
		pR = 90;
		if (!moving) {
			move.play();
			move.setLoop(true);
			moving = true;
		}
		break;
	case GLUT_KEY_DOWN://move character backward -z
			pZ += 0.1;
			if (thirdP) {
				camera.moveZ(-0.1);
				camera.setEye(pX, camera.eye.y, pZ + 2.5);
				camera.setCenter(pX, 1.8, -1000.0);

			}
			else if (firstP) {//rotate camera with player if first person view
				camera.moveZ(-0.1);
				//camera.rotateX();
				camera.setCenter(pX, 1.3, 1000);
				camera.setEye(pX, camera.eye.y, pZ + 0.1);

			}
		pR = 270;
		if (!moving) {
			move.play();
			move.setLoop(true);
			moving = true;
		}

		break;
	case GLUT_KEY_LEFT://move character left +x
			pX -= 0.1;
			if (thirdP) {
				camera.moveX(0.1);
				camera.setEye(pX, camera.eye.y, pZ + 2.5);
				camera.setCenter(pX, 1.8, -1000.0);

			}
			else if (firstP) {//rotate camera with player if first person view
				camera.moveZ(0.1);
				camera.setCenter(-1000, camera.eye.y, pZ);
				camera.setEye(pX - 0.1, camera.eye.y, pZ);
			}
		pR = 180;
		if (!moving) {
			move.play();
			move.setLoop(true);
			moving = true;
		}
		break;
	case GLUT_KEY_RIGHT://move character right -x
			pX += 0.1;
			if (thirdP) {
				camera.moveX(-0.1);
				camera.setEye(pX, camera.eye.y, pZ + 2.5);
				camera.setCenter(pX, 1.8, -1000.0);

			}
			else if (firstP) {//rotate camera with player if first person view
				camera.moveZ(-0.1);
				camera.setCenter(1000, camera.eye.y, pZ);
				camera.setEye(pX + 0.1, camera.eye.y, pZ);
			}
		pR = 360;
		if (!moving) {
			move.play();
			move.setLoop(true);
			moving = true;
		}
		break;

	}

	glutPostRedisplay();
}
void speUp(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP://move character forward +z
		if (moving) {
			moving = false;
			move.stop();
		}
		break;
	case GLUT_KEY_DOWN://move character backward -z
		if (moving) {
			moving = false;
			move.stop();
		}
		break;
	case GLUT_KEY_LEFT://move character right +x
		if (moving) {
			moving = false;
			move.stop();
		}
		break;
	case GLUT_KEY_RIGHT://move character left -x
		if (moving) {
			moving = false;
			move.stop();
		}
		break;
	}
	glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void Motion(int x, int y)//move the camera while zooming
{
	//y = HEIGHT - y;

	//if (cameraZoom - y > 0)
	//{
	//	camera.eye.x += -0.1;
	//	camera.eye.z += -0.1;
	//}
	//else
	//{
	//	camera.eye.x += 0.1;
	//	camera.eye.z += 0.1;
	//}

	//cameraZoom = y;

	//glLoadIdentity();	//Clear Model_View Matrix
	//camera.look();
	////gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	//GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void actM(int button, int state, int x, int y)//use mouse to zoom into scene
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (lvl ==1 && times1 > 0 == true && checkCollision(pX, pZ, Specialfruitx, Specialfruitz, 5, playerRadius)) {
			Specialfruity = 0.5;
		}
		else if (times2 > 0 == true && checkCollision(pX, pZ, Specialdishx, Specialdishz, 5, playerRadius)) {
			Specialdishy = 0.5;
		}
	}
	glutPostRedisplay();
}

void passM(int x, int y)//passive motion function takes 2 parameters the x and y positions of the mouse
//it allows the user to make use of the mouse motion without clicking on the mouse buttons
{
	//y = HEIGHT - y;
	//x = WIDTH - x;
	float d = 0.01;
	float a = 1.0;
	if (x > 100) {
		camera.rotateY(a);
	}
	else if (x < 100) {
		camera.rotateY(-a);
	}
	if (y > 100) {
		camera.rotateX(a);
	}
	else if (y < 100) {
		camera.rotateX(-a);
	}
	
	glutPostRedisplay();
}


//=======================================================================
// Reshape Function
//=======================================================================
void Reshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
	//gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void myLoadAssets()//loading external files
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_barn.Load("Models/barn/BARNF.3ds");
	//model_tree1.Load("Models/tree/Tree1.3ds");
	//model_tree2.Load("Models/tree/Tree1.3ds");
	//model_tree3.Load("Models/tree/Tree1.3ds");
	//model_tree4.Load("Models/tree/Tree1.3ds");
	//model_tree5.Load("Models/tree/Tree1.3ds");
	//model_tree6.Load("Models/tree/Tree1.3ds");
	//model_tree7.Load("Models/tree/Tree1.3ds");
	//model_tree8.Load("Models/tree/Tree1.3ds");
	//model_tree9.Load("Models/tree/Tree1.3ds");
	//model_tree10.Load("Models/tree/Tree1.3ds");
	//model_tree11.Load("Models/tree/Tree1.3ds");
	
	//player
	farmer.Load("models/player/farmer.3ds");
	waiter.Load("models/player/waiter.3ds");

	//lvl 1

	//collectibles
	apple.Load("models/crops/apple.3ds");
	banana.Load("models/crops/banana.3ds");
	carrot.Load("models/crops/carrot.3ds");
	pumpkin.Load("models/crops/pumpkin.3ds");

	////obstacles
	fence.Load("models/fence/fence.3ds");
	chick.Load("models/animals/chick.3ds");
	duck.Load("models/animals/duck.3ds");
	dog.Load("models/animals/dog.3ds");

	////lvl2

	////collectibles
	sushi.Load("models/food/sushi.3ds");
	dango.Load("models/food/dango.3ds");
	rice.Load("models/food/rice.3ds");
	rice2.Load("models/food/bibmp.3ds");

	////obstacles
	table.Load("models/table/table.3ds");
	woman.Load("models/woman/woman.3ds");
	wall.Load("models/wall/wall.3ds");




	// Loading texture files

	tex_ground.Load("Textures/ground.bmp");

	tex_floor.Load("Textures/box.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
	loadBMP(&tex2, "Textures/sky-stars-texture-skin-wallpaper-preview.bmp", true);
	loadBMP(&tex3, "Textures/apple.bmp", true);
	loadBMP(&tex4, "Textures/carrot.bmp", true);
	loadBMP(&tex5, "Textures/lettuce.bmp", true);
	loadBMP(&tex6, "Textures/onion.bmp", true);
	loadBMP(&tex7, "Textures/orange.bmp", true);
	loadBMP(&tex8, "Textures/strawberry.bmp", true);
	loadBMP(&tex9, "Textures/baby chick.bmp", true);
	loadBMP(&tex10, "Textures/chicken.bmp", true);
}
//count time down
void countDown(int val) {

	if (lvl == 1) {
		
		if (times1 > 0)
			times1 -= 1;
		if (times1 == 0) {
			lvl += 1;
			sunRot = 0;//reset sunlight source
			 pR = 90;//player rotation angle
			 pX = 5.0;//player x position start at 1.5 to collect should be at 2.5 collide with right fence at 2.7 collide with left fence at 0.3
			 pZ = 9;//player z position start at 4.5 to collect should be at 1.6 collide with back fence at -0.15 player goes up if <1.2
			 pY = 0;//player Height in space (for jumping)
			ex = 2.0f;
			  ey = 1.8f;
			  ez = 10.0f;
			 //camera CENTER vector:
			  cx = 2.0f;
			  cy = 1.8f;
			  cz = 0.0f;
			 //camera UP vector
			  ux = 0.0f;
			  uy = 1.0f;
			  uz = 0.0f;
			 day = true;
		}
		if (times1 == 30) {
			day = false;
		}
	}
	else {
		
		if (!resto) {
			
			resto = true;
			farmMusic.stop();
			restoMusic.play();
		}
		if (times2 > 0)
			times2 -= 1;
		if (times2 == 30) {
			day2 = false;
		}
		if (times2 == 0)
			end = true;
	}
	if (end && win) {
		restoMusic.stop();
		winner.play();
		timer = 5000;
	}
	if (end && !win) {
		restoMusic.stop();
		loser.play();
		dieSnd.play();
		timer = 5000;
	}


	glutPostRedisplay();
	glutTimerFunc(timer, countDown, 0);
}
void jumping(int val) {//handle jumping motion
	if (jump && jumpCount > 0) {
		jumpCount -= 1;
		pY = 1;
	}
	else if (jumpCount == 0 && pY > 0) {
		jump = false;
		pY -= 0.1;
	}
	else {
		jumpCount = 8;
		pY = 0;
	}
	


	glutPostRedisplay();
	glutTimerFunc(200, jumping, 0);
}
void anim() {
	cRot += 0.05;
	if (lvl == 1) {
		if (day)
			sunRot += 0.01;
		if (!day)
			sunRot -= 0.01;
		if (times1 > 30) {
			sunG = 0.740;
			sunB = 0.02;
		}
		else {
			sunR = 0.02;
			sunG = 0.03;
			sunB = 0.9;
		}
		lR += 0.05;
		lR2 -= 0.05;

	}
	else {
		if (day2)
			sunRot += 0.01;
		if (!day2)
			sunRot -= 0.01;
		if (times2 > 30) {
			sunG = 0.740;
			sunB = 0.02;
		}
		else {
			sunR = 0.02;
			sunG = 0.03;
			sunB = 0.9;
		}
		lR += 0.05;
		lR2 -= 0.05;
	}

	glutPostRedisplay();
}
//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100, 50);
	glutCreateWindow(title);

	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutSpecialUpFunc(speUp);
	//glutPassiveMotionFunc(passM);	//call the passive motion function
	//glutMotionFunc(Motion);
	glutMouseFunc(actM);
	glutReshapeFunc(Reshape);
	glutIdleFunc(anim);

	glutTimerFunc(timer, countDown, 0);//timer
	glutTimerFunc(500, jumping, 0);//timed jumps - can't jump until back on ground - can't fly

	sound_Buffer1.loadFromFile("footsteps_grass_1-6810.wav");
	move.setBuffer(sound_Buffer1);
	move.setVolume(300.0f);

	sound_Buffer2.loadFromFile("footsteps_grass_1-6810.wav");
	move2.setBuffer(sound_Buffer2);
	move2.setVolume(300.0f);

	sound_Buffer3.loadFromFile("footsteps_grass_1-6810.wav");
	move3.setBuffer(sound_Buffer3);
	move3.setVolume(300.0f);

	sound_Buffer4.loadFromFile("footsteps_grass_1-6810.wav");
	move4.setBuffer(sound_Buffer4);
	move4.setVolume(300.0f);

	sound_Buffer5.loadFromFile("success-fanfare-trumpets-6185.wav");
	winner.setBuffer(sound_Buffer5);
	winner.setVolume(10.0f);

	sound_Buffer6.loadFromFile("brass-fail-8-a-207130.wav");
	loser.setBuffer(sound_Buffer6);
	loser.setVolume(10.0f);

	sound_Buffer7.loadFromFile("jump-up-245782.wav");
	jumpSnd.setBuffer(sound_Buffer7);
	jumpSnd.setVolume(50.0f);

	sound_Buffer8.loadFromFile("item-pick-up-38258.wav");
	cltSnd.setBuffer(sound_Buffer8);
	cltSnd.setVolume(50.0f);

	sound_Buffer9.loadFromFile("duck-quack-112941.wav");
	duckSnd.setBuffer(sound_Buffer9);
	duckSnd.setVolume(50.0f);

	sound_Buffer10.loadFromFile("single-bark-of-a-dog-38780.wav");
	dogSnd.setBuffer(sound_Buffer10);
	dogSnd.setVolume(50.0f);

	sound_Buffer11.loadFromFile("male-sigh-6763.wav");
	manSnd.setBuffer(sound_Buffer11);
	manSnd.setVolume(50.0f);

	sound_Buffer12.loadFromFile("hey-36263.wav");
	wmanSnd.setBuffer(sound_Buffer12);
	wmanSnd.setVolume(50.0f);

	sound_Buffer13.loadFromFile("male-death-sound-128357.wav");
	dieSnd.setBuffer(sound_Buffer13);
	dieSnd.setVolume(50.0f);

	sound_Buffer14.loadFromFile("disapproving-male-grunt-38196.wav");
	collideSnd.setBuffer(sound_Buffer14);
	collideSnd.setVolume(50.0f);

	

	farmMusic.openFromFile("Prisma Cafe - Paper Mario Color Splash OST.ogg");
	farmMusic.setVolume(10.0f);
	farmMusic.setLoop(true);
	farmMusic.play();

	restoMusic.openFromFile("Rayman Origins Music Gourmand Land ~ Land a Chef.ogg");
	restoMusic.setVolume(8.0f);
	restoMusic.setLoop(true);
	


	Init();

	myLoadAssets();


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);



	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}