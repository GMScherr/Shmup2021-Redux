//Custom physics library responsible for handling generalized physics, such as collisions and object movements
//It also handles simple vector maths, such as measuring the distance between two vectors and finding their heading
//Update : a good week later it seems physics2d is handling rendering stuff as well, perhaps I should rename it to game_engine.h instead
#ifndef _PHYSICS2D_H
#define _PHYSICS2D_H

#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <SOIL/SOIL.h>

/*****************************************************DEFINES***********************************************************************/
#define numberOfSides 12 //Defines the number of sides of a circle drawn using drawCircle
#define framerate_sec 30
#define framerate_msec 1.0/framerate_sec
#define screen_width 10
#define screen_height 10
#define BACKGROUND_STATUS true
/****************************************************PHYSICS OBJECTS DEFINITIONS****************************************************/
class vector2
{
	public:
		GLfloat x;
		GLfloat y;
		vector2(){
			x = 0;
			y = 0;
		}
};

class positionalObject
{
	public:
		vector2 position;
};

//Definition : shape objects which define a circle and square, used for collision detection, and a square used solely for sprite rendering
class squareObject : public positionalObject
{
	public:		
		GLfloat width;
		GLfloat height;
		GLfloat minRadius;
		squareObject(){
			width = 1;
			height = 1;
			minRadius = sqrt(2);
		}
		//minRadius defines the radius of a circle that contains the polygon. Used to run a preliminary collision test
};

class texturedSquare : public squareObject
{
	public:
		GLuint texture;
		vector2 textureScale;
		vector2 numberOfTextures;
		vector2 currentTexture;
		vector2 textureSize;
		GLfloat rotationAngle;
		GLfloat animationTimer;
		GLfloat animationTimerAux;
		texturedSquare(){
			textureScale.x = 1;
			textureScale.y = 1;
			numberOfTextures.x = 1;
			numberOfTextures.y = 1;
			currentTexture.x = 1;
			currentTexture.y = 1;
			rotationAngle = 0;
			//The size of the texture sample area given a texture of size 1x1. I'm pretty sure OpenGL normalizes all textures to 1x1 regardless of size
			textureSize.x = 1/numberOfTextures.x;
			textureSize.y = 1/numberOfTextures.y;
			animationTimer = 0;
			animationTimerAux = 1;
		}
		//In case the texture of the object changes
		void updateTextureSizes(){
			textureSize.x = 1.0f/numberOfTextures.x;
			textureSize.y = 1.0f/numberOfTextures.y;
		}
		//Advances textures from left to right and jumps up once it reaches the end of a line
		void nextFrame(){
			currentTexture.x++;
			if(currentTexture.x > numberOfTextures.x){
				currentTexture.x = 1;
				currentTexture.y++;
			}
			if(currentTexture.y > numberOfTextures.y)
				currentTexture.y = 1;
		}
		void updateAnimation(){
			animationTimer = animationTimer - 1.0f/framerate_sec;
			if(animationTimer < 0){
				animationTimer = animationTimerAux;
				nextFrame();
			}
		}
};

class circleObject : public positionalObject
{
	public:
		GLfloat radius;
		circleObject(){
			radius = 1;
		}
};

//Definition : a game object is an object that exists within the game and may be interactable or not, such as text boxes, enemies or background textures
class gameObject : public positionalObject
{
	public:
		GLfloat velocity;
		vector2 velocityVector;
		texturedSquare sprite;
		gameObject(){
			velocity = 0/framerate_sec;
			velocityVector.x = 0;
			velocityVector.y = 0;
		}
		void updatePosition(){
			position.x = position.x + velocity*velocityVector.x;
			position.y = position.y + velocity*velocityVector.y;
		}
		void setVelocity(GLfloat newVelocity){
			velocity = newVelocity/framerate_sec;
		}
};

//Definition : a game entity exists only during gameplay and can be interacted with. The three main entities are enemies, bullets and players
class gameEntity : public gameObject
{
	public:
		int health;
		gameEntity(){
			health = 1;
		}
};

//Definition : The player class always has a circular hitbox. canFire() updates the firing cooldown to determine if the player can fire a bullet or not
class player : public gameEntity
{
	public:
		int numberOfBombs;
		int score;
		circleObject hitbox;
		GLfloat bulletCooldown;
		GLfloat bulletCooldownAux;
		player(){
			numberOfBombs = 3;
			score = 0;
			velocity = 0.4f/framerate_sec;
			health = 3;
			hitbox.position = position;
			hitbox.radius = 10;
			bulletCooldown = 0;
			bulletCooldownAux = 2.5f;
		}
		bool canFire(){
			bulletCooldown = bulletCooldown - 1.0f/framerate_sec;
			if (bulletCooldown <= 0){
				bulletCooldown = bulletCooldownAux;
				return true;
			}
			return false;
		}

};

//Definition : class for entities that need to be removed after a given amount of time : mostly enemies and bullets, be them enemy bullets or player bullets
//Update lifespan is to be triggered on every frame, returning a 1 if the lifespan has expired or 0 if not
class lifespanEntity : public gameEntity
{
	public:
		GLfloat lifespan;
		lifespanEntity(){
			lifespan = 10;
		}
		bool updateLifeSpan(){
			lifespan = lifespan - 1.0f/framerate_sec;
			if (lifespan < 0)
				return true;
			return false;
		}
};

class enemy : public lifespanEntity
{
	public:
		squareObject hitbox;
		GLfloat bulletCooldown;
		GLfloat bulletCooldownAux;
		GLuint enemyType;
		bool isActive;
		enemy(){
			isActive = false;
			enemyType = 0;
		}
		bool canFire(){
			bulletCooldown = bulletCooldown - 1.0f/framerate_sec;
			if (bulletCooldown <= 0){
				bulletCooldown = bulletCooldownAux;
				return true;
			}
			return false;
		}
		void decreaseHealth(int damage){
			health = health - damage;
		}
};

class bullet : public lifespanEntity
{
	public:
		circleObject hitbox;
};

/***************************************************VECTOR FUNCTIONS DEFINITIONS****************************************************/
GLfloat vectorDistance2 (vector2 obj1Pos , vector2 obj2Pos);
vector2 vectorSubtract2 (vector2 vec1 , vector2 vec2);
vector2 normalizeVector (vector2 vec);

/***************************************************PHYSICS FUNCTIONS DEFINITIONS***************************************************/

/***************************************************COLLISION FUNCTIONS DEFINITIONS***************************************************/
bool pointSquareCollide (vector2 point, squareObject square);
bool pointCircleCollide (vector2 point, circleObject circle);
bool squareCollide (squareObject square1, squareObject square2);
bool circleCollide (circleObject circle1, circleObject circle2);
bool circleSquareCollide (circleObject circle , squareObject square);

/****************************************************MISCELLANEOUS******************************************************************/
void drawSquare (squareObject square1);
void drawCircle (circleObject circle);
void drawGameObject (gameObject gObject);

#endif