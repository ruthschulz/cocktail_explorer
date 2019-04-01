#ifndef DRAW_H_
#define DRAW_H_

#pragma once
#include "Ball.h"
#include "DynamicBall.h"
#include "Drink.h"
#include "DrinkList.h"

class Draw
{
	double k = 2;				// spring constant
	double d = 2;				// damping constant
	double e = 40;				// repulsion constant
	double mass = 0.1f;			// mass of the balls
	double timestep = 0.002f;		// timestep
	double xleft = -3.0;		// left side of the screen
	double xcenter = 0.0;		// center of the screen
	double yinit = 3.5;			// towards the top of the screen
	std::map<std::pair<std::string, std::string>, double> otherAttractions;
	std::vector<Ball> buttons;	// the static buttons
	std::vector<DynamicBall> movingButtons;	// the moving buttons of drinks
public:
	int next, back, moreDrinks, lastDrinks, next2, next3, next4, back2; // used with buttons to check which button was pressed

public:
	Draw();
	~Draw();
	int setVisibleDrinks(std::string name, DrinkList& drinks);
	void openingBalls(void);
	void choicesBalls(void);
	void showDrinks(bool drinksToShow, bool showLetters, char letter, DrinkList& drinks);
	void showAlphabet(DrinkList& drinks);
	int drawIngredients(int ingrSet, DrinkList& drinks);
	int numButtons(void);
	int numMovingButtons(void);
	std::string firstVisibleDrinkName(void);
	void moveBalls(void);
	DynamicBall getMovingBall(int index);
	Ball getBall(int index);
	void changeSelection(int index);
};

#endif
