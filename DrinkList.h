#ifndef DRINKLIST_H_
#define DRINKLIST_H_

#pragma once
#include "Drink.h"
#include "DynamicBall.h"
#include <utility>

class DrinkList
{
	std::map<std::string, Drink, cicompare> drinks;	// all drinks read in from file
	std::map<std::string, Drink, cicompare> possibleDrinks;	// current set of possible drinks (all, starting with letter, or including ingredient)
	std::map<char, int> alphabetInfo;	// number of drinks starting with each letter
	double xleft = -3.0;	// left side of screen
	double xright = 2.0;	// right side of screen
	double yinit = 3.5;		// towards the top of the screen
	std::set<std::string> ingredients;	// all ingredients used in drinks read in from file
	std::set<std::string> selectedIngr;	// ingredients selected by user to put in possible drinks

public:
	DrinkList();
	~DrinkList();
	int initDrinks(void);
	int setAllPossibleDrinks(void);
	int setPossibleAlphabetDrinks(int letter);
	int setPossibleDrinks(void);
	void setAlphabet(void);
	void clearSelected(void);
	int findLetter(int letterNum);
	void addSelectedIngredient(std::string name);
	void removeSelectedIngredient(std::string name);
	Drink getDrink(std::string name);
	std::string getNextDrinkName(std::string name);
	std::string getLastDrinkName(std::string name);
	void setVisible(std::string name, int visible);
	std::vector<DynamicBall> setMovingBalls(std::string name);
	std::string setRandomDrinkCenter(void);
	int numPossibleDrinks(void);
	int numSelectedIngr(void);
	int numIngredients(void);
	int numLettersUsed(void);
	std::string firstDrinkName(void);
	int nextLetter(int letter);
	int lastLetter(int letter);
	bool drinksWithLetter(int letter);
	std::vector<Ball> setIngrBalls(int ingrSet);
	std::string getSelectedIngr(int index);
};

#endif
