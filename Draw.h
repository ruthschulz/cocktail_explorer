#ifndef DRAW_H_
#define DRAW_H_

#pragma once
#include "Ball.h"
#include "DynamicBall.h"
#include "Drink.h"

class Draw
{
	double x1 = -3.0;
	double x2 = 2.0;
	double x3 = -1.0;
	double x4 = 0.0;
	double x5 = 2.0;
	double yinit = 3.5;
public:
	int next, back, moreDrinks, lastDrinks, next2, next3, next4, back2; // used with buttons to check which button was pressed
	std::vector<Ball> buttons;
	std::vector<DynamicBall> movingButtons;
	std::map<std::pair<std::string, std::string>, double> otherAttractions;

public:
	Draw();
	~Draw();
	int setVisibleDrinks(std::string name, std::map<std::string, Drink, cicompare> &possibleDrinks);
	void openingBalls(void);
	void choicesBalls(void);
	void showDrinks(bool drinksToShow, bool showLetters, char letter, std::map<char, int> &alphabetInfo);
	void showAlphabet(std::map<char, int> &alphabetInfo);
	int drawIngredients(int ingrSet, std::set<std::string> &ingredients, std::set<std::string>& selected);
	void setRandomDrinkCenter(std::map<std::string, Drink, cicompare>& possibleDrinks);
};

#endif
