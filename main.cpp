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

DrinkList drinks;
Draw canvas;
int state = INTRO;			// the state of the interface, starts in introduction
int ingredientsSet;			// the number of the set of the ingredients
int height = 800;			// height of the display
int width = 1000;			// width of the display
double mouseposx, mouseposy;	// x and y position of the mouse
int numIngredientsDisplayed;	// the number of ingredients displayed
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
	glColor3f((GLfloat)red, (GLfloat)green, (GLfloat)blue);
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
	glColor3f((GLfloat)red, (GLfloat)green, (GLfloat)blue);
	glScalef((GLfloat)(0.0014*size), (GLfloat) (0.0016*size), (GLfloat) (0.0016*size));
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
	glColor3f((GLfloat)red, (GLfloat)green, (GLfloat)blue);
	glBegin(GL_LINES);
	glVertex2f((GLfloat)xpos1, (GLfloat)ypos1);
	glVertex2f((GLfloat)xpos2, (GLfloat)ypos2);
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
	int b, a;
	double yi;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (canvas.numMovingButtons() > 0){
		if (state != INTRO) {
			glPushMatrix();
			glTranslated(0.0, 1.0, 0.0);
			glColor3f(1.0, 0.5, 1.0);
			glRectf((GLfloat)-xline, (GLfloat)-yline, (GLfloat)xline, (GLfloat)(-yline - 0.05));
			if (canvas.numMovingButtons() != 0)
				displayText(-2.7, 3.0, 0.0, 1.0, 0.5, 0.0, "Drinks similar to ", 1.0);
			if (canvas.firstVisibleDrinkName() != "null"){
				displayText(0.0, 3.0, 0.0, 1.0, 0.5, 0.0, canvas.firstVisibleDrinkName(), 1.0);
			}
			if (drinks.numSelectedIngr()>0){
				displayText(-1.8, 2.8, 0.0, 1.0, 0.5, 0.0, "containing", 0.8);
				a = 0;
				b = 0;
				for (int i = 0; i < drinks.numSelectedIngr(); i++) {
					std::string ingrName = drinks.getSelectedIngr(i);
					displayText(2 * b - 0.3, 2.8 - (a - b)*0.1, 0.0, 1.0, 0.5, 0.0, ingrName, 0.8);
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
			if (canvas.numMovingButtons() > 0) {
				DynamicBall center = canvas.getMovingBall(0);
				displayText(-3.0 - 2.5, -3.7, 0.0, 1.0, 0.5, 0.0, canvas.firstVisibleDrinkName(), 1.0);
				a = 0;
				for (std::vector<std::string>::iterator it = center.nextText.begin();
					it != center.nextText.end(); it++) {
					displayText(-3.0 - 2.4, -3.8 - (a + 1)*0.2, 0.0, 1.0, -0.5, 0.0, *it, 0.8);
					a++;
				}
			}
			else {
				displayText(-2.0, 0.0, 0.0, 1.0, 1.0, 1.0, "No drinks with these ingredients", 1.0);
			}
			glTranslated(-1.0, 0.0, 0.0);
			// draw each ball and the text label
			for (int i=0; i<canvas.numMovingButtons(); i++){
				DynamicBall thisBall = canvas.getMovingBall(i);
				if (thisBall.posy <= -yline + 0.7) {
					yi = -yline + 0.7;
				}
				else if (thisBall.posy >= yline - 0.7) {
					yi = yline - 0.7;
				}
				else {
					yi = thisBall.posy;
				}
				displayBall(thisBall.posx, yi, 0.0, thisBall.red, thisBall.green, thisBall.blue, thisBall.size);
				if (i>0){
					displayText(thisBall.posx + thisBall.size, yi, 0.0, 1.0, 0.5, 0.0,
						thisBall.text, 0.7);
				}
				else {
					displayText(thisBall.posx + thisBall.size, yi, 0.0, 1.0, 1.0, 1.0,
						thisBall.text, 0.7);
				}
				int m = 0;
				for (std::vector<std::string>::iterator itNt = thisBall.nextText.begin();
					itNt != thisBall.nextText.end(); itNt++){
					displayText(thisBall.posx + thisBall.size, yi - (1 + m)*thisBall.size, 0.0, 1.0, 0.0, 1.0, *itNt, 0.6);
					m++;
				}
			}

			glPopMatrix();
		}
	}
	// draw the stationary buttons
	for (int i=0; i<canvas.numButtons(); i++){
		Ball thisBall = canvas.getBall(i);
		displayBall(thisBall.posx, thisBall.posy, thisBall.posz, thisBall.red, thisBall.green, thisBall.blue, thisBall.size);
		displayText(thisBall.posx + thisBall.size, thisBall.posy, thisBall.posz, thisBall.textRed, thisBall.textGreen, thisBall.textBlue, thisBall.text, thisBall.textSize);
		b = 0;
		for (std::vector<std::string>::iterator itNt = thisBall.nextText.begin(); itNt != thisBall.nextText.end(); itNt++){
			displayText(thisBall.posx + thisBall.size, thisBall.posy - (1 + b)*(thisBall.textSize / 5.5), thisBall.posz, thisBall.textRed,
				(thisBall.textGreen - 1.0), thisBall.textBlue, *itNt, (thisBall.textSize*0.8));
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
	canvas.moveBalls();
	display();
}


/*
 * void action(int ball)
 *
 * take an action depending on the state and the input
 */
void action(int ball)
{
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
			drinks.clearSelected();
			numIngredientsDisplayed = canvas.drawIngredients(ingredientsSet,drinks);
			state = INGREDIENTS;
		}
		else if (ball == canvas.next2) {
			drinks.setAllPossibleDrinks();
			canvas.setVisibleDrinks(drinks.firstDrinkName(), drinks);
			canvas.showDrinks(drinks.numPossibleDrinks()>0,false,'a',drinks);
			state = DRINKS;
		}
		else if (ball == canvas.next4) {
			drinks.setAllPossibleDrinks();
			drinks.setAlphabet();
			canvas.showAlphabet(drinks);
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
			drinks.setPossibleDrinks();
			if (drinks.numPossibleDrinks() > 0) {
				canvas.setVisibleDrinks(drinks.firstDrinkName(), drinks);
			}
			else {
				canvas.setVisibleDrinks("null", drinks);
			}
			canvas.showDrinks(drinks.numPossibleDrinks()>0,false,'a',drinks);
			state = DRINKS;
		}
		else if (ball == canvas.moreDrinks) {
			ingredientsSet = ingredientsSet + 1;
			numIngredientsDisplayed = canvas.drawIngredients(ingredientsSet,drinks);
		}
		else if (ball == canvas.lastDrinks) {
			ingredientsSet = ingredientsSet - 1;
			numIngredientsDisplayed = canvas.drawIngredients(ingredientsSet,drinks);
		}
		else if ((ball < numIngredientsDisplayed) && (ball >= 0)) {
			Ball thisBall = canvas.getBall(ball);
			if (thisBall.selected == 0) {
				drinks.addSelectedIngredient(thisBall.text);
			}
			else {
				drinks.removeSelectedIngredient(thisBall.text);
			}
			canvas.changeSelection(ball);
		}
		break;
	case DRINKS:
		if (ball == canvas.next) {				// random drink
			std::string newName = drinks.setRandomDrinkCenter();
			canvas.setVisibleDrinks(newName, drinks);
			canvas.showDrinks(drinks.numPossibleDrinks()>0,displayedLetter>=0, displayedLetter + 'a',drinks);
		}
		else if (ball == canvas.next2) {				// next drink
			if (drinks.numPossibleDrinks() > 1) {
				std::string newName = drinks.getNextDrinkName(canvas.firstVisibleDrinkName());
				if (newName != "null") {
					canvas.setVisibleDrinks(newName, drinks);
				}
			}
		}
		else if (ball == canvas.next3) {				// last drink
			if (drinks.numPossibleDrinks() > 1) {
				std::string newName = drinks.getLastDrinkName(canvas.firstVisibleDrinkName());
				if (newName != "null") {
					canvas.setVisibleDrinks(newName, drinks);
				}
			}
		}
		else if (ball == canvas.back) {				// cancel
			drinks.setAllPossibleDrinks();
			canvas.setVisibleDrinks("null",drinks);
			canvas.choicesBalls();
			displayedLetter = -1;
			state = CHOICES;
		}
		else if (ball == canvas.moreDrinks) {			// next letter
			displayedLetter = drinks.nextLetter(displayedLetter);
			drinks.setPossibleAlphabetDrinks(displayedLetter);
			canvas.setVisibleDrinks(drinks.firstDrinkName(),drinks);
			canvas.showDrinks(drinks.numPossibleDrinks() > 0, true, displayedLetter+'a', drinks);
			state = DRINKS;
		}
		else if (ball == canvas.lastDrinks) {			// last letter
			displayedLetter = drinks.lastLetter(displayedLetter);
			drinks.setPossibleAlphabetDrinks(displayedLetter);
			canvas.setVisibleDrinks(drinks.firstDrinkName(),drinks);
			canvas.showDrinks(drinks.numPossibleDrinks() > 0, true, displayedLetter+'a', drinks);
			state = DRINKS;
		}
		break;
	case ALPHABET:
		if (ball == canvas.back) {
			canvas.choicesBalls();
			state = CHOICES;
		}
		else if ((ball < (canvas.numButtons())) && (ball >= 0)) {
			displayedLetter = drinks.findLetter(drinks.numLettersUsed() - (((canvas.numButtons()) - ball) / 2));
			drinks.setPossibleAlphabetDrinks(displayedLetter);
			canvas.setVisibleDrinks(drinks.firstDrinkName(),drinks);
			canvas.showDrinks(drinks.numPossibleDrinks()>0,true,displayedLetter+ 'a',drinks);
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
			for (int i=0; i<canvas.numButtons(); i++){
				Ball thisBall = canvas.getBall(i);
				if (((thisBall.posx - thisBall.size) <= mouseposx) && ((thisBall.posx + thisBall.size) >= mouseposx)) {
					if (((thisBall.posy - thisBall.size) <= mouseposy) && ((thisBall.posy + thisBall.size) >= mouseposy)) {
						c = b;
						d = 1;
					}
				}
				b++;
			}
			if (d == 1) {
				action(c);
				display();
			}

			if (canvas.numMovingButtons() > 0) {
				// dynamic balls are visible
				for (int i=0; i<canvas.numMovingButtons(); i++){
					DynamicBall thisBall = canvas.getMovingBall(i);
					if (thisBall.posy <= -yline + 0.7) {
						yi = -yline + 0.7;
					}
					else if (thisBall.posy >= yline - 0.7) {
						yi = yline - 0.7;
					}
					else {
						yi = thisBall.posy;
					}
					if ((((thisBall.posx - thisBall.size - 1.0)) <= mouseposx)
						&& (((thisBall.posx + thisBall.size - 1.0)) >= mouseposx)) {
						if ((((yi - thisBall.size + 1.0)) <= mouseposy)
							&& (((yi + thisBall.size + 1.0)) >= mouseposy))
						{
							newName = thisBall.text;
						}
					}
				}
				if ((newName != "null") && (newName != canvas.firstVisibleDrinkName())) {
					canvas.setVisibleDrinks(newName, drinks);
					canvas.showDrinks(drinks.numPossibleDrinks() > 0, displayedLetter >= 0, displayedLetter + 'a', drinks);
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
	drinks.initDrinks();
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