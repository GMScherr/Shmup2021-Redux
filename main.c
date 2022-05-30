#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <list>
#include <SOIL/SOIL.h>
#include "physics2d.h"
using namespace std;
//Manual compile : g++ TP1.c -lGL -lGLU -lglut -lm -lSOIL

//Global keyboard vector
bool keyboard [127];
//Global special keyboard vector (Arrow keys and Function keys do not return char, so a special keyboard callback is needed)
bool specialKeyboard[4];
enum directionals {left = 0,up,right,down};
enum enemyType {biplane};

//Global scope lists which contain important game information. Names are self explaining
list<enemy> enemyList;
list<bullet> enemyBulletList;
list<bullet> playerBulletList;

//Textures. Loaded during the setup() and never again touched upon
GLuint biplaneTexture;
GLuint propeller;
GLuint roundBullet;
GLuint playerTexture;
GLuint wingTexture;
GLuint background1;

//Useful global objects, such as templates and the player
player Player;
bullet playerBulletTemplate;
enemy enemyBiplaneTemplate;

//Animation objects
gameObject propellerObject;
texturedSquare propellerAnimation;

gameObject wingObject;
texturedSquare wingAnimation;

gameObject backgroundObject;
gameObject backgroundObjectCopy;
texturedSquare backgroundTexture;

/*******************************************************************Pipelines**************************************************************/

void enemyPipeline(){
	auto it = enemyList.begin();
	while (it != enemyList.end()){
		if(it->updateLifeSpan()||(it->health<0)){
			it = enemyList.erase(it);
		}
		else{
			if(it->isActive){
				it->updatePosition();
				it->hitbox.position = it->position;
				drawGameObject(*it);
				switch(it->enemyType){
					case biplane:{
							propellerObject.position = it->position;
							propellerObject.position.y = propellerObject.position.y + 0.72f;
							propellerObject.sprite.updateAnimation();
							drawGameObject(propellerObject);
							break;
					}
				}
			}
			it++;
		}
	}
}

void playerBulletPipeline(){
	auto it = playerBulletList.begin();
	while (it != playerBulletList.end()){
		//Everything done to bullets goes here
		if(it->updateLifeSpan()){
			it = playerBulletList.erase(it);
		}
		else{
			it->updatePosition();
			it->hitbox.position = it->position;
			drawGameObject(*it);
			//I could just do an if(hitBoxVisible) flag but to save on if calls just uncomment the line below if you wish to view the object's hitbox
			//drawCircle(it->hitbox);
			//Check for collisions with enemy hitboxes and destroys the bullet
			auto it2 = enemyList.begin();
			while (it2 != enemyList.end()){
				if (it2->isActive)
					if (circleSquareCollide(it->hitbox,it2->hitbox)){
						it2->decreaseHealth(1);
						it = playerBulletList.erase(it);
						break;			
					}
				it2++;
			}
			//Look at this deliciously disgusting O(m*n) shit. I hope this doesn't brick the game		
			it++;
		}
	}
}

void playerKeyboard(){
	//Player Movement Inputs :
	GLfloat speedMod;
	//Z & z keys to "Focus", essentally making you slower and making your movement more precise
	if(keyboard[90]||keyboard[122])
		speedMod = 0.5;
	else
		speedMod = 1;

	if(specialKeyboard[up])
		Player.velocityVector.y = speedMod;
	else
		if(specialKeyboard[down])
			Player.velocityVector.y = -speedMod;
		else
			Player.velocityVector.y = 0;

	if(specialKeyboard[right])
		Player.velocityVector.x = speedMod;
	else
		if(specialKeyboard[left])
			Player.velocityVector.x = -speedMod;
		else
			Player.velocityVector.x = 0;

	//Player attack inputs :
	//X & x keys to fire bullets
	if(keyboard[88]||keyboard[120]){
		if(Player.canFire()){
			bullet newBullet;
			newBullet = playerBulletTemplate;
			newBullet.position = Player.position;
			newBullet.position.y = Player.position.y + 1.0f;
			playerBulletList.push_back(newBullet);
		}
	}
}

/*************************************************************************Gameloop*************************************************************************************/

//Main gameloop function. "stuff" parameter is useless fluff, glutTimerFunc forces your callback to receive an int parameter
void gameloop(int stuff){
	glClear(GL_COLOR_BUFFER_BIT);
	if(BACKGROUND_STATUS){
		drawGameObject(backgroundObject);
		backgroundObject.updatePosition();
		drawGameObject(backgroundObjectCopy);
		backgroundObjectCopy.updatePosition();
		if(backgroundObject.position.y < -backgroundTexture.height - screen_height)
			backgroundObject.position.y = backgroundObjectCopy.position.y + 2*backgroundTexture.height;
		if(backgroundObjectCopy.position.y < -backgroundTexture.height - screen_height)
			backgroundObjectCopy.position.y = backgroundObject.position.y + 2*backgroundTexture.height;

	}
	playerKeyboard();
	if(Player.position.x < -screen_width)
		Player.position.x = -screen_width;
	if(Player.position.x > screen_width)
		Player.position.x = screen_width;

	if(Player.position.y < -screen_height)
		Player.position.y = -screen_height;
	if(Player.position.y > screen_height)
		Player.position.y = screen_height;

	playerBulletPipeline();
	enemyPipeline();
	Player.updatePosition();
	Player.hitbox.position = Player.position;

	drawGameObject(Player);
	wingObject.position = Player.position;
	wingObject.sprite.updateAnimation();
	drawGameObject(wingObject);

	glutPostRedisplay();
	glutTimerFunc(1/framerate_sec,gameloop,0);
}

void drawFunc() {
	//Be warned, putting any of the drawObject calls in here does not work, so they have been moved to the gameloop instead
	//God knows why, because that's the way I did it last time.This requires further investigation
	glFlush();
}

/************************************************************************Setup******************************************************************************************/

//Code copied verbatim from my last project and, thusly, the teacher's example
GLuint loadtexture (const char* file){
	GLuint texture = SOIL_load_OGL_texture(
                           file,
                           SOIL_LOAD_AUTO,
                           SOIL_CREATE_NEW_ID,
                           SOIL_FLAG_INVERT_Y
                       );
	if (texture == 0)
		printf ("ERROR : '%s'\n",SOIL_last_result());
	return texture;
}

void setup() {
	//Keyboard initialization
	glutIgnoreKeyRepeat(1);
	for (int i = 0; i < 127;i++)
		keyboard[i] = false;
	for (int i = 0; i < 4;i++)
		specialKeyboard[i] = false;

	//Draw mode initialization
   glClearColor(1, 1, 1, 1);
   glEnable (GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   //Texture loading
   biplaneTexture = loadtexture ("Textures/EnemyBiplane.png");
   roundBullet = loadtexture ("Textures/RoundBullet.png");
   propeller = loadtexture ("Textures/BiplanePropAnimation.png");
   playerTexture = loadtexture ("Textures/BodyIdle.png");
   wingTexture = loadtexture ("Textures/WingSpritesheet.png");
   background1 = loadtexture ("Textures/BackgroundPrelim.png");

   //Template object initilization
   playerBulletTemplate.sprite.texture = roundBullet;
	playerBulletTemplate.sprite.textureScale.x = 0.2f;
	playerBulletTemplate.sprite.textureScale.y = 0.2f;
	playerBulletTemplate.setVelocity(1.5f);
	playerBulletTemplate.velocityVector.y = 1;
	playerBulletTemplate.lifespan = 100;
	playerBulletTemplate.hitbox.radius = 0.15f;

	enemyBiplaneTemplate.sprite.texture = biplaneTexture;
	enemyBiplaneTemplate.sprite.textureScale.x = 1;
	enemyBiplaneTemplate.sprite.textureScale.y = 1;
	enemyBiplaneTemplate.setVelocity(0.2f);
	enemyBiplaneTemplate.lifespan = 1000;
	enemyBiplaneTemplate.hitbox.width = 0.4f;
	enemyBiplaneTemplate.health = 5;
	enemyBiplaneTemplate.enemyType = biplane;
	enemyBiplaneTemplate.velocityVector.y = 1;
	enemyBiplaneTemplate.isActive = true;

	//Animation object initialization
	propellerAnimation.numberOfTextures.y = 9;
	propellerAnimation.updateTextureSizes();
	propellerAnimation.texture = propeller;
	propellerAnimation.textureScale.x = 1;
	propellerAnimation.textureScale.y = 0.111f;
	propellerAnimation.animationTimerAux = 0.5f;
	propellerObject.sprite = propellerAnimation;

	wingAnimation.numberOfTextures.y = 4;
	wingAnimation.updateTextureSizes();
	wingAnimation.texture = wingTexture;
	wingAnimation.animationTimerAux = 5;
	wingObject.sprite = wingAnimation;

	backgroundTexture.texture = background1;
	backgroundTexture.width = screen_width;
	backgroundTexture.height = screen_width*10;
	backgroundObject.sprite = backgroundTexture;
	backgroundObject.position.y = backgroundTexture.height - screen_height;
	backgroundObject.setVelocity(2.0f);
	backgroundObject.velocityVector.y = -1;

	backgroundObjectCopy = backgroundObject;
	backgroundObjectCopy.position.y = backgroundObject.position.y + 2*backgroundTexture.height;

	enemyList.push_back(enemyBiplaneTemplate);

   Player.sprite.texture = playerTexture;
}

/*********************************************************************************OpenGL Specific Code******************************************************************/

void reshape(int width, int height) {
   glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-screen_width, screen_width, -screen_height, screen_height, -1, 1);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboardPress (unsigned char key, int x, int y){
	keyboard[key] = true;
}

void keyboardRelease (unsigned char key, int x, int y){
	keyboard[key] = false;
}

void specialKeyboardPress (int key, int x, int y){
	switch(key)
	{
		case GLUT_KEY_UP:
		specialKeyboard[up] = true;
		break;
		case GLUT_KEY_DOWN:
		specialKeyboard[down] = true;
		break;
		case GLUT_KEY_LEFT:
		specialKeyboard[left] = true;
		break;
		case GLUT_KEY_RIGHT:
		specialKeyboard[right] =true;
		break;
	}
}

void specialKeyboardRelease (int key, int x, int y){
	switch(key)
	{
		case GLUT_KEY_UP:
		specialKeyboard[up] = false;
		break;
		case GLUT_KEY_DOWN:
		specialKeyboard[down] = false;
		break;
		case GLUT_KEY_LEFT:
		specialKeyboard[left] = false;
		break;
		case GLUT_KEY_RIGHT:
		specialKeyboard[right] =false;
		break;
	}
}


int main(int argc, char** argv) {
   glutInit(&argc, argv);

   glutInitContextVersion(1, 1);
   glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
   glutInitWindowSize(1000, 1000);
   glutInitWindowPosition(100, 100);

   glutCreateWindow("Stahlgefieder Redux");

   glutDisplayFunc(drawFunc);
   glutReshapeFunc(reshape);  
   glutKeyboardFunc(keyboardPress);
   glutKeyboardUpFunc(keyboardRelease);
   glutSpecialFunc(specialKeyboardPress);
   glutSpecialUpFunc(specialKeyboardRelease);

   setup();
   glutTimerFunc(1/framerate_sec,gameloop,0);
   glutMainLoop();
   return 0;
}