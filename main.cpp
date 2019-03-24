#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include "Dependencies/glew/glew.h"
#include "Dependencies/freeglut/freeglut.h"

#define MAXINGREDIENTS	100		//the maximum number of ingredients allowed
#define MAXDRINKS		1000	//the maximum number of drinks stored
#define MAXBUTTONS		100		//the maximum number of buttons on screen
#define INTRO			1		//the introduction state
#define CHOICES			2		//the initial choices state - how to choose the cocktail
#define INGREDIENTS		3		//choosing by ingredients state
#define DRINKS			4		//view the drinks chosen with the ingredients
#define ALLDRINKS		6		//view all of the drinks
#define ALPHABET		13		//view the alphabet
#define PI				3.1415926535897932384626433832795

//define the structure for each drink
struct drink {
	char name[50];							// the name of the drink
	char ingredient[MAXINGREDIENTS][50];	// array of the ingredients
	int numIngredients;						// number of ingredients
	double amount[MAXINGREDIENTS];			// array of amounts of the ingredients
	char amountString[MAXINGREDIENTS][10];	// array of amounts of the ingredients (string)
	char textIngredients[5][60];			// ingredients in the drink to display
	int numlines;							// number of lines in the textIngredients
	int validDrink;							// 1 if valid, 0 if not valid
	int visible;							// if the drink is visible
	int ballPosition;						// position in the movingButtons array
	int newBallPosition;					// position in the newMovingButtons array
	int drinkPos;							// position in the drinks array
};

// define the structure for each ball
struct ball {
	double posx;			// x position
	double posy;			// y position
	double posz;			// z position
	double size;			// size of the ball (radius)
	double red;			// amount of red of ball
	double green;		// amount of green of ball
	double blue;			// amount of blue of ball
	double textSize;			// size of the ball (radius)
	double textRed;			// amount of red of ball
	double textGreen;		// amount of green of ball
	double textBlue;			// amount of blue of ball
	std::string text;	// the name of the drink
	char nextText[5][60];	// the ingredients of the drink
	int numlines;			// number of lines in the nextText
	int selected;		// if the ball is selected
	int drink;			// position in possible drinks array
};

// define the structure for each dynamic ball
struct dynamicball {
	double posx;			// x position
	double oldposx;		// last x position
	double posy;			// y position
	double oldposy;		// last y position
	double size;			// size of the ball (radius)
	double force;		// force on the ball
	double forcetheta;	// direction of the force
	double velx;			// x velocity
	double vely;			// y velocity
	double accx;			// x acceleration
	double accy;			// y acceleration
	double mass;			// mass of the ball
	int center;			// boolean determining if center ball ( 1 = centerball, 0 = not centerball)
	double red;			// amount of red of ball
	double green;		// amount of green of ball
	double blue;			// amount of blue of ball
	std::string text;	// the name of the drink
	char nextText[5][60];	// the ingredients of the drink
	int numlines;			// number of lines in the nextText
	int drinkPos;		// the position of the drink in the drinks array
};

// define the arrow structure
struct arrow {
	char * text;
	char * text2;
	int ingredientPos[MAXINGREDIENTS];
	double xpos;
	double ypos;
	int drinksPos[MAXDRINKS];
};

// define the option structure
struct option {
	int one;
	int two;
};

// define the ingredient structure
struct ingredient {
	int pipeNum;
	char name[50];
	double pipeAmount;
};

struct arrow arrows[4];								// the arrows in the corners of the screen
struct option options[MAXINGREDIENTS*MAXINGREDIENTS];	// array of the different options of ingredients
struct option options2[MAXINGREDIENTS*MAXINGREDIENTS];	// array of the different options of ingredients
int arrowMatrix[MAXINGREDIENTS][MAXINGREDIENTS];		// the arrow matrix - selection of options
int arrowDrinks[MAXDRINKS];								// the arrow drinks
int visibleArrowMatrix[MAXINGREDIENTS][MAXINGREDIENTS];	// the arrows that are visible
char ingredients[MAXINGREDIENTS][50];			// the array of ingredients
int selected[MAXINGREDIENTS];					// whether the ingredient is selected
char selectedIngredients[MAXINGREDIENTS][50];	// the array of selected ingredients
struct drink drinks[MAXDRINKS];					// an array of drinks
struct drink possibleDrinks[MAXDRINKS];			// an array of possible drinks
struct ball buttons[MAXBUTTONS];				// an array of balls
struct dynamicball movingButtons[MAXDRINKS];	// an array of dynamic balls
struct dynamicball newMovingButtons[MAXDRINKS]; // an array of dynamic balls
int alphabetInfo[26];							// an array of the information for each letter
double amount;				// amount of ingredients - used in init
char str[50];				// string used when reading in from files
char *str1, *stopstring;	// strings used when reading in from files
int numingredients;			// the number of ingredients
int chosenDrink;			// the number of the chosen drink
int set;					// the number of the set of the drinks
int ingredientsSet;			// the number of the set of the ingredients
int letterSet;				// the number of the set of the drinks for each letter
int letter;					// the index of the current letter
int next, back, moreDrinks, lastDrinks, next2, next3, next4, back2; // used with buttons to check which button was pressed
int numDrinksDisplayed;		// the number of drinks displayed
int numIngredientsDisplayed;	// the number of ingredients displayed
int numArrows;					// the number of arrows
int numPossibleArrows;			// the number of possible arrows
int state = INTRO;			// the state of the interface, starts in introduction
int height = 800;			// height of the display
int width = 1000;			// width of the display
int numButtons = 0;			// the number of buttons displayed
int numPossibleDrinks;		// the number of drinks in the possible drinks array
int numDrinks;				// the number of drinks total
int numLetters;				// the number of letters displayed
double mouseposx, mouseposy;	// x and y position of the mouse

//positions in the screen
double x1 = -4.5;
double x2 = -3.0;
double x3 = -1.0;
double x4 = 0.0;
double x5 = 2.0;
double yinit = 3.5;

//dynamic balls variables
int numVisibleDrinks;		// the number of drinks displayed as balls
double otherAttractions[MAXDRINKS - 1][MAXDRINKS - 1];	// the array of the attractions of all of the visible drinks
double attrforce, attrforce1, attrforcex, attrforcey; // components of the attraction force
double dampforce;			// the damping force
double repforce, repforce1, repforcex, repforcey; // components of the repulsive force
double forcex, forcey;		// x and y components of the force on the ball
double attrtheta, attrtheta1, damptheta, reptheta, reptheta1;	// directions of the force components
double num;					// used when creating a new random number
double distance, velocity;	// used when determining the force on the ball
double percent;				// value of similarity placed in the attractions array
int centerballDrinks = 0;	// the center ball in the drinks array
int centerballBalls = 0;	// the center ball in the movingButtons array
int centerDrink = 0;		// the center ball in the buttons array
int showIngredients = -1;	// the drink to display the ingredients of
double k = 2;				// spring constant
double d = 2;				// damping constant
double e = 40;				// repulsion constant
double mass = 0.1f;			// mass of the balls
double timestep = 0.002f;		// timestep
int dynButton = 0;
int dynLastButton = 0;		// if a dynamic ball was last selected
double xline = 4.5f;
double yline = 3.4f;
int randomDrink = 0;

int dynamic = 0;

char alphabet[26][5];
std::string stateText = "";
int displayedLetter = 0;
int letterState = 0;

/*
 * void initIngredients (void)
 *
 * reads in ingredients from a file (ingredients.txt)
 * stores these ingredients in an array
 * the closes the file
 * file is of the format:
 * ingredient 1
 * ...
 * final ingredient
 * end
 *
 */
void initIngredients(void)
{
	int i, index;
	int a = 0;

	std::ifstream b_file("ingredients2.txt");
	b_file.getline(str, 50, '\n');
	while (strcmp(str, "end") != 0) {
		if (a == 0) {
			strcpy_s(ingredients[a], str);
		}
		else {
			index = 0;
			for (i = 0; i < a; i++) {
				if (_stricmp(str, ingredients[i]) == 1) {
					index = i + 1;
				}
			}
			for (i = a; i > index; i--) {
				strcpy_s(ingredients[i], ingredients[i - 1]);
			}
			strcpy_s(ingredients[index], str);
		}

		b_file.getline(str, 50, '\n');
		a++;
	}
	numingredients = a;
	b_file.close();
}

/*
 * void initDrinks (void)
 *
 * reads in drinks from a file (drinks.txt)
 * stores these drinks in an array, with the ingredients and amounts
 * then closes the file
 * file is of the format:
 * name of drink
 * ingredient: amount
 * ...
 * final ingredient: amount
 * end
 * ...
 * last
 * drinks are in alphabetical order (?)
 */
void initDrinks(void)
{
	int g, a, b, i, count, index, decimal, sign, len, c;
	char *next_token = NULL;

	std::ifstream a_file("drinks.txt");
	a_file.getline(str, 50, '\n');
	g = 0;
	index = 0;
	while (strcmp(str, "last") != 0) {
		if (g == 0) {
			len = strlen(str);
			strcpy_s(drinks[g].name, str);
		}
		else {
			index = 0;
			for (i = 0; i < g; i++) {
				if (_stricmp(str, drinks[i].name) == 1) {
					index = i + 1;
				}
			}
			for (i = g; i > index; i--) {
				drinks[i] = drinks[i - 1];
			}
			len = strlen(str);
			strcpy_s(drinks[index].name, str);
		}
		drinks[index].validDrink = 1;
		for (b = 0;b < 5;b++) {
			strcpy_s(drinks[index].textIngredients[b], "");
		}
		a_file.getline(str, 50, '\n');
		a = 0;
		b = 0;
		c = 0;
		while (strcmp(str, "end") != 0) {
			str1 = strtok_s(str, ":", &next_token);
			strcpy_s(drinks[index].ingredient[a], str1);
			if (strlen(drinks[index].textIngredients[b]) > 22) {
				b++;
				c = 0;
			}
			if (c != 0) {
				strcat_s(drinks[index].textIngredients[b], ", ");
			}
			else {
				c = 1;
			}
			strcat_s(drinks[index].textIngredients[b], drinks[index].ingredient[a]);
			count = 0;
			for (i = 0; i < numingredients; i++) {
				if (strcmp(drinks[index].ingredient[a], ingredients[i]) == 0)
					count = count++;
			}
			if (count != 1)
				drinks[index].validDrink = 0;

			str1 = strtok_s(NULL, " ", &next_token);
			amount = (double)strtod(str1, &stopstring);
			drinks[index].amount[a] = amount;
			a++;
			a_file.getline(str, 50, '\n');
		}
		drinks[index].numlines = b;
		for (i = 0; i < drinks[index].numlines; i++) {
			strcat_s(drinks[index].textIngredients[i], ",");
		}
		strcpy_s(drinks[index].ingredient[a], "null");
		a_file.getline(str, 50, '\n');
		drinks[index].numIngredients = a;
		g++;
	}
	strcpy_s(drinks[g].name, "null");
	a_file.close();
	numDrinks = g;
}


/*
 * void init(void)
 *
 * initialisation fuction
 * clears the color, and sets up constants
 */
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

	x1 = -3.0;
	x2 = 2.0;
	x3 = -1.0;
	x4 = 0.0;
	x5 = 2.0;
	yinit = 3.5;

	strcpy_s(alphabet[0], "A");
	strcpy_s(alphabet[1], "B");
	strcpy_s(alphabet[2], "C");
	strcpy_s(alphabet[3], "D");
	strcpy_s(alphabet[4], "E");
	strcpy_s(alphabet[5], "F");
	strcpy_s(alphabet[6], "G");
	strcpy_s(alphabet[7], "H");
	strcpy_s(alphabet[8], "I");
	strcpy_s(alphabet[9], "J");
	strcpy_s(alphabet[10], "K");
	strcpy_s(alphabet[11], "L");
	strcpy_s(alphabet[12], "M");
	strcpy_s(alphabet[13], "N");
	strcpy_s(alphabet[14], "O");
	strcpy_s(alphabet[15], "P");
	strcpy_s(alphabet[16], "Q");
	strcpy_s(alphabet[17], "R");
	strcpy_s(alphabet[18], "S");
	strcpy_s(alphabet[19], "T");
	strcpy_s(alphabet[20], "U");
	strcpy_s(alphabet[21], "V");
	strcpy_s(alphabet[22], "W");
	strcpy_s(alphabet[23], "X");
	strcpy_s(alphabet[24], "Y");
	strcpy_s(alphabet[25], "Z");
}

/*
 * void makeButton(int position,double posx,double posy,double posz,
 *				double size,double red,double green,double blue,
 *				double textSize,	double textRed,double textGreen,
 *				double textBlue,char * text,char nextText[5][60],
 *				int selected,int drink)
 *
 * makes a button in the position indicated in the buttons array
 * with the values given
 */
void makeButton(int position, double posx, double posy, double posz,
	double size, double red, double green, double blue,
	double textSize, double textRed, double textGreen,
	double textBlue, std::string text, char nextText[5][60],
	int selected, int drink)
{
	int b;

	buttons[position].posx = posx;
	buttons[position].posy = posy;
	buttons[position].posz = posz;
	buttons[position].size = size;
	buttons[position].red = red;
	buttons[position].green = green;
	buttons[position].blue = blue;
	buttons[position].textSize = textSize;
	buttons[position].textRed = textRed;
	buttons[position].textGreen = textGreen;
	buttons[position].textBlue = textBlue;
	buttons[position].text = text;
	for (b = 0; b < 5; b++) {
		strcpy_s(buttons[position].nextText[b], nextText[b]);
	}
	buttons[position].selected = selected;
	buttons[position].drink = drink;
}


/*
 * void initBalls (void)
 *
 * initialises the stationary balls ie the buttons
 */
void initBalls(void)
{
	int a;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	std::string t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array
	int j = 0;

	for (a = 0; a < 5; a++) {
		strcpy_s(nT[a], "");
	}
	for (j = 0; j < MAXBUTTONS; j++) {
		makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	}
	next = -1;
	back = -1;
	moreDrinks = -1;
	lastDrinks = -1;
	next2 = -1;
	next3 = -1;
	next4 = -1;
	back2 = -1;
}


/*
 * void initDynamicBalls(void)
 * initialises the array of possible balls
 * places them all at the center with no velocity
 * also sets the colour of the ball
 */
void initDynamicBalls(void)
{
	int j;

	for (j = 0; j < MAXDRINKS; j++) {
		movingButtons[j].posx = 0.0;
		movingButtons[j].posy = 0.0;
		movingButtons[j].velx = 0.0;
		movingButtons[j].vely = 0.0;
		movingButtons[j].accx = 0.0;
		movingButtons[j].accy = 0.0;
		movingButtons[j].size = 0.2;
		movingButtons[j].mass = 0.1;
		movingButtons[j].center = 0;
		movingButtons[j].green = 0.0;
		movingButtons[j].blue = 0.0;
		movingButtons[j].red = 0.0;
	}
}


/*
 * void initNewDynamicBalls(void)
 * initialises the array of possible balls
 * places them all at the center with no velocity
 * also sets the colour of the ball
 */
void initNewDynamicBalls(void)
{
	int j;

	for (j = 0; j < MAXDRINKS; j++) {
		newMovingButtons[j].posx = 0.0;
		newMovingButtons[j].posy = 0.0;
		newMovingButtons[j].velx = 0.0;
		newMovingButtons[j].vely = 0.0;
		newMovingButtons[j].accx = 0.0;
		newMovingButtons[j].accy = 0.0;
		newMovingButtons[j].size = 0.2;
		newMovingButtons[j].mass = 0.1;
		newMovingButtons[j].center = 0;
		newMovingButtons[j].green = 0.0;
		newMovingButtons[j].blue = 0.0;
		newMovingButtons[j].red = 0.0;
	}
}


void clearSelected() {
	for (int i = 0; i < MAXINGREDIENTS; i++) {
		selected[i] = 0;
	}
}


/* Pauses for a specified number of milliseconds. */
void sleep(clock_t wait)
{
	clock_t goal;
	goal = wait + clock();
	while (goal > clock())
		;
}

void clearArrowMatrix(void) {
	int l, m;

	for (l = 0; l < MAXINGREDIENTS; l++) {
		for (m = 0; m < MAXINGREDIENTS; m++) {
			arrowMatrix[l][m] = 0;
		}
	}

}

void clearVisibleArrowMatrix(void) {
	int l, m;

	for (l = 0; l < MAXINGREDIENTS; l++) {
		for (m = 0; m < MAXINGREDIENTS; m++) {
			visibleArrowMatrix[l][m] = 0;
		}
	}

}

void setAlphabet(void) {
	int i;
	int count;
	int j = 0;

	for (i = 0; i < 26; i++) {
		count = 0;
		while (_strnicmp(alphabet[i], possibleDrinks[j].name, 1) == 0 &&
			(strcmp(possibleDrinks[j].name, "null") != 0)) {
			count++;
			j++;
		}
		// set count for letter
		alphabetInfo[i] = count;
	}

}


/*
 * void openingBalls (void)
 *
 * the opening screen for the interface
 */
void openingBalls(void)
{
	int a;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	std::string t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array
	int j = 0;

	for (a = 0; a < 5; a++) {
		strcpy_s(nT[a], "");
	}
	initBalls();
	x = x4;
	s = 0.7;
	r = 1.0;
	makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	next = j;
	j++;
	s = 0.0;
	r = 0.0;
	x = x4 - 0.28;
	t = "enter";
	tS = 1.0;
	makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	j++;
	x = x4 - 1.2;
	y = 2.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	t = "PipeDream";
	tS = 2.0;
	makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	j++;
	numButtons = j;
}


/*
 * void choicesBalls(void)
 *
 * the initial choices of how to choose the drink
 * ie select ingredients, view all, make your own
 */
void choicesBalls(void)
{
	int a;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	std::string t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array
	int j = 0;

	for (a = 0; a < 5; a++) {
		strcpy_s(nT[a], "");
	}
	initBalls();
	t = "View All Drinks";
	s = 0.3;
	r = 0.5;
	b = 0.5;
	x = x1;
	y = yinit - (j + 2);
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	tS = 1.0;
	makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	next2 = j;
	j++;
	y = yinit - (j + 2);
	t = "Search by ingredients";
	makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	next = j;
	j++;
	y = yinit - (j + 2);
	t = "Search by starting letter";
	makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	next4 = j;
	j++;
	t = "Back to Start";
	b = 0.5;
	r = 0.0;
	x = x1;
	y = yinit - 7.3;
	s = 0.3;
	tS = 1.0;
	makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	back = j;
	j++;

	numButtons = j;
}

/*
 * void showDrinks(void)
 *
 */
void showDrinks(void)
{
	int a;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	std::string t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array
	int j = 0;

	for (a = 0; a < 5; a++) {
		strcpy_s(nT[a], "");
	}
	initBalls();
	dynamic = 1;
	if (numPossibleDrinks > 1) {
		t = "Random";
		s = 0.25;
		r = 0.5;
		b = 0.5;
		x = x1 + 2.4;
		y = yinit - 7.7;
		tR = 1.0;
		tG = 1.0;
		tB = 1.0;
		tS = 1.0;
		makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		next = j;
		j++;
		t = "Next Drink";
		strcpy_s(nT[0], "(alphabetically)");
		s = 0.25;
		r = 0.5;
		b = 0.5;
		x = x1 + 2.4;
		y = yinit - 6.3;
		tR = 1.0;
		tG = 1.0;
		tB = 1.0;
		tS = 0.8;
		makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		next2 = j;
		j++;
		t = "Last Drink";
		strcpy_s(nT[0], "(alphabetically)");
		s = 0.25;
		r = 0.5;
		b = 0.5;
		x = x1 + 2.4;
		y = yinit - 7.0;
		tR = 1.0;
		tG = 1.0;
		tB = 1.0;
		tS = 0.8;
		makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		next3 = j;
		j++;
	}
	else {
		next = -1;
		next2 = -1;
		next3 = -1;
	}
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	tS = 0.8;
	t = "Main menu";
	strcpy_s(nT[0], "");
	b = 0.5;
	r = 0.0;
	x = x1 + 0.5;
	y = yinit - 7.3;
	s = 0.3;
	tS = 1.0;
	makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	back = j;
	j++;

	if (letterState == 1) {
		t = "Drinks starting with";
		s = 0.0;
		x = -5.8;
		y = 1.35;
		tS = 0.6;
		makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		j++;
		t = "";
		b = 0.5;
		r = 0.5;
		x = -5.0;
		y = 1.0;
		s = 0.25;
		makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		lastDrinks = j;
		j++;
		s = 0.0;
		a = displayedLetter - 1;;
		if (a == -1) {
			a = 25;
		}
		while (alphabetInfo[a] == 0) {
			a--;
			if (a == -1) {
				a = 25;
			}
		}
		t = alphabet[a];
		tS = 1.0;
		x = x - 0.1;
		makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		j++;
		t = "Drinks starting with";
		tS = 0.6;
		s = 0.0;
		x = 4.2;
		y = 1.35;
		makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		j++;
		t = "";
		s = 0.3;
		b = 0.5;
		r = 0.5;
		x = 5.0;
		y = 1.0;
		s = 0.25;
		makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		moreDrinks = j;
		j++;
		s = 0.0;
		a = displayedLetter + 1;
		if (a == 26) {
			a = 0;
		}
		while (alphabetInfo[a] == 0) {
			a++;
			if (a == 26) {
				a = 0;
			}
		}
		t = alphabet[a];
		tS = 1.0;
		x = x - 0.1;
		makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		j++;
	}
	numButtons = j;
}

void showAlphabet() {
	int j;
	int a;
	int k;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	std::string t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a = 0; a < 5; a++) {
		strcpy_s(nT[a], "");
	}
	j = 0;
	s = 0.3;
	r = 0.5;
	g = 0.0;
	b = 0.5;
	tS = 1.0;
	tR = 1.0;
	tG = 1.0;
	tB = 1.0;

	for (a = 0;a < 5;a++) {
		strcpy_s(nT[a], "");
	}
	d = 0;

	k = 0;
	for (int i = 0; i < 26; i++) {
		if (alphabetInfo[i] != 0) {
			if (k < 7) {
				x = x1 + k * 0.7;
				y = yinit - 2.0;
			}
			else if (k < 14) {
				x = x1 + (k - 7)*0.7;
				y = yinit - 3.0;
			}
			else if (k < 21) {
				x = x1 + (k - 14)*0.7;
				y = yinit - 4.0;
			}
			else {
				x = x1 + (k - 21)*0.7;
				y = yinit - 5.0;
			}
			k++;
			t = "";
			s = 0.3;
			makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
			j++;
			s = 0.0;
			t = alphabet[i];
			x = x - 0.1;
			makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
			j++;
		}
	}
	numLetters = k;

	tR = 1.0;
	tG = 1.0;
	tB = 1.0;
	t = "Main menu";
	b = 0.5;
	r = 0.0;
	x = x1;
	y = yinit - 7.3;
	s = 0.3;
	tS = 1.0;
	makeButton(j, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	back = j;
	j++;

	numButtons = j;
}

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

void drawArrows(void) {
	int i;
	double green = 0.0;
	glPushMatrix();

	for (i = 0; i < numArrows; i++) {
		glColor3d(0.7, 0.0, 0.5);
		glPushMatrix();
		glTranslated(arrows[i].xpos, arrows[i].ypos, 0.0);
		glutSolidSphere(0.3, 32, 32);
		glPopMatrix();
	}

	for (i = 0; i < numArrows; i++) {
		displayText(arrows[i].xpos, arrows[i].ypos + 0.2, 0.0, 1.0, 1.0, 1.0, "A drink with:", 0.8);
		displayText(arrows[i].xpos, arrows[i].ypos, 0.0, 1.0, 1.0, 1.0,
			arrows[i].text, 1.0);
		if (strcmp(arrows[i].text, arrows[i].text2) != 0) {
			displayText(arrows[i].xpos, arrows[i].ypos - 0.2, 0.0, 1.0, 1.0, 1.0,
				arrows[i].text2, 1.0);
		}
	}

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

	if (dynamic == 1) {
		if (state != INTRO) {
			glPushMatrix();
			glTranslated(0.0, 1.0, 0.0);
			glColor3f(1.0, 0.5, 1.0);
			glRectf(-xline, -yline, xline, -yline - 0.05);

			if (numVisibleDrinks != 0)
				displayText(-2.7, 3.0, 0.0, 1.0, 0.5, 0.0, "Drinks similar to ", 1.0);
			if (movingButtons[centerballBalls].text != "null"){
				displayText(0.0, 3.0, 0.0, 1.0, 0.5, 0.0, movingButtons[centerballBalls].text, 1.0);
			}
			displayText(-1.8, 2.8, 0.0, 1.0, 0.5, 0.0, stateText, 0.8);
			if (stateText == "containing"){
				a = 0;
				b = 0;
				while (strcmp(selectedIngredients[a], "null") != 0) {
					displayText(2 * b - 0.3, 2.8 - (a - b)*0.1, 0.0, 1.0, 0.5, 0.0, selectedIngredients[a], 0.8);
					a++;
					if (b == 0) {
						b = 1;
					}
					else {
						b = 0;
					}
				}
			}
			else if (stateText == "starting with") {
				displayText(-0.1, 2.8, 0.0, 1.0, 0.5, 0.0, alphabet[displayedLetter], 0.8);
			}
			if (numVisibleDrinks > 0) {
				displayText(x1 - 2.5, -3.7, 0.0, 1.0, 0.5, 0.0, movingButtons[centerballBalls].text, 1.0);
				for (a = 0; a < 5; a++) {
					displayText(x1 - 2.4, -3.8 - (a + 1)*0.2, 0.0, 1.0, -0.5, 0.0, movingButtons[centerballBalls].nextText[a], 0.8);
				}
			}
			else {
				displayText(-2.0, 0.0, 0.0, 1.0, 1.0, 1.0, "No drinks with these ingredients", 1.0);
			}

			glTranslated(-1.0, 0.0, 0.0);
			drawArrows();
			// draw each ball and the text label
			for (i = 0; i < numVisibleDrinks; i++) {
				if (movingButtons[i].posy <= -yline + 0.7) {
					yi = -yline + 0.7;
				}
				else if (movingButtons[i].posy >= yline - 0.7) {
					yi = yline - 0.7;
				}
				else {
					yi = movingButtons[i].posy;
				}
				displayBall(movingButtons[i].posx, yi, 0.0, movingButtons[i].red,
					movingButtons[i].green, movingButtons[i].blue,
					movingButtons[i].size);

			}
			for (i = 0; i < numVisibleDrinks; i++) {
				if (movingButtons[i].posy <= -yline + 0.7) {
					yi = -yline + 0.7;
				}
				else if (movingButtons[i].posy >= yline - 0.7) {
					yi = yline - 0.7;
				}
				else {
					yi = movingButtons[i].posy;
				}

				if (movingButtons[i].center == 1) {
					displayText(movingButtons[i].posx + movingButtons[i].size, yi, 0.0, 1.0, 0.5, 0.0,
						movingButtons[i].text, 0.7);
				}
				else {
					displayText(movingButtons[i].posx + movingButtons[i].size, yi, 0.0, 1.0, 1.0, 1.0,
						movingButtons[i].text, 0.7);

				}
				for (m = 0;m < 5;m++) {
					displayText(movingButtons[i].posx + movingButtons[i].size,
						yi - (1 + m)*movingButtons[i].size, 0.0, 1.0, 0.0, 1.0,
						movingButtons[i].nextText[m], 0.6);
				}
			}

			glPopMatrix();
		}
	}
	// draw the stationary buttons
	for (i = 0; i < numButtons; i++) {
		displayBall(buttons[i].posx, buttons[i].posy, buttons[i].posz,
			buttons[i].red, buttons[i].green, buttons[i].blue, buttons[i].size);
		displayText(buttons[i].posx + buttons[i].size, buttons[i].posy, buttons[i].posz,
			buttons[i].textRed, buttons[i].textGreen, buttons[i].textBlue,
			buttons[i].text, buttons[i].textSize);
		for (b = 0;b < 5;b++) {
			displayText(buttons[i].posx + buttons[i].size, buttons[i].posy - (1 + b)*(buttons[i].textSize / 5.5),
				buttons[i].posz, buttons[i].textRed, (buttons[i].textGreen - 1.0), buttons[i].textBlue,
				buttons[i].nextText[b], (buttons[i].textSize*0.8));
		}
	}
	glutSwapBuffers();
}

void drawIngredients(int ingrSet) {
	int i;
	int a;
	int start, remaining;
	char nT[5][60];	// the ingredients of the drink
	double x = 0.0;		// x position		
	double y = 0.0;		// y position
	double z = 0.0;		// z position
	double s = 0.0;		// size of the ball (radius)
	double r = 0.0;		// amount of red of ball
	double g = 0.0;		// amount of green of ball
	double b = 0.0;		// amount of blue of ball
	double tS = 0.0;		// size of the ball (radius)
	double tR = 0.0;		// amount of red of ball
	double tG = 0.0;		// amount of green of ball
	double tB = 0.0;		// amount of blue of ball
	std::string t = "";		// the name of the drink
	int sel = 0;		// if the ball is selected
	int d = 0;			// position in possible drinks array

	for (a = 0; a < 5; a++) {
		strcpy_s(nT[a], "");
	}
	initBalls();
	start = 20 * ingrSet;
	remaining = numingredients - start;
	moreDrinks = -1;
	lastDrinks = -1;
	if (remaining < 20) {
		numIngredientsDisplayed = remaining;
		for (i = 0; i < remaining;i++) {
			t = ingredients[i + start];
			r = 1.0;
			if (i < remaining / 2) {
				x = x1;
				y = yinit - (i + 2)*0.5;
			}
			else {
				x = x2;
				y = yinit - ((i + 2) - (remaining / 2))*0.5;
			}
			s = 0.2;
			if (selected[i + start] == 1) {
				r = 0.0;
				b = 1.0;
				sel = 1;
			}
			else {
				r = 1.0;
				b = 0.0;
				sel = 0;
			}
			tR = 1.0;
			tB = 1.0;
			tG = 1.0;
			tS = 0.8;
			makeButton(i, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		}
	}
	else {
		numIngredientsDisplayed = 20;
		for (i = 0; i < 20;i++) {
			t = ingredients[i + start];
			r = 1.0;
			if (i < 10) {
				x = x1;
				y = yinit - (i + 2)*0.5;
			}
			else {
				x = x2;
				y = yinit - ((i + 2) - (10))*0.5;
			}
			s = 0.2;
			if (selected[i + start] == 1) {
				r = 0.0;
				b = 1.0;
				sel = 1;
			}
			else {
				r = 1.0;
				b = 0.0;
				sel = 0;
			}
			tR = 1.0;
			tB = 1.0;
			tG = 1.0;
			tS = 0.8;
			makeButton(i, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		}

		y = yinit;
		x = x1;
		s = 0.2;
		r = 0.5;
		b = 1.0;
		tS = 1.0;
		t = "more ingredients";
		for (a = 0;a < 5;a++) {
			strcpy_s(nT[a], "");
		}
		d = 0;
		makeButton(i, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		moreDrinks = i;
	}
	if (ingredientsSet != 0) {
		i++;
		x = x1;
		y = yinit - 0.5;
		s = 0.2;
		r = 0.5;
		g = 0.0;
		b = 1.0;
		tS = 1.0;
		t = "previous ingredients";
		for (a = 0;a < 5;a++) {
			strcpy_s(nT[a], "");
		}
		d = 0;
		makeButton(i, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
		lastDrinks = i;
	}
	else {
		lastDrinks = -1;
	}
	i++;
	t = "Select ingredients";
	r = 0.0;
	x = x1;
	y = yinit + 0.5;
	s = 0.0;
	tS = 1.5;
	makeButton(i, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	i++;
	t = "View Drinks";
	b = 0.5;
	r = 0.5;
	x = x1;
	y = yinit - (1 + 12)*0.5;
	s = 0.3;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 1.0;
	makeButton(i, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	next = i;
	i++;
	t = "with selected";
	b = 0.0;
	r = 0.0;
	x = x1 + 0.5;
	y = yinit - (1 + 12)*0.5 - 0.2;
	s = 0.0;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 0.8;
	makeButton(i, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	i++;
	t = " ingredients";
	b = 0.0;
	r = 0.0;
	x = x1 + 0.5;
	y = yinit - (1 + 12)*0.5 - 0.4;
	s = 0.0;
	tR = 1.0;
	tB = 1.0;
	tG = 1.0;
	tS = 0.8;
	makeButton(i, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	i++;
	t = "Main menu";
	b = 0.5;
	x = x1;
	y = yinit - 7.3;
	s = 0.3;
	tS = 1.0;
	makeButton(i, x, y, z, s, r, g, b, tS, tR, tG, tB, t, nT, sel, d);
	back = i;
	i++;

	numButtons = i;
}


void setArrowMatrix(void) {

	int j, l, m, i;
	int check1, check2;

	clearArrowMatrix();

	for (l = 0; l < numingredients; l++) {
		for (m = l; m < numingredients; m++) {
			for (i = 0; i < numPossibleDrinks; i++) {
				check1 = 0;
				check2 = 0;
				for (j = 0; j < possibleDrinks[i].numIngredients; j++) {
					if (strcmp(possibleDrinks[i].ingredient[j], ingredients[l]) == 0) {
						check1 = 1;
					}
					else if (strcmp(possibleDrinks[i].ingredient[j], ingredients[m]) == 0) {
						check2 = 1;
					}
				}
				if ((check1 == 1) && (check2 == 1)) {
					arrowMatrix[l][m] = arrowMatrix[l][m] + 1;
				}
			}
		}
	}

	i = 0;
	for (l = 0; l < numingredients; l++) {
		for (m = l; m < numingredients; m++) {
			if (arrowMatrix[l][m] > 0) {
				options2[i].one = l;
				options2[i].two = m;
				i++;
			}
		}
	}
	numPossibleArrows = i;

}

/*
 * void setPossibleDrinks(void)
 *
 * compiles a posssible drinks array
 */
void setPossibleDrinks(void)
{
	int a, i, j, g;
	int numSelected, count;

	g = 0;
	for (i = 0; i < MAXINGREDIENTS; i++) {
		if (selected[i] == 1) {
			strcpy_s(selectedIngredients[g], ingredients[i]);
			g++;
		}
	}
	strcpy_s(selectedIngredients[g], "null");
	numSelected = g;
	j = 0;
	if (numSelected != 0) {
		g = 0;
		while (strcmp(drinks[g].name, "null") != 0) {
			if (drinks[g].validDrink == 1) {
				count = 0;
				for (i = 0; i < numSelected; i++) {
					a = 0;
					while (strcmp(drinks[g].ingredient[a], "null") != 0) {
						if (strcmp(drinks[g].ingredient[a], selectedIngredients[i]) == 0)
							count = count++;
						a++;
					}
				}
				if (count == numSelected) {
					possibleDrinks[j] = drinks[g];
					possibleDrinks[j].drinkPos = g;
					j++;
				}
			}
			g++;
		}
	}
	possibleDrinks[j] = drinks[g];
	numPossibleDrinks = j;
	setArrowMatrix();
}


/*
 * void setAllPossibleDrinks(void)
 *
 * compiles a posssible drinks array with all drinks
 */
void setAllPossibleDrinks(void)
{
	int j, g;

	g = 0;
	j = 0;
	while (strcmp(drinks[g].name, "null") != 0) {
		if (drinks[g].validDrink == 1) {
			possibleDrinks[j] = drinks[g];
			possibleDrinks[j].drinkPos = g;
			j++;
		}
		g++;
	}
	possibleDrinks[j] = drinks[g];
	numPossibleDrinks = j;
	setArrowMatrix();
}

/*
 * void setAllPossibleDrinks(void)
 *
 * compiles a posssible drinks array with all drinks
 */
void setPossibleAlphabetDrinks(int letter)
{
	int j, g;

	g = 0;
	j = 0;
	while ((_strnicmp(alphabet[letter], drinks[j].name, 1) == 1) &&
		(strcmp(drinks[j].name, "null") != 0)) {
		j++;
	}
	while ((_strnicmp(alphabet[letter], drinks[j].name, 1) == 0) &&
		(strcmp(drinks[j].name, "null") != 0)) {
		if (drinks[j].validDrink == 1) {
			possibleDrinks[g] = drinks[j];
			possibleDrinks[g].drinkPos = j;
			g++;
		}
		j++;
	}
	while (strcmp(drinks[j].name, "null") != 0) {
		j++;
	}
	possibleDrinks[g] = drinks[j];
	numPossibleDrinks = g;
	setArrowMatrix();
}


/*
 * void setVisibleDrinks(int ball)
 * sets up an array of balls that will be displayed
 * based on similarity to the center ball, which is an argument
 * for this function.
 * also sets up an array of similarities.
 */
void setVisibleDrinks(int ball, int newPos)
{
	int g, a, b, z, count, index, pos, m, check;
	double count2, number, number2, limit;
	int numb;
	int num = numingredients;

	if (ball == -1) {
		numVisibleDrinks = 0;
	}
	if (ball != centerballBalls) {
		if (ball == -1) {
			if (newPos == -1) {
				pos = 0;
				initDynamicBalls();
			}
			else {
				pos = newPos;
			}
			centerballDrinks = pos;
		}
		else {
			centerballDrinks = movingButtons[ball].drinkPos;
			pos = centerballDrinks;
		}
		numVisibleDrinks = 0;
		limit = 1.0;
		numb = 0;
		check = 0;
		while ((numb < 10) || (numb > 30)) {
			initNewDynamicBalls();
			/*
			 * Determine visible objects and attractions for first center
			 */
			g = 0;
			index = 0;
			if (strcmp(possibleDrinks[pos].name, "null") != 0)
			{
				while (strcmp(possibleDrinks[g].name, "null") != 0)
				{
					a = 0;
					count = 0;
					while (strcmp(possibleDrinks[pos].ingredient[a], "null") != 0)
					{
						b = 0;
						while (strcmp(possibleDrinks[g].ingredient[b], "null") != 0) {
							if (strcmp(possibleDrinks[g].ingredient[b], possibleDrinks[pos].ingredient[a]) == 0)
							{
								count++;
							}
							b++;
						}
						a++;
					}
					number = (double)(b - 1);	// number of ingredients for other ball
					number2 = (double)(a - 1); // number of ingredients for center ball
					count2 = (double)count;	// number of same ingredients
					if (numb > 30) {
						percent = (count2 / number + count2 / number2) / 2;
					}
					else {
						percent = count2 / number;
					}
					if (percent > limit)
					{
						if (possibleDrinks[g].visible == 1) {
							// set position to old position
							newMovingButtons[index].posx = movingButtons[possibleDrinks[g].ballPosition].posx;
							newMovingButtons[index].posy = movingButtons[possibleDrinks[g].ballPosition].posy;
							possibleDrinks[g].newBallPosition = index;
						}
						else {
							possibleDrinks[g].visible = 1;
							possibleDrinks[g].newBallPosition = index;
						}
						newMovingButtons[index].text = possibleDrinks[g].name;
						for (m = 0; m < 5; m++)
							strcpy_s(newMovingButtons[index].nextText[m], possibleDrinks[g].textIngredients[m]);
						newMovingButtons[index].drinkPos = g;
						newMovingButtons[index].red = (double)possibleDrinks[newMovingButtons[index].drinkPos].numIngredients / 5.0;
						newMovingButtons[index].blue = 1.0 - ((double)possibleDrinks[newMovingButtons[index].drinkPos].numIngredients / 5.0);
						if (g == pos) {
							centerballBalls = index;
							newMovingButtons[centerballBalls].center = 1;
						}
						index++;
					}
					if (percent <= limit)
					{
						possibleDrinks[g].visible = 0;
					}
					g++;
				}
			}
			//set the number of visible drinks
			numVisibleDrinks = index;
			numb = numVisibleDrinks;
			if (numPossibleDrinks < 10) {
				if (numPossibleDrinks == numVisibleDrinks)
					numb = 10;
			}
			if (check == 1) {
				numb = 10;
			}
			if (numb > 30) {
				limit = limit + 0.05;
				check = 1;
			}
			else {
				limit = limit - 0.05;
			}

			if (limit <= 0) {
				limit = 0.05;
				check = 1;
			}
		}
		for (z = 0; z < numVisibleDrinks; z++) {
			movingButtons[z] = newMovingButtons[z];
			possibleDrinks[movingButtons[z].drinkPos].ballPosition = z;
		}
		for (int l = 0; l < MAXDRINKS - 1; l++) {
			for (int m = 0; m < MAXDRINKS - 1; m++) {
				otherAttractions[l][m] = 0;
			}
		}
		//determine attractions between the visible objects that are not the center
		for (index = 0; index < numVisibleDrinks - 1; index++) {
			for (g = index; g < numVisibleDrinks; g++) {
				a = 0;
				count = 0;
				while (strcmp(possibleDrinks[movingButtons[index].drinkPos].ingredient[a], "null") != 0) {
					b = 0;
					while (strcmp(possibleDrinks[movingButtons[g].drinkPos].ingredient[b], "null") != 0) {
						if (strcmp(possibleDrinks[movingButtons[g].drinkPos].ingredient[b], possibleDrinks[movingButtons[index].drinkPos].ingredient[a]) == 0) {
							count++;
						}
						b++;
					}
					a++;
				}
				otherAttractions[index][g] = count / (double)(possibleDrinks[movingButtons[g].drinkPos].numIngredients);
				otherAttractions[g][index] = count / (double)(possibleDrinks[movingButtons[index].drinkPos].numIngredients);
			}
		}
		numingredients = num;
	}
	if (numVisibleDrinks == 0) {
		stateText = "";
	}
}


void setArrows(void) {
	int i, j, l, m, limit;
	double theta;
	int check1, check2;
	int combo;
	int otherDrinks, test;

	j = 0;
	clearSelected();
	clearVisibleArrowMatrix();

	for (l = 0; l < numingredients; l++) {
		for (m = l; m < numingredients; m++) {
			for (i = 0; i < numVisibleDrinks; i++) {
				check1 = 0;
				check2 = 0;
				for (j = 0; j < possibleDrinks[movingButtons[i].drinkPos].numIngredients; j++) {
					if (strcmp(possibleDrinks[movingButtons[i].drinkPos].ingredient[j], ingredients[l]) == 0) {
						check1 = 1;
					}
					else if (strcmp(possibleDrinks[movingButtons[i].drinkPos].ingredient[j], ingredients[m]) == 0) {
						check2 = 1;
					}
					if ((check1 == 1) && (check2 == 1)) {
						visibleArrowMatrix[l][m] = visibleArrowMatrix[l][m] + 1;
					}
				}
			}
		}
	}

	i = 0;
	theta = 0.0;
	limit = 0;

	for (l = 0; l < numingredients; l++) {
		for (m = l; m < numingredients; m++) {
			if ((visibleArrowMatrix[l][m] > 0) && (arrowMatrix[l][m] > 1)) {
				options[i].one = l;
				options[i].two = m;
				i++;
			}
		}
	}

	otherDrinks = numPossibleDrinks - numVisibleDrinks;
	if (otherDrinks < 4) {
		test = otherDrinks;
	}
	else {
		test = 4;
	}

	if (numVisibleDrinks < numPossibleDrinks) {
		for (j = 0; j < test; j++) {
			num = rand();
			combo = (num / 32767) * numPossibleArrows;
			arrows[j].text = ingredients[options2[combo].one];
			arrows[j].text2 = ingredients[options2[combo].two];
			arrows[j].ingredientPos[0] = options2[combo].one;
			arrows[j].ingredientPos[1] = options2[combo].two;
		}
		numArrows = test;

	}
	else {
		numArrows = 0;
	}
	arrows[0].xpos = -4.5;
	arrows[0].ypos = 3.0;
	arrows[1].xpos = 4.5;
	arrows[1].ypos = 3.0;
	arrows[2].xpos = 4.5;
	arrows[2].ypos = -3.0;
	arrows[3].xpos = -4.5;
	arrows[3].ypos = -3.0;

}


/*
 * void moveBalls (void)
 *
 * for each of the balls, determine the new position from the forces acting
 * on the ball, and redraw the ball in this position
 */
void moveBalls(void)
{
	for (int j = 0; j < numVisibleDrinks; j++) {
		if (movingButtons[j].center == 0) {
			movingButtons[j].oldposx = movingButtons[j].posx;
			movingButtons[j].oldposy = movingButtons[j].posy;
			// initialise the repulsive force
			repforce = 0.0;
			reptheta = 0;
			// add up all of the repulsive forces
			for (int a = 0; a < numVisibleDrinks; a++) {
				if (a != j) {
					// first make sure that the balls are not in the same x or y position
					// if they are, add a random amount to the position (between -0.5 and 0.5)
					if (movingButtons[j].posx == movingButtons[a].posx) {
						num = rand();
						num = (num / 32767) - 0.5;
						movingButtons[j].posx = movingButtons[j].posx + num;
					}
					if (movingButtons[j].posy == movingButtons[a].posy) {
						num = rand();
						num = (num / 32767) - 0.5;
						movingButtons[j].posy = movingButtons[j].posy + num;
					}
					// determine the distance between the two balls
					distance = sqrt((movingButtons[j].posx - movingButtons[a].posx) * (movingButtons[j].posx - movingButtons[a].posx)
						+ (movingButtons[j].posy - movingButtons[a].posy) * (movingButtons[j].posy - movingButtons[a].posy));
					// determine the repulsive force and direction of the force
					repforce1 = e / (fabs(distance) * distance);
					reptheta1 = atan2(movingButtons[j].posy - movingButtons[a].posy, movingButtons[j].posx - movingButtons[a].posx);
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
			distance = sqrt((movingButtons[j].posx * movingButtons[j].posx) + (movingButtons[j].posy * movingButtons[j].posy));
			if (j < centerballBalls) {
				attrforce = -((k*otherAttractions[j][centerballBalls])*distance);
			}
			else {
				attrforce = -((k*otherAttractions[centerballBalls][j])*distance);
			}
			attrtheta = atan2(movingButtons[j].posy, movingButtons[j].posx);
			//determine attraction to other balls based on distance
			for (int l = 0; l < numVisibleDrinks; l++) {
				if (l != j) {
					if (otherAttractions[j][l] != 0) {
						// first make sure that the balls are not in the same x or y position
						// if they are, add a random amount to the position (between -0.5 and 0.5)
						if (movingButtons[j].posx == movingButtons[l].posx) {
							num = rand();
							num = (num / 32767) - 0.5;
							movingButtons[j].posx = movingButtons[j].posx + num;
						}
						if (movingButtons[j].posy == movingButtons[l].posy) {
							num = rand();
							num = (num / 32767) - 0.5;
							movingButtons[j].posy = movingButtons[j].posy + num;
						}
						// determine the distance between the two balls
						distance = sqrt((movingButtons[j].posx - movingButtons[l].posx) * (movingButtons[j].posx - movingButtons[l].posx)
							+ (movingButtons[j].posy - movingButtons[l].posy) * (movingButtons[j].posy - movingButtons[l].posy));
						// determine the repulsive force and direction of the force
						if (j < l) {
							attrforce1 = -((k*otherAttractions[j][l])*distance);
						}
						else {
							attrforce1 = -((k*otherAttractions[l][j])*distance);
						}
						attrtheta1 = atan2(movingButtons[j].posy - movingButtons[l].posy, movingButtons[j].posx - movingButtons[l].posx);
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
			velocity = sqrt((movingButtons[j].velx * movingButtons[j].velx) + (movingButtons[j].vely * movingButtons[j].vely));
			dampforce = -(d*velocity);
			damptheta = atan2(movingButtons[j].vely, movingButtons[j].velx);
			// determine the x and y components of the force on the ball
			forcex = attrforce * cos(attrtheta) + dampforce * cos(damptheta) + repforce * cos(reptheta);
			forcey = attrforce * sin(attrtheta) + dampforce * sin(damptheta) + repforce * sin(reptheta);
			// set the force and direction for the ball
			movingButtons[j].force = sqrt((forcex * forcex) + (forcey * forcey));
			movingButtons[j].forcetheta = atan2(forcey, forcex);
			while (movingButtons[j].force > 1000) {
				movingButtons[j].force = movingButtons[j].force / 10;
			}
			// set the acceleration for the ball
			movingButtons[j].accx = movingButtons[j].force * cos(movingButtons[j].forcetheta) / movingButtons[j].mass;
			movingButtons[j].accy = movingButtons[j].force * sin(movingButtons[j].forcetheta) / movingButtons[j].mass;
			// set the velocity for the ball
			movingButtons[j].velx = movingButtons[j].velx + movingButtons[j].accx * timestep;
			movingButtons[j].vely = movingButtons[j].vely + movingButtons[j].accy * timestep;
			// set the new position of the ball
			movingButtons[j].posx = movingButtons[j].posx + movingButtons[j].velx * timestep;
			movingButtons[j].posy = movingButtons[j].posy + movingButtons[j].vely * timestep;
		}
		else {
			// put the center ball in the center position
			distance = sqrt((movingButtons[j].posx * movingButtons[j].posx) + (movingButtons[j].posy * movingButtons[j].posy));
			if (distance > 0.05) {
				attrforce = -(6 * k*distance);
				attrtheta = atan2(movingButtons[j].posy, movingButtons[j].posx);
				// determine the x and y components of the force on the ball
				forcex = attrforce * cos(attrtheta);
				forcey = attrforce * sin(attrtheta);
				// set the force and direction for the ball
				movingButtons[j].force = sqrt((forcex * forcex) + (forcey * forcey));
				movingButtons[j].forcetheta = atan2(forcey, forcex);
				while (movingButtons[j].force > 1000) {
					movingButtons[j].force = movingButtons[j].force / 10;
				}
				// set the acceleration for the ball
				movingButtons[j].accx = movingButtons[j].force * cos(movingButtons[j].forcetheta) / movingButtons[j].mass;
				movingButtons[j].accy = movingButtons[j].force * sin(movingButtons[j].forcetheta) / movingButtons[j].mass;
				// set the velocity for the ball
				movingButtons[j].velx = movingButtons[j].velx + movingButtons[j].accx * timestep;
				movingButtons[j].vely = movingButtons[j].vely + movingButtons[j].accy * timestep;
				// set the new position of the ball
				movingButtons[j].posx = movingButtons[j].posx + movingButtons[j].velx * timestep;
				movingButtons[j].posy = movingButtons[j].posy + movingButtons[j].vely * timestep;
			}
			else {
				movingButtons[j].posx = 0.0;
				movingButtons[j].posy = 0.0;
			}
		}
	}
	display();
}



void setArrowDrinkCenter(int arrowIndex) {
	int check1, check2;
	int b, g, i;
	int number;

	g = 0;
	i = 0;
	while (strcmp(possibleDrinks[g].name, "null") != 0) {
		check1 = 0;
		check2 = 0;
		b = 0;
		while (strcmp(possibleDrinks[g].ingredient[b], "null") != 0) {
			if (strcmp(possibleDrinks[g].ingredient[b], arrows[arrowIndex].text) == 0) {
				check1 = 1;
			}
			if (strcmp(possibleDrinks[g].ingredient[b], arrows[arrowIndex].text2) == 0) {
				check2 = 1;
			}
			b++;
		}
		if ((check1 == 1) && (check2 == 1)) {
			// put the new possible drinks in a selected array
			arrowDrinks[i] = g;
			i++;
		}
		g++;
	}
	// choose a random drink from these possible drinks, then set the visible drinks
	num = rand();
	num = (num / 32767) * i;
	number = (int)num;
	setVisibleDrinks(-1, arrowDrinks[number]);
	setArrows();


}

void setRandomDrinkCenter() {

	int number;

	num = rand();
	num = (num / 32767) * numPossibleDrinks;
	number = (int)num;
	setVisibleDrinks(-1, number);
}

int findLetter(int letterNum) {
	int i, count, letter;
	letter = 0;
	count = 0;
	for (i = 0; i < 26; i++) {
		if (alphabetInfo[i] != 0) {
			if (count == letterNum) {
				letter = i;;
			}
			count++;
		}
	}
	return letter;
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
		if (ball == next) {
			setAllPossibleDrinks();
			setVisibleDrinks(-1, -1);
			choicesBalls();
			state = CHOICES;
		}
		break;
	case CHOICES:
		if (ball == next) {
			ingredientsSet = 0;
			clearSelected();
			drawIngredients(ingredientsSet);
			state = INGREDIENTS;
		}
		else if (ball == next2) {
			set = 0;
			setAllPossibleDrinks();
			stateText = "";
			setVisibleDrinks(-1, -1);
			chosenDrink = 0;
			dynamic = 1;
			setArrows();
			letterState = 0;
			showDrinks();
			state = DRINKS;
		}
		else if (ball == next4) {
			setAlphabet();
			showAlphabet();
			state = ALPHABET;
		}
		else if (ball == back) {
			openingBalls();
			state = INTRO;
		}
		break;
	case INGREDIENTS:
		if (ball == back) {	// Cancel
			choicesBalls();
			state = CHOICES;
		}
		else if (ball == next) {	// View Drinks
			set = 0;
			setPossibleDrinks();
			stateText = "containing";
			setVisibleDrinks(-1, 0);
			setArrows();
			letterState = 0;
			showDrinks();
			chosenDrink = 0;
			state = DRINKS;
		}
		else if (ball == moreDrinks) {
			ingredientsSet = ingredientsSet + 1;
			drawIngredients(ingredientsSet);
		}
		else if (ball == lastDrinks) {
			ingredientsSet = ingredientsSet - 1;
			drawIngredients(ingredientsSet);
		}
		else if ((ball < numIngredientsDisplayed) && (ball >= 0)) {
			if (buttons[ball].selected == 0) {
				buttons[ball].red = 0.0;
				buttons[ball].blue = 1.0;
				buttons[ball].selected = 1;
				selected[(20 * ingredientsSet + ball)] = 1;
			}
			else {
				buttons[ball].red = 1.0;
				buttons[ball].blue = 0.0;
				buttons[ball].selected = 0;
				selected[(20 * ingredientsSet + ball)] = 0;
			}
		}
		break;
	case DRINKS:
		if (ball == next) {				// random drink
			setRandomDrinkCenter();
			setArrows();
			letterState = 0;
			showDrinks();
		}
		else if (ball == next2) {				// next drink
			if (centerballDrinks < numPossibleDrinks - 1) {
				setVisibleDrinks(-1, centerballDrinks + 1);
			}
			else {
				setVisibleDrinks(-1, 0);
			}
			setArrows();
		}
		else if (ball == next3) {				// last drink
			if (centerballDrinks > 0) {
				setVisibleDrinks(-1, centerballDrinks - 1);
			}
			else {
				setVisibleDrinks(-1, numPossibleDrinks - 1);
			}
			setArrows();
		}
		else if (ball == back) {				// cancel
			dynamic = 0;
			setAllPossibleDrinks();
			setVisibleDrinks(-1, -1);
			setArrows();
			choicesBalls();
			state = CHOICES;
		}
		else if (ball == moreDrinks) {			// next letter
			number = displayedLetter + 1;
			if (number == 26) {
				number = 0;
			}
			while (alphabetInfo[number] == 0) {
				number++;
				if (number == 26) {
					number = 0;
				}
			}
			setPossibleAlphabetDrinks(number);
			stateText = "starting with";
			displayedLetter = number;
			letterState = 1;
			setVisibleDrinks(-1, -1);
			setArrows();
			showDrinks();
			state = DRINKS;
		}
		else if (ball == lastDrinks) {			// last letter
			number = displayedLetter - 1;
			if (number == -1) {
				number = 25;
			}
			while (alphabetInfo[number] == 0) {
				number--;
				if (number == -1) {
					number = 25;
				}
			}
			setPossibleAlphabetDrinks(number);
			stateText = "starting with";
			displayedLetter = number;
			letterState = 1;
			setVisibleDrinks(-1, -1);
			setArrows();
			showDrinks();
			state = DRINKS;
		}
		break;
	case ALPHABET:
		if (ball == back) {
			choicesBalls();
			state = CHOICES;
		}
		else if ((ball < (numButtons)) && (ball >= 0)) {
			number1 = numLetters - (((numButtons)-ball) / 2);
			number = findLetter(number1);
			setPossibleAlphabetDrinks(number);
			stateText = "starting with";
			displayedLetter = number;
			letterState = 1;
			setVisibleDrinks(-1, -1);
			setArrows();
			showDrinks();
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
			for (b = 0; b < numButtons; b++) {
				if (((buttons[b].posx - buttons[b].size) <= mouseposx)
					&& ((buttons[b].posx + buttons[b].size) >= mouseposx)) {
					if (((buttons[b].posy - buttons[b].size) <= mouseposy)
						&& ((buttons[b].posy + buttons[b].size) >= mouseposy)) {
						c = b;
						d = 1;

					}
				}
			}
			if (d == 1) {
				if (dynButton == 0) {
					dynLastButton = 0;
				}
				else {
					dynLastButton = 1;
				}
				dynButton = 0;
				action(c);
				display();

			}

			d = 0;
			c = -1;

			if (dynamic == 1) {
				for (b = 0; b < numVisibleDrinks; b++) {
					if (movingButtons[b].posy <= -yline + 0.7) {
						yi = -yline + 0.7;
					}
					else if (movingButtons[b].posy >= yline - 0.7) {
						yi = yline - 0.7;
					}
					else {
						yi = movingButtons[b].posy;
					}
					if ((((movingButtons[b].posx - movingButtons[b].size - 1.0)) <= mouseposx)
						&& (((movingButtons[b].posx + movingButtons[b].size - 1.0)) >= mouseposx)) {
						if ((((yi - movingButtons[b].size + 1.0)) <= mouseposy)
							&& (((yi + movingButtons[b].size + 1.0)) >= mouseposy))
						{
							d = 1;
							c = b;
						}
					}
				}

				if (d == 1) {
					setVisibleDrinks(c, 0);
					setArrows();
				}

				for (b = 0; b < numArrows; b++) {
					if (((arrows[b].xpos - 0.3 - 1.0) <= mouseposx) && ((arrows[b].xpos + 0.3 - 1.0) >= mouseposx)
						&& ((arrows[b].ypos - 0.3 + 1.0) <= mouseposy) && ((arrows[b].ypos + 0.3 + 1.0) >= mouseposy)) {
						setArrowDrinkCenter(b);
					}
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
	initIngredients();
	initDrinks();
	setAllPossibleDrinks();
	setVisibleDrinks(-1, 0);
	openingBalls();
	// set the random seed
	srand((unsigned)time(NULL));
	// set up the display
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(moveBalls);
	glutMainLoop();
	return(0);
}