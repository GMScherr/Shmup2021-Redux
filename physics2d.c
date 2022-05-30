#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <SOIL/SOIL.h>
#include "physics2d.h"

/***************************************************VECTOR FUNCTIONS*******************************************************************/

GLfloat vectorDistance2 (vector2 vec1 , vector2 vec2){
	return sqrt(pow((vec1.x - vec2.x),2) + pow((vec1.y - vec2.y),2));
}

vector2 vectorSubtract2 (vector2 vec1 , vector2 vec2){
	vector2 resultVec;
	resultVec.x = vec1.x - vec2.x;
	resultVec.y = vec1.y - vec2.y;
	return resultVec;
}

vector2 normalizeVector (vector2 vec){
	vector2 resultVec = vec;
	GLfloat magnitude = sqrt(pow(resultVec.x,2) + pow(resultVec.y,2));
	resultVec.x = resultVec.x/magnitude;
	resultVec.y = resultVec.y/magnitude;
	return resultVec;
}

/***************************************************COLLISION FUNCTIONS*******************************************************************/

bool pointSquareCollide (vector2 point, squareObject square){
	if ((point.x > square.position.x - square.width)&&(point.x < square.position.x + square.width))
		if ((point.y > square.position.y - square.height)&&(point.y < square.position.y + square.height))
			return true;
	return false;
}

bool pointCircleCollide (vector2 point, circleObject circle){
	if (vectorDistance2(point,circle.position)<circle.radius)
		return true;
	else
		return false;
}

bool squareCollide (squareObject square1, squareObject square2){
	//Testing if the two squares are close enough to even bother doing the collision test
	if(vectorDistance2(square1.position,square2.position) > (square1.minRadius + square2.minRadius))
		return 0;
	//Testing if the upper and lower bounds of square1 are withn the upper and lower bounds of square2
	if(((square1.position.y + square1.height >= square2.position.y - square2.height)&&(square1.position.y + square1.height <= square2.position.y + square2.height))||
		((square1.position.y - square1.height >= square2.position.y - square2.height)&&(square1.position.y - square1.height <= square2.position.y + square2.height)))
	//Testing if the right and left bounds of square1 are within the right and left bounds of square2
		if(((square1.position.x + square1.width >= square2.position.x - square2.width)&&(square1.position.x + square1.width <= square2.position.x + square2.width))||
		((square1.position.x - square1.width >= square2.position.x - square2.width)&&(square1.position.x - square1.width <= square2.position.x + square2.width)))
			return true;
		else
			return false;
	else
		return false;

}

bool circleCollide (circleObject circle1, circleObject circle2){
	if (vectorDistance2(circle1.position,circle2.position) < (circle1.radius + circle2.radius))
		return true;
	else
		return false;
}

bool circleSquareCollide (circleObject circle , squareObject square){
	//First we check if the circle is too far away from the square to begin testing
	if(vectorDistance2(circle.position,square.position) > (circle.radius + square.minRadius))
		return false;
	//Second we check if the center of the circle is within the square. 
	if(pointSquareCollide(circle.position,square))
		return true;
	//Two squares are created which are used to detect circle to square edge detection. This works for all cases except circle to square corner collision
	squareObject testSquare1;
	testSquare1.height = square.height;
	testSquare1.width = square.width + circle.radius;
	testSquare1.position = square.position;
	if(pointSquareCollide(circle.position,testSquare1))
		return true;
	squareObject testSquare2;
	testSquare2.height = square.height + circle.radius;
	testSquare2.width = square.width;
	testSquare2.position = square.position;
	if(pointSquareCollide(circle.position,testSquare2))
		return true;
	//Circle to square corner collision detection. For this to return true, the circle's center must lie within one of four circles with their centers located on the corners
	vector2 cornerDirection;
	//To save time, we can create a single corner circle if we know whether the circle is above and to the right or left, or below and to the right or left
	//We store this information here. A similar approach could have been taken for the squares but it would actually be slower than just creating two squares
	if(circle.position.x >= square.position.x)
		cornerDirection.x = 1;
	else
		cornerDirection.x = -1;
	if(circle.position.y >= square.position.y)
		cornerDirection.y = 1;
	else
		cornerDirection.y = -1;
	circleObject testCircle;
	testCircle.position.x = square.position.x + square.width*cornerDirection.x;
	testCircle.position.y = square.position.y + square.height*cornerDirection.y;
	testCircle.radius = circle.radius;
	if(pointCircleCollide(circle.position,testCircle))
		return true;	 	
	//Worst case scenario : all tests are run and still return false. This happens if the circle is right next to a corner but not touching
	return false;
}


/***************************************************PHYSICS FUNCTIONS*******************************************************************/
//Here lies the corpse of the updatePosition() function. I made it so it returned a vector2 like an idiot, not realizing this is OOP and methods go in classes

/****************************************************MISCELLANEOUS******************************************************************/
void drawSquare (squareObject square1){
	glColor3f(1,0,0);
	glPushMatrix();
	glTranslatef (square1.position.x,square1.position.y,0);
	glBegin(GL_POLYGON);
	glVertex2f(-square1.width,square1.height);
	glVertex2f(square1.width,square1.height);
	glVertex2f(square1.width,-square1.height);
	glVertex2f(-square1.width,-square1.height);
	glEnd();
	glPopMatrix();
}

void drawCircle (circleObject circle){
	float ang = (2*M_PI)/numberOfSides;
	glColor3f(1,0,0);
	glPushMatrix();
	glTranslatef (circle.position.x,circle.position.y,0);
    glBegin(GL_TRIANGLE_FAN);
      glVertex2f(0,0);
      for (int i = 0;i<=numberOfSides;i++){
        glVertex2f(cos(ang*i)*circle.radius,sin(ang*i)*circle.radius);
      }
    glEnd();
    glPopMatrix();
}

void drawGameObject (gameObject gObject){
	glPushMatrix();
	glTranslatef(gObject.position.x,gObject.position.y,0);
	glRotatef(gObject.sprite.rotationAngle,0,0,1);
	glScalef(gObject.sprite.textureScale.x,gObject.sprite.textureScale.y,1);
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gObject.sprite.texture);
	glBegin(GL_POLYGON);
	//I wrote all of this esoteric shit without testing it once. Have faith in God that it works
	//Praise the lord, it did work first fucking try baby (At least for a single frame texture that is)

	//Upper Left Corner
	glTexCoord2f(gObject.sprite.textureSize.x*(gObject.sprite.currentTexture.x - 1),gObject.sprite.textureSize.y*gObject.sprite.currentTexture.y);
	glVertex2f(-gObject.sprite.width,gObject.sprite.height);

	//Upper Right Corner
	glTexCoord2f(gObject.sprite.textureSize.x*gObject.sprite.currentTexture.x,gObject.sprite.textureSize.y*gObject.sprite.currentTexture.y);
	glVertex2f(gObject.sprite.width,gObject.sprite.height);

	//Lower Right Corner
	glTexCoord2f(gObject.sprite.textureSize.x*gObject.sprite.currentTexture.x,gObject.sprite.textureSize.y*(gObject.sprite.currentTexture.y - 1));
	glVertex2f(gObject.sprite.width,- gObject.sprite.height);

	//Lower Left Corner
	glTexCoord2f(gObject.sprite.textureSize.x*(gObject.sprite.currentTexture.x - 1),gObject.sprite.textureSize.y*(gObject.sprite.currentTexture.y - 1));
	glVertex2f(- gObject.sprite.width,- gObject.sprite.height);

	glEnd();
	glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

