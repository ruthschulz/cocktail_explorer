#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include "Dependencies/glew/glew.h"
#include "Dependencies/freeglut/freeglut.h"
#include "Drink.h"
#include "DrinkList.h"
#include "Draw.h"


// state for the finite state machine
#define INTRO			1		//the introduction state
#define CHOICES			2		//the initial choices state - how to choose the cocktail
#define INGREDIENTS		3		//choosing by ingredients state
#define DRINKS			4		//view the drinks chosen with the ingredients
#define ALLDRINKS		6		//view all of the drinks
#define ALPHABET		13		//view the alphabet
#define PI				3.1415926535897932384626433832795

DrinkList drinks1;
Draw canvas;

int state = INTRO;			// the state of the interface, starts in introduction
int ingredientsSet;			// the number of the set of the ingredients
int height = 800;			// height of the display
int width = 1000;			// width of the display
double mouseposx, mouseposy;	// x and y position of the mouse
int numIngredientsDisplayed;	// the number of ingredients displayed


//dynamic balls variables
double attrforce, attrforce1, attrforcex, attrforcey; // components of the attraction force
double dampforce;			// the damping force
double repforce, repforce1, repforcex, repforcey; // components of the repulsive force
double forcex, forcey;		// x and y components of the force on the ball
double attrtheta, attrtheta1, damptheta, reptheta, reptheta1;	// directions of the force components
double num;					// used when creating a new random number
double distance, velocity;	// used when determining the force on the ball
double k = 2;				// spring constant
double d = 2;				// damping constant
double e = 40;				// repulsion constant
double mass = 0.1f;			// mass of the balls
double timestep = 0.002f;		// timestep
double xline = 4.5f;
double yline = 3.4f;
int displayedLetter = -1;


/*
 * void displayBall(double xpos, double ypos, double zpos, double red,
 *				 double green, double blue, double size)
 *
 * displays a ball with the given information
 */
void displayBall(double xpos, double ypos, double zpos, double red,
	double green, double blue, double size)
{
	float angle;
	float i;

	const int CIRCLE_UNITS = 30;

	glPushMatrix();
	glTranslated(xpos, ypos, 0);
	glColor3f(red, green, blue);
	glBegin(GL_POLYGON);

	for (i = 0; i < CIRCLE_UNITS; i++) {
		angle = (float)(2.0 * PI * i / (float)CIRCLE_UNITS);
		glVertex2d(size * cos((double)angle), size * sin((double)angle));
	}

	glEnd();

	glPopMatrix();

}

/*
 * void displayText(double xpos, double ypos, double zpos, double red,
 *				 double green, double blue, char * text, double size)
 *
 * displays text with the given information
 */
void displayText(double xpos, double ypos, double zpos, double red,
	double green, double blue, std::string text, double size)
{
	glPushMatrix();
	// translate the origin
	glTranslated(xpos, ypos, zpos);
	glPushAttrib(GL_ENABLE_BIT);
	// set the colour
	glColor3f(red, green, blue);
	glScalef(0.0014*size, 0.0016*size, 0.0016*size);
	for (auto c : text)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, c);
	glPopAttrib();
	glPopMatrix();
}

/*
 * void drawLine (double xpos1, double ypos1, double xpos2, double ypos2,
 *			   double red, double green, double blue)
 *
 * draws a line with the given information
 */
void drawLine(double xpos1, double ypos1, double xpos2, double ypos2,
	double red, double green, double blue)
{
	glPushMatrix();
	glColor3f(red, green, blue);
	glBegin(GL_LINES);
	glVertex2f(xpos1, ypos1);
	glVertex2f(xpos2, ypos2);
	glEnd();
	glPopMatrix();
}

/*
 * void display(void)
 *
 * the display function - displays the stationary balls and text
 */
void display(void)
{
	int i, b, m, a;
	double yi;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (canvas.movingButtons.size() > 0){
		if (state != INTRO) {
			glPushMatrix();
			glTranslated(0.0, 1.0, 0.0);
			glColor3f(1.0, 0.5, 1.0);
			glRectf(-xline, -yline, xline, -yline - 0.05);

			if (canvas.movingButtons.size() != 0)
				displayText(-2.7, 3.0, 0.0, 1.0, 0.5, 0.0, "Drinks similar to ", 1.0);
			if (canvas.movingButtons.begin()->text != "null"){
				displayText(0.0, 3.0, 0.0, 1.0, 0.5, 0.0, canvas.movingButtons.begin()->text, 1.0);
			}
			if (drinks1.selectedIngr.size()>0){
				displayText(-1.8, 2.8, 0.0, 1.0, 0.5, 0.0, "containing", 0.8);
				a = 0;
				b = 0;
				for (std::set<std::string>::iterator it = drinks1.selectedIngr.begin(); it != drinks1.selectedIngr.end(); it++) {
					displayText(2 * b - 0.3, 2.8 - (a - b)*0.1, 0.0, 1.0, 0.5, 0.0, *it, 0.8);
					a++;
					if (b == 0) {
						b = 1;
					}
					else {
						b = 0;
					}
				}
			}
			else if (displayedLetter >= 0){
				displayText(-1.8, 2.8, 0.0, 1.0, 0.5, 0.0, "starting with", 0.8);
				char x = 'a' + displayedLetter;
				std::string tmp(1, x);
				displayText(-0.1, 2.8, 0.0, 1.0, 0.5, 0.0, tmp, 0.8);
			}
			if (canvas.movingButtons.size() > 0) {
				displayText(-3.0 - 2.5, -3.7, 0.0, 1.0, 0.5, 0.0, canvas.movingButtons.begin()->text, 1.0);
				a = 0;
				for (std::vector<std::string>::iterator it = canvas.movingButtons.begin()->nextText.begin();
					it != canvas.movingButtons.begin()->nextText.end(); it++) {
					displayText(-3.0 - 2.4, -3.8 - (a + 1)*0.2, 0.0, 1.0, -0.5, 0.0, *it, 0.8);
					a++;
				}
			}
			else {
				displayText(-2.0, 0.0, 0.0, 1.0, 1.0, 1.0, "No drinks with these ingredients", 1.0);
			}

			glTranslated(-1.0, 0.0, 0.0);
			// draw each ball and the text label
			for (std::vector<DynamicBall>::iterator it = canvas.movingButtons.begin();
				it != canvas.movingButtons.end(); it++) {
				if (it->posy <= -yline + 0.7) {
					yi = -yline + 0.7;
				}
				else if (it->posy >= yline - 0.7) {
					yi = yline - 0.7;
				}
				else {
					yi = it->posy;
				}
				displayBall(it->posx, yi, 0.0, it->red, it->green, it->blue, it->size);
				if (it == canvas.movingButtons.begin()){
					displayText(it->posx + it->size, yi, 0.0, 1.0, 0.5, 0.0,
						it->text, 0.7);
				}
				else {
					displayText(it->posx + it->size, yi, 0.0, 1.0, 1.0, 1.0,
						it->text, 0.7);

				}
				int m = 0;
				for (std::vector<std::string>::iterator itNt = it->nextText.begin();
					itNt != it->nextText.end(); itNt++){
					displayText(it->posx + it->size, yi - (1 + m)*it->size, 0.0, 1.0, 0.0, 1.0, *itNt, 0.6);
					m++;
				}
			}

			glPopMatrix();
		}
	}
	// draw the stationary buttons
	for (std::vector<Ball>::iterator it = canvas.buttons.begin(); it != canvas.buttons.end(); it++) {
		displayBall(it->posx, it->posy, it->posz, it->red, it->green, it->blue, it->size);
		displayText(it->posx + it->size, it->posy, it->posz, it->textRed, it->textGreen, it->textBlue, it->text, it->textSize);
		b = 0;
		for (std::vector<std::string>::iterator itNt = it->nextText.begin(); itNt != it->nextText.end(); itNt++){
			displayText(it->posx + it->size, it->posy - (1 + b)*(it->textSize / 5.5), it->posz, it->textRed, 
				(it->textGreen - 1.0), it->textBlue, *itNt, (it->textSize*0.8));
			b++;
		}
	}
	glutSwapBuffers();
}

/*
 * void moveBalls (void)
 *
 * for each of the balls, determine the new position from the forces acting
 * on the ball, and redraw the ball in this position
 */
void moveBalls(void)
{
	for (std::vector<DynamicBall>::iterator it = canvas.movingButtons.begin(); it != canvas.movingButtons.end(); it++) {
		if (it!=canvas.movingButtons.begin()){
			it->oldposx = it->posx;
			it->oldposy = it->posy;
			// initialise the repulsive force
			repforce = 0.0;
			reptheta = 0;
			// add up all of the repulsive forces
			for (std::vector<DynamicBall>::iterator it1 = canvas.movingButtons.begin();
				it1 != canvas.movingButtons.end(); it1++){
				if (it != it1){
					// first make sure that the balls are not in the same x or y position
					// if they are, add a random amount to the position (between -0.5 and 0.5)
					if (it->posx == it1->posx) {
						num = rand();
						num = (num / 32767) - 0.5;
						it->posx = it->posx + num;
					}
					if (it->posy == it1->posy) {
						num = rand();
						num = (num / 32767) - 0.5;
						it->posy = it->posy + num;
					}
					// determine the distance between the two balls
					distance = sqrt((it->posx - it1->posx) * (it->posx - it1->posx)
						+ (it->posy - it1->posy) * (it->posy - it1->posy));
					// determine the repulsive force and direction of the force
					repforce1 = e / (fabs(distance) * distance);
					reptheta1 = atan2(it->posy - it1->posy, it->posx - it1->posx);
					// determine the x and y components of the force
					repforcex = repforce * cos(reptheta) + repforce1 * cos(reptheta1);
					repforcey = repforce * sin(reptheta) + repforce1 * sin(reptheta1);
					// add to the sum of the repulsive forces so far
					repforce = sqrt((repforcex * repforcex) + (repforcey * repforcey));
					reptheta = atan2(repforcey, repforcex);
				}
			}
			// work out force, acceleration, velocity and position
			// determine attraction to center based on distance to center
			distance = sqrt((it->posx * it->posx) + (it->posy * it->posy));
			attrforce = -((k*canvas.otherAttractions[std::make_pair(canvas.movingButtons.begin()->text, it->text)])*distance);
			attrtheta = atan2(it->posy, it->posx);
			//determine attraction to other balls based on distance
			for (std::vector<DynamicBall>::iterator it1 = canvas.movingButtons.begin();
				it1 != canvas.movingButtons.end(); it1++) {
				if (it != it1) {
					if ((canvas.otherAttractions[std::make_pair(canvas.movingButtons.begin()->text, it->text)]) != 0) {
						// first make sure that the balls are not in the same x or y position
						// if they are, add a random amount to the position (between -0.5 and 0.5)
						if (it->posx == it1->posx) {
							num = rand();
							num = (num / 32767) - 0.5;
							it->posx = it->posx + num;
						}
						if (it->posy == it1->posy) {
							num = rand();
							num = (num / 32767) - 0.5;
							it->posy = it->posy + num;
						}
						// determine the distance between the two balls
						distance = sqrt((it->posx - it1->posx) * (it->posx - it1->posx)
							+ (it->posy - it1->posy) * (it->posy - it1->posy));
						// determine the repulsive force and direction of the force
						attrforce1 = -((k*canvas.otherAttractions[std::make_pair(canvas.movingButtons.begin()->text, it->text)])*distance);
						attrtheta1 = atan2(it->posy - it1->posy, it->posx - it1->posx);
						// determine the x and y components of the force
						attrforcex = attrforce * cos(attrtheta) + attrforce1 * cos(attrtheta1);
						attrforcey = attrforce * sin(attrtheta) + attrforce1 * sin(attrtheta1);
						// add to the sum of the repulsive forces so far
						attrforce = sqrt((attrforcex * attrforcex) + (attrforcey * attrforcey));
						attrtheta = atan2(attrforcey, attrforcex);
					}
				}
			}
			// determine damping force based on current velocity
			velocity = sqrt((it->velx * it->velx) + (it->vely * it->vely));
			dampforce = -(d*velocity);
			damptheta = atan2(it->vely, it->velx);
			// determine the x and y components of the force on the ball
			forcex = attrforce * cos(attrtheta) + dampforce * cos(damptheta) + repforce * cos(reptheta);
			forcey = attrforce * sin(attrtheta) + dampforce * sin(damptheta) + repforce * sin(reptheta);
			// set the force and direction for the ball
			it->force = sqrt((forcex * forcex) + (forcey * forcey));
			it->forcetheta = atan2(forcey, forcex);
			while (it->force > 1000) {
				it->force = it->force / 10;
			}
			// set the acceleration for the ball
			it->accx = it->force * cos(it->forcetheta) / it->mass;
			it->accy = it->force * sin(it->forcetheta) / it->mass;
			// set the velocity for the ball
			it->velx = it->velx + it->accx * timestep;
			it->vely = it->vely + it->accy * timestep;
			// set the new position of the ball
			it->posx = it->posx + it->velx * timestep;
			it->posy = it->posy + it->vely * timestep;
		}
		else {
			// put the center ball in the center position
			distance = sqrt((it->posx * it->posx) + (it->posy * it->posy));
			if (distance > 0.05) {
				attrforce = -(6 * k*distance);
				attrtheta = atan2(it->posy, it->posx);
				// determine the x and y components of the force on the ball
				forcex = attrforce * cos(attrtheta);
				forcey = attrforce * sin(attrtheta);
				// set the force and direction for the ball
				it->force = sqrt((forcex * forcex) + (forcey * forcey));
				it->forcetheta = atan2(forcey, forcex);
				while (it->force > 1000) {
					it->force = it->force / 10;
				}
				// set the acceleration for the ball
				it->accx = it->force * cos(it->forcetheta) / it->mass;
				it->accy = it->force * sin(it->forcetheta) / it->mass;
				// set the velocity for the ball
				it->velx = it->velx + it->accx * timestep;
				it->vely = it->vely + it->accy * timestep;
				// set the new position of the ball
				it->posx = it->posx + it->velx * timestep;
				it->posy = it->posy + it->vely * timestep;
			}
			else {
				it->posx = 0.0;
				it->posy = 0.0;
			}
		}
	}
	display();
}


/*
 * void action(int ball)
 *
 * take an action depending on the state and the input
 */
void action(int ball)
{
	int number, number1;
	switch (state) {
	case INTRO:
		if (ball == canvas.next) {
			canvas.choicesBalls();
			state = CHOICES;
		}
		break;
	case CHOICES:
		if (ball == canvas.next) {
			ingredientsSet = 0;
			drinks1.clearSelected();
			numIngredientsDisplayed = canvas.drawIngredients(ingredientsSet,drinks1.ingredients,drinks1.selectedIngr);
			state = INGREDIENTS;
		}
		else if (ball == canvas.next2) {
			drinks1.setAllPossibleDrinks();
			canvas.setVisibleDrinks(drinks1.possibleDrinks.begin()->first, drinks1.possibleDrinks);
			canvas.showDrinks(drinks1.possibleDrinks.size()>0,false,'a',drinks1.alphabetInfo);
			state = DRINKS;
		}
		else if (ball == canvas.next4) {
			drinks1.setAllPossibleDrinks();
			drinks1.setAlphabet();
			canvas.showAlphabet(drinks1.alphabetInfo);
			state = ALPHABET;
		}
		else if (ball == canvas.back) {
			canvas.openingBalls();
			state = INTRO;
		}
		break;
	case INGREDIENTS:
		if (ball == canvas.back) {	// Cancel
			canvas.choicesBalls();
			state = CHOICES;
		}
		else if (ball == canvas.next) {	// View Drinks
			drinks1.setPossibleDrinks();
			if (drinks1.possibleDrinks.size() > 0) {
				canvas.setVisibleDrinks(drinks1.possibleDrinks.begin()->first, drinks1.possibleDrinks);
			}
			else {
				canvas.setVisibleDrinks("null", drinks1.possibleDrinks);
			}
			canvas.showDrinks(drinks1.possibleDrinks.size()>0,false,'a',drinks1.alphabetInfo);
			state = DRINKS;
		}
		else if (ball == canvas.moreDrinks) {
			ingredientsSet = ingredientsSet + 1;
			numIngredientsDisplayed = canvas.drawIngredients(ingredientsSet,drinks1.ingredients,drinks1.selectedIngr);
		}
		else if (ball == canvas.lastDrinks) {
			ingredientsSet = ingredientsSet - 1;
			numIngredientsDisplayed = canvas.drawIngredients(ingredientsSet,drinks1.ingredients,drinks1.selectedIngr);
		}
		else if ((ball < numIngredientsDisplayed) && (ball >= 0)) {
			std::vector<Ball>::iterator it = canvas.buttons.begin();
			for (int i = 0; i < ball; i++)
				it++;
			if (it->selected == 0) {
				it->red = 0.0;
				it->blue = 1.0;
				it->selected = 1;
				drinks1.addSelectedIngredient(it->text);
			}
			else {
				it->red = 1.0;
				it->blue = 0.0;
				it->selected = 0;
				drinks1.removeSelectedIngredient(it->text);
			}
		}
		break;
	case DRINKS:
		if (ball == canvas.next) {				// random drink
			canvas.setRandomDrinkCenter(drinks1.possibleDrinks);
			canvas.showDrinks(drinks1.possibleDrinks.size()>0,displayedLetter>=0, displayedLetter + 'a',drinks1.alphabetInfo);
		}
		else if (ball == canvas.next2) {				// next drink
			std::cout << drinks1.possibleDrinks.size() << std::endl;
			if (drinks1.possibleDrinks.size() > 1) {
				std::cout << "next drink: current drink: " << canvas.movingButtons.begin()->text;
				std::map<std::string, Drink, cicompare>::iterator it = drinks1.possibleDrinks.find(canvas.movingButtons.begin()->text);
				if (it != drinks1.possibleDrinks.end()) {
					std::cout << "current: " << it->first;
					it++;
					if (it != drinks1.possibleDrinks.end()) {
						std::cout << "next1: " << it->first;
						canvas.setVisibleDrinks(it->first, drinks1.possibleDrinks);
					}
					else {
						std::cout << "next2: " << drinks1.possibleDrinks.begin()->first;
						canvas.setVisibleDrinks(drinks1.possibleDrinks.begin()->first, drinks1.possibleDrinks);
					}
				}
			}
		}
		else if (ball == canvas.next3) {				// last drink
			if (drinks1.possibleDrinks.size() > 1) {
				std::map<std::string, Drink, cicompare>::iterator it = drinks1.possibleDrinks.find(canvas.movingButtons.begin()->text);
				if (it != drinks1.possibleDrinks.begin()) {
					it--;
					canvas.setVisibleDrinks(it->first, drinks1.possibleDrinks);
				}
				else {
					it = drinks1.possibleDrinks.end();
					it--;
					canvas.setVisibleDrinks(it->first, drinks1.possibleDrinks);
				}
			}
		}
		else if (ball == canvas.back) {				// cancel
			drinks1.setAllPossibleDrinks();
			canvas.setVisibleDrinks("null",drinks1.possibleDrinks);
			canvas.choicesBalls();
			displayedLetter = -1;
			state = CHOICES;
		}
		else if (ball == canvas.moreDrinks) {			// next letter
			number = (displayedLetter + 1) % 26;
			while (drinks1.alphabetInfo.find(number+'a') == drinks1.alphabetInfo.end()) {
				number = (number + 1) % 26;
			}
			drinks1.setPossibleAlphabetDrinks(number);
			displayedLetter = number;
			canvas.setVisibleDrinks(drinks1.possibleDrinks.begin()->first,drinks1.possibleDrinks);
			canvas.showDrinks(drinks1.possibleDrinks.size() > 0, true, displayedLetter+'a', drinks1.alphabetInfo);
			state = DRINKS;
		}
		else if (ball == canvas.lastDrinks) {			// last letter
			number = (displayedLetter + 25) % 26;
			while (drinks1.alphabetInfo.find(number + 'a') == drinks1.alphabetInfo.end()){
				number = (number + 25) % 26;
			}
			drinks1.setPossibleAlphabetDrinks(number);
			displayedLetter = number;
			canvas.setVisibleDrinks(drinks1.possibleDrinks.begin()->first,drinks1.possibleDrinks);
			canvas.showDrinks(drinks1.possibleDrinks.size() > 0, true, displayedLetter+'a', drinks1.alphabetInfo);
			state = DRINKS;
		}
		break;
	case ALPHABET:
		if (ball == canvas.back) {
			canvas.choicesBalls();
			state = CHOICES;
		}
		else if ((ball < (canvas.buttons.size())) && (ball >= 0)) {
			number1 = drinks1.alphabetInfo.size() - (((canvas.buttons.size())-ball) / 2);
			number = drinks1.findLetter(number1);
			drinks1.setPossibleAlphabetDrinks(number-'a');
			displayedLetter = number - 'a';
			canvas.setVisibleDrinks(drinks1.possibleDrinks.begin()->first,drinks1.possibleDrinks);
			canvas.showDrinks(drinks1.possibleDrinks.size()>0,true,displayedLetter+ 'a',drinks1.alphabetInfo);
			state = DRINKS;
		}
	}
}


/*
 * void mouse(int button, int state, int x, int y)
 *
 * determine which button was pressed, and take an action
 */
void mouse(int button, int state, int x, int y)
{
	int b, c, d;
	double yi;
	std::string newName = "null";

	d = 0;
	c = 0;
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
			// determine the mouse position in the matrix of the screen
			if (width <= (height * 2)) {
				mouseposx = (((double)x * 12.0) / (double)width) - 6.0;
				mouseposy = (((((double)y * 6.0) / (double)height) - 3.0) * (-1)) * ((2 * (double)height) / (double)width);
			}
			else {
				mouseposx = ((((double)x * 12.0) / (double)width) - 6.0) * ((double)width / (2 * (double)height));
				mouseposy = ((((double)y * 6.0) / (double)height) - 3.0) * (-1);
			}
			// check whether the mouse was clicked while over one of the balls
			b = 0;
			for (std::vector<Ball>::iterator it = canvas.buttons.begin(); it != canvas.buttons.end(); it++) {
				if (((it->posx - it->size) <= mouseposx) && ((it->posx + it->size) >= mouseposx)) {
					if (((it->posy - it->size) <= mouseposy) && ((it->posy + it->size) >= mouseposy)) {
						c = b;
						d = 1;
						std::cout << c;
					}
				}
				b++;
			}
			if (d == 1) {
				action(c);
				display();
			}

			if (canvas.movingButtons.size() > 0) {
				std::cout << "moving buttons " << std::endl;
				// dynamic balls are visible
				for (std::vector<DynamicBall>::iterator it = canvas.movingButtons.begin();
					it != canvas.movingButtons.end(); it++) {
					if (it->posy <= -yline + 0.7) {
						yi = -yline + 0.7;
					}
					else if (it->posy >= yline - 0.7) {
						yi = yline - 0.7;
					}
					else {
						yi = it->posy;
					}
					if ((((it->posx - it->size - 1.0)) <= mouseposx)
						&& (((it->posx + it->size - 1.0)) >= mouseposx)) {
						if ((((yi - it->size + 1.0)) <= mouseposy)
							&& (((yi + it->size + 1.0)) >= mouseposy))
						{
							newName = it->text;
						}
					}
				}
				if ((newName != "null") && (newName != canvas.movingButtons.begin()->text)) {
					canvas.setVisibleDrinks(newName, drinks1.possibleDrinks);
					canvas.showDrinks(drinks1.possibleDrinks.size() > 0, displayedLetter >= 0, displayedLetter + 'a', drinks1.alphabetInfo);
				}
			}
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			exit(0);
			break;
		default:
			break;
		}
	}
}

/*
 * keyboard(unsigned char key, int x, int y)
 *
 * if the excape key is pressed, exit
 */
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case 32:
		action(-3);
		display();
		break;
	}
}


/*
 * void reshape (int w, int h)
 *
 * when the window is reshaped by the user, reshape the graphics
 * makes sure that the graphics is not distorted
 */
void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= (h * 2))
		glOrtho(-6.0, 6.0, -3.0*((GLdouble)h * 2) / (GLdouble)w,
			3.0*((GLdouble)h * 2) / (GLdouble)w, -10.0, 10.0);
	else
		glOrtho(-6.0*(GLdouble)w / ((GLdouble)h * 2),
			6.0*(GLdouble)w / ((GLdouble)h * 2), -3.0, 3.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/*
 * int main(int argc, char** argv)
 *
 * initialise and start main loop
 */
int main(int argc, char** argv)
{
	int numDrinks1;
	numDrinks1 = drinks1.initDrinks();
	canvas.openingBalls();
	// set the random seed
	srand((unsigned)time(NULL));
	// set up the display
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow(argv[0]);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(moveBalls);
	glutMainLoop();
	return(0);
}